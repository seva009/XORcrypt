#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <chrono>

using namespace std;

const int keyPos = 15;
const int blockSize = 256;

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
    vector<short> result;
    for (int i = 0; i < block.size(); i++){
        result.push_back(block[i] ^ key[i + pos]);
    }
    return result;
}
    
void writeKeyFile(vector<char> key, fstream &file, int size) {
    const string keyStr = to_string(size);
    for (int i = 0; i < keyPos - keyStr.length(); i++) {
        file << 0;
    }
    file << keyStr;
    for (int i = 0; i < key.size(); i++) {
        file << key[i];
    }
}

vector<vector<short>> splitSrcFile(fstream &file) {
    int size = getFileSize(file); // size of file
    int fBlocks = (size - (size % blockSize)) / blockSize; // number of full blocks
    cout << "Number of full blocks: " << fBlocks << endl;
    int endBlock = size % blockSize; // size of last block
    cout << "Size of last block: " << endBlock << endl;
    vector<vector<short>> result;
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

vector<char> getKeyFromFile(fstream &file) {
    vector<char> key;
    for (int i = keyPos + 1; i < getFileSize(file); i++) {
        key.push_back(file.get());
    }
    file.seekg(0, ios::beg);
    return key;
}

int getFileSizeFromKeyFile(fstream &file, fstream &key) {
    int size = getFileSize(file);
    int keySize = getFileSize(key);
    if (keySize - size == keyPos) {
        return size;
    }
    cout << "Wrong key size" << endl;
    return -1;
}

unsigned int passToSeed(string password) {
    string out;
    for (int i = 0; i < password.length(); i++) {
        out += (int)password[i];
    }
    return stoi(password) % 4294967295 + 1;
}

int main(int argc, char *argv[]) {
    cout << "Enter file name: ";
    string password;
    string file;
    cin >> file;
    cout << "Enter password: ";
    cin >> password;
    fstream cr(file);
    if (!cr.is_open()) {
        cout << "File not found" << endl;
        return 1;
    }
    cout << "Starting..." << endl;
    auto start = std::chrono::high_resolution_clock::now();
    vector<vector<short>> blocks = splitSrcFile(cr);
    int scrSize = getFileSize(cr);
    vector<char> key;
    key = genByteVec(scrSize, passToSeed(password));
    cout << "Scr file size: " << scrSize << endl;
    for (int i = 0; i < blocks.size(); i++) {
        blocks[i] = xorBlock(key, blocks[i]);
        cout << "XORing blocks " << i << "/" << blocks.size() - 1 << endl;
    }
    fstream out(file, ios::out | ios::in);
    for (int i = 0; i < blocks.size(); i++) {
        cout << "Writing block: " << i << '/' << blocks.size() << endl;
        for (int j = 0; j < blocks[i].size(); j++) {
            out.put(blocks[i][j]);
        }
    }
    out.close();
    auto end = std::chrono::high_resolution_clock::now();
    cout << "Time to crack: " << (std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() + 1) * passToSeed(password) * 2 / 1000 << " sec" << endl;
    return 0;
}