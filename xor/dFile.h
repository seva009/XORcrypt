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
    mt19937_64 eng;
    private:
        void* genKey(unsigned int seed, size_t size) {
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
            if (md5hash.length() < 8) {
                throw std::runtime_error("Invalid hash length");
            }

            std::string subHash = md5hash.substr(0, 8);
            unsigned int result = std::stoul(subHash, nullptr, 16);

            return result;
        }

        unsigned int MD5HashToUInt(const std::string& str) {
            std::string hashStr = md5(str); // Вызов функции md5 для получения хеша MD5
            unsigned int result = htoui(hashStr);

            return result;
        }

        void wipeKey(void* memKey, size_t size) {
            for (int i = 0; i < size; i++) {
                ((unsigned char*)memKey)[i] = 0;
                cout << "Wiping keychar: " << i << "/" << size - 1 << endl;
            }
        }
    public:
        size_t size = 0;
        dFile file;
        void* memKey;
        void init(string filename, string password) {
            file.Create(filename);
            cout << "File created" << endl;
            file.loadFile();
            cout << "File loaded" << endl;
            size = file.getLoadedSize();
            cout << "File size: " << size << endl;
            unsigned int hash = MD5HashToUInt(password);
            memKey = genKey(hash, size);
            cout << "Key generated" << endl;
        }

        void cryptFile() {
            #pragma omp parallel for
            for (int i = 0; i < size; i++) {
                ((unsigned char*)file.memFilePtr)[i] ^= ((unsigned char*)this->memKey)[i];
            }
        }

        void saveFile() {
            file.file.write((char*)file.memFilePtr, size);
        }

        void setThreads(int numThreads) {
            omp_set_num_threads(numThreads);
        }

        void wipe() {
            file.clear();
            memset(memKey, 0, size);
            cout << ((char*)memKey)[1] << ((char*)memKey)[size-1] << endl;
            free(memKey);
        }
};