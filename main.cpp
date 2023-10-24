#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <openssl/sha.h>
#include <string>
#include <algorithm>
#include <omp.h>

using namespace std;

const int keyPos = 15;
const int blockSize = 256;

unsigned int SHA1Hash(const std::string& str) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(str.c_str()), str.length(), hash);

    unsigned int result = 0;
    for (int i = 0; i < sizeof(unsigned int); i++) {
        result <<= 8;
        result |= hash[i];
    }

    return result;
}

int getFileSize(fstream &file){
    file.seekg(0, ios::end);
    int size = file.tellg();
    file.seekg(0, ios::beg);
    return size;
}

vector<char> genByteVec(int len, int seed = time(NULL), int size = 256){
    srand(seed);
    vector<char> ranKey;
    for (int i = 0; i < len; i++){
        ranKey.push_back(static_cast<char>(rand() % 94) + 33);
    }
    return ranKey;
}

vector<short> xorBlock(vector<char> key, vector<short> block, int pos = 0){
    vector<short> result(block.size());
    std::transform(block.begin(), block.end(), key.begin() + pos, result.begin(), std::bit_xor<short>());
    return result;
}

vector<vector<short>> splitSrcFile(fstream &file) {
    int size = getFileSize(file); // size of file
    int fBlocks = (size - (size % blockSize)) / blockSize; // number of full blocks
    int endBlock = size % blockSize; // size of last block
    vector<vector<short>> result;
    result.reserve(fBlocks + 1);
    for (int i = 0; i < fBlocks; i++) { // pushing full blocks into result
        vector<short> block;
        for (int j = 0; j < blockSize; j++) {
            block.push_back(file.get());
        }
        result.push_back(block);
    }
    for (int i = 0; i < endBlock; i++) { // pushing last block into result
        vector<short> block;
        block.push_back(file.get());
        result.push_back(block);
    }
    return result;
}

unsigned int passToSeed(string password) { return SHA1Hash(password); }

int main(int argc, char *argv[]) {
    cout << "Enter file name: ";
    string password;
    string file;
    cin >> file;
    cout << "Enter password: ";
    cin >> password;
    int numThreads;
    cout << "Enter the number of threads: ";
    cin >> numThreads;
    omp_set_num_threads(numThreads);
    fstream cr(file);
    if (!cr.is_open()) {
        cout << "File not found" << endl;
        return 1;
    }
    cout << "Starting..." << endl;
    auto start = std::chrono::high_resolution_clock::now();
    vector<vector<short>> blocks = splitSrcFile(cr);
    int scrSize = getFileSize(cr);
    cr.close();
    vector<char> key;
    key = genByteVec(scrSize, passToSeed(password));
    cout << "Scr file size: " << scrSize << endl;
    const int blockCount = 512;

    #pragma omp parallel for
    for (int i = 0; i < blocks.size(); i += blockCount) {
        int end = min(i + blockCount, (int)blocks.size());
        for (int j = i; j < end; j++) {
            blocks[j] = xorBlock(key, blocks[j]);
            cout << "XORing block " << j << "/" << blocks.size() - 1 << endl;
        }
    }

    fstream out(file, ios::out | ios::in);

    for (int i = 0; i < blocks.size(); i += blockCount) {
        int end = min(i + blockCount, (int)blocks.size());
        for (int j = i; j < end; j++) {
            cout << "Writing block: " << j << '/' << blocks.size() << endl;
            for (int k = 0; k < blocks[j].size(); k++) {
                out.put(blocks[j][k]);
            }
        }
    }

    out.close();
    auto end = std::chrono::high_resolution_clock::now();
    int64_t time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    cout << "Crypt time: " << time << endl;
    cout << "Time to crack: " << time * passToSeed(password) / 500 << " sec" << endl;
    return 0;
}