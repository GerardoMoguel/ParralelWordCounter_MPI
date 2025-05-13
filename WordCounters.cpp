/*Gerardo Moguel


============================================================================================
This class counts every word used in every book. 
============================================================================================
*/

/*
============================================================================================
                                        IMPORTS
============================================================================================
*/
#include <iostream> 
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
using namespace std;

class WordCounters{

/*
============================================================================================
                                       ATRIBUTES
============================================================================================
*/
map<string, vector<int>> wordMap; //map with the bag of words

/*
============================================================================================
                                TURNS .TXT FILE INTO MAP
============================================================================================
*/
public:
void fileToMap(const string& filename) {
    ifstream infile(filename);

    if (!infile.is_open()) {
        cout << "Could not open the file." << endl;
        return;
    }
    string line;

    // Skip the header line
    getline(infile, line);

    while (getline(infile, line)) {
        stringstream ss(line);
        string word;
        ss >> word; //the first word is the "word"

        vector<int> counts;
        int count;
        while (ss >> count) { //number by number
            counts.push_back(count);
        }
        wordMap[word] = counts;
    }
}


/*
============================================================================================
                                    SERIAL COUNTER
============================================================================================
*/
void serialCounter(const string& filename){
        ifstream books(filename);

    if (!books.is_open()) {
        cout << "Could not open the file." << endl;
        return;
    }
    string line;
    getline(books,line); //the first line tells the number of books to analize.
    int numBooks = stoi(line);
    string book;
    for(int i=0;i<numBooks;i++){

        getline(books,book);
        ifstream bookfile(book); //opens new file
        if (!bookfile.is_open()) {
            cout << "Could not open the file." << endl;
            return;
        }
        //puts each unique word in the map
        while (getline(bookfile, line)) {
            istringstream stream(line);
            string word;

            while (stream >> word) {
                if (wordMap.find(word) != wordMap.end()) { // If the word is in the map
                    wordMap[word][i] +=1; //iif it finds it, it adds one count
                }
            }
        }
        bookfile.close();
    }
    // Write to file in Outputs folder
    string outputPath = R"(D:\Documentos\GitHub\ParralelWordCounter_MPI\Outputs\file_map_serial.txt)";
    ofstream out(outputPath);
    if (!out.is_open()) {
        cout << "Failed to write to output file." << endl;
        return;
    }

    // Header
    out << "word";
    for (int i = 0; i < numBooks; i++) {
        out << "\tbook" << i;
    }
    out << endl;

    // Content
    for (const auto& [word, counts] : wordMap) {
        out << word;
        for (int count : counts) {
            out << "\t" << count;
        }
        out << "\n";
    }
    out.close();
    books.close();
}

/*
============================================================================================
                                   PARALLEL COUNTER
============================================================================================
*/

/*
============================================================================================
                                OUTPUT FILE GENERATOR
============================================================================================
*/
};

int main(int argc, char* argv[]) { //receives parameter from python
    string filePath = argv[2];
    string maps=argv[1];
    WordCounters a;
    a.fileToMap(maps);
    a.serialCounter(filePath);
    return 0;
}