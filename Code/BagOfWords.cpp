/*Gerardo Moguel


============================================================================================
This class generates a map of every unique word used in the books.
This maps are used in the "word counter" algorithms.
============================================================================================


============================================================================================
                                        IMPORTS
============================================================================================
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cctype> 
#include <vector>
#include <map>
using namespace std;

class BagOfWords{
/*
============================================================================================
                                       ATRIBUTES
============================================================================================
*/
int numBooks;
map<string,vector<int>> bagOfWords;


/*
============================================================================================
                                      Word Cleaner
============================================================================================
*/
public:
string cleanWord(string word) {
    // Replace punctuation with spaces
    for (char& c : word) {
        if (ispunct(c)) {
            c = ' ';
        }
    }

    // Convert to lowercase
    transform(word.begin(), word.end(), word.begin(), ::tolower);

    return word;
}
/*
============================================================================================
                          Receives the file from the python script
                (It reads the number of books and the location of each file)
============================================================================================
*/
public:
void processFile(const string& path) { //won't modify the String, therefore is a constant, and "&" to avoid making copies
    ifstream file(path);
    string line;
    //tries to open the file
    if (!file.is_open()) {
        cout << "Could not open the file." << endl;
        return;
    }
    getline(file, line); //reads the first line, to get the num of books.
    numBooks = stoi(line); 
    string bookPath; //variable to keep the paths Strings
    for(int i=0;i<numBooks;i++){
        getline(file,bookPath);
        ifstream bookfile(bookPath); //opens new file
        if (!bookfile.is_open()) {
            cout << "Could not open the file." << endl;
            return;
        }
        
        //puts each unique word in the map
        while (getline(bookfile, line)) {
            line = cleanWord(line);
            istringstream stream(line);
            string word;
            while (stream >> word) {
                if (bagOfWords.find(word) == bagOfWords.end()) { // If the word isn't in the map
                    bagOfWords[word] = vector<int>(numBooks,0); //initizalices in 0 all the books.
                }
            }
        }
    bookfile.close();//closes the book file
    }
    file.close(); //closes the "paths" file
    // Write to file in Outputs folder
        string outputPath = R"(D:\Documentos\GitHub\ParralelWordCounter_MPI\Outputs\file_maps.txt)";
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
        for (const auto& [word, counts] : bagOfWords) {
            out << word;
            for (int count : counts) {
                out << "\t" << "0";
            }
            out << "\n";
        }

        out.close();
}
};

//Main
int main(int argc, char* argv[]) { //receives parameter from python
    string filePaths = argv[1];
    BagOfWords a;
    a.processFile(argv[1]);
    return 0;
}