#include "dFile.h"

int main() {
    string password;
    string file;
    int numThreads;
    char wipe;
    cout << "Enter file name: ";
    cin >> file;
    cout << "Enter password: ";
    cin >> password;
    cout << "Enter the number of threads: ";
    cin >> numThreads;
    cout << "Do you want to wipe the key? (y/n): ";
    cin >> wipe;
    Crypt crypt;
    cout << "Starting..." << endl;
    crypt.setThreads(numThreads);
    cout << "Threads set" << endl;
    crypt.init(file, password);
    cout << "Init done" << endl;
    crypt.cryptFile();
    cout << "Crypt done" << endl;
    crypt.saveFile();
    cout << "Save done" << endl;
    if (wipe == 'y') {
        crypt.wipe(true);
    }
    else {
        crypt.wipe(false);
    }
    return 0;
}