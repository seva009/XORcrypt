#include <fstream>
#include <cmath>
#include <omp.h>
#include <iostream>
#include "md5.h"
#include <random>

using namespace std;

struct dFile {  
    public:
        fstream file;
        void* memFilePtr;
        void loadFile() {
            void * memFilePtr = malloc(_getFileSize(file));
            file.read((char*)memFilePtr, _getFileSize(file));
            this->memFilePtr = memFilePtr;
        }
        
        size_t getLoadedSize() {
            return _getFileSize(file);
        }

        void Create(string filename) {
            file.open(filename, ios::in | ios::out);
        }

        void clear() {
            file.close();
            free(memFilePtr);
        }
    private:
        size_t _getFileSize(fstream &file) {
            if (!file.is_open()) {
                throw std::runtime_error("File not opened");
            }
            file.seekg(0, ios::end);
            streampos pos = file.tellg();
            file.seekg(0, ios::beg);
            return static_cast<size_t>(pos);
        }
};

struct Crypt {
    mt19937_64 gen1;
    mt19937_64 gen2;
    private:
        void* genKey(unsigned int seed, size_t size, mt19937_64& eng) {
            void* memKey = malloc(size);
            if (memKey == nullptr) {
                return NULL;
            }
            eng.seed(seed);
            for (int i = 0; i < size; i++) {
                ((unsigned char*)memKey)[i] = eng() % 256;
            }
            return memKey;
        }

        unsigned int htoui(const std::string& md5hash) {
            std::string subHash = md5hash.substr(0, 8);
            unsigned int result = std::stoul(subHash, nullptr, 16);

            return result;
        }

        unsigned int MD5HashToUInt(const std::string& str) {
            std::string hashStr = md5(str); // Вызов функции md5 для получения хеша MD5
            unsigned int result = htoui(hashStr);

            return result;
        }
    public:
        size_t size = 0;
        dFile file;
        bool hasNd = false;
        void* memKey;
        void* ndMemKey;
        void init(string filename, string password, string ndPassword) {
            hasNd = true;
            file.Create(filename);
            cout << "File created" << endl;
            file.loadFile();
            cout << "File loaded" << endl;
            size = file.getLoadedSize();
            cout << "File size: " << size << endl;
            unsigned int hash = MD5HashToUInt(password);
            unsigned int ndHash = MD5HashToUInt(ndPassword);
            memKey = genKey(hash, size, gen1);
            ndMemKey = genKey(ndHash, size, gen2);
            cout << "Key generated" << endl;
        }

        void init(string filename, string password) {
            file.Create(filename);
            cout << "File created" << endl;
            file.loadFile();
            cout << "File loaded" << endl;
            size = file.getLoadedSize();
            cout << "File size: " << size << endl;
            unsigned int hash = MD5HashToUInt(password);
            memKey = genKey(hash, size, gen1);
            cout << "Key generated" << endl;
        }

        void cryptFile() {
            if (hasNd) {
                #pragma omp parallel for
                for (int i = 0; i < size; i++) {
                ((unsigned char*)file.memFilePtr)[i] ^= ((unsigned char*)this->memKey)[i];
                ((unsigned char*)file.memFilePtr)[i] ^= ((unsigned char*)this->ndMemKey)[i];
            }
            }
            else {
                #pragma omp parallel for
                for (int i = 0; i < size; i++) {
                    ((unsigned char*)file.memFilePtr)[i] ^= ((unsigned char*)this->memKey)[i];
                }
            }
        }

        void saveFile() {
            file.file.write((char*)file.memFilePtr, size);
        }

        void setThreads(int numThreads) {
            omp_set_num_threads(numThreads);
        }

        void wipe() {
            memset(memKey, 0, size);
            free(memKey);
            if (hasNd) {
                memset(ndMemKey, 0, size);
                free(ndMemKey);
            }
        }

        void fileWipe() {
            file.clear();
        }
};