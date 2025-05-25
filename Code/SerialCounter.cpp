#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <chrono>
#include <iomanip>
using namespace std;

class SerialCounter {
    map<string, vector<int>> wordMap;

    void fileToMap(const string& fileName) {
        wordMap.clear();
        ifstream infile(fileName);
        if (!infile.is_open()) {
            cerr << "Could not open map file: " << fileName << endl;
        }
        string line;
        getline(infile, line); // skip header
        while (getline(infile, line)) {
            stringstream ss(line);
            string word;
            ss >> word;
            vector<int> counts;
            int count;
            while (ss >> count) counts.push_back(count);
            wordMap[word] = counts;
        }
    }

    void serialCounter(const string& fileName) {
        ifstream books(fileName);
        if (!books.is_open()) {
            cerr << "Could not open paths file: " << fileName << endl;
            return;
        }

        string line;
        getline(books, line);
        int numBooks = stoi(line);
        string book;
        map<string, vector<int>> localMap;

        for (int i = 0; i < numBooks; ++i) {
            getline(books, book);
            ifstream bookfile(book);
            if (!bookfile.is_open()) {
                cerr << "Could not open book: " << book << endl;
                return;
            }

            while (getline(bookfile, line)) {
                istringstream stream(line);
                string word;
                while (getline(stream, word, ',')) {
                    if (localMap.find(word) == localMap.end()) {
                        localMap[word] = vector<int>(numBooks, 0);
                    }
                    localMap[word][i]++;
                }
            }
        }

        ofstream out("Outputs/file_map_serial.txt");
        out << "word";
        for (int i = 0; i < numBooks; ++i) out << "\tbook" << i;
        out << "\n";
        for (auto& [w, counts] : localMap) {
            out << w;
            for (int c : counts) out << "\t" << c;
            out << "\n";
        }
    }
};