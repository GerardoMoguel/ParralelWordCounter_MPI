/*Gerardo Moguel


============================================================================================
This class generates a unordered_map of every unique word used in the books.
This hashmaps are used in the "word counter" algorithms.

This process will be made serial and parallel, because i got curious if it can be paralelized,
and how this affects the execution time.
Thanks for reading.
============================================================================================


============================================================================================
                                        IMPORTS
============================================================================================
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
using namespace std;

class BagOfWords{
/*
============================================================================================
                                       ATRIBUTES
============================================================================================
*/
int num_books=0;
unordered_map<string, int> wordCount;


/*
============================================================================================
                          Receives the file from the python script
                (It reads the number of books and the location of each file)
============================================================================================
*/

void processFile(const string& path) {
    ifstream file(path);
    string line;

    if (!file.is_open()) {
        cout << "Could not open the file." << endl;
        return;
    }

    while (getline(file, line)) {
        istringstream stream(line);
        string word;

        while (stream >> word) {
            cout << "Word: " << word << endl;
        }
    }

    file.close();
}

int main() {
    string path = "test.txt";
    processFile(path);  // Call the void method
    return 0;
}

    //Generates the hashmaps of each book

    //Join the hashmaps together for global analisis

    //Executes the WordCounters.cpp script

};