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
#include <vector>
#include <map>
using namespace std;

class BagOfWords{
/*
============================================================================================
                                       ATRIBUTES
============================================================================================
*/
int num_books=0;
vector<map<string,int>> listOfMaps;

/*
============================================================================================
                          Receives the file from the python script
                (It reads the number of books and the location of each file)
============================================================================================
*/

void processFile(const string& path) { //won't modify the String, therefore is a constant, and "&" to avoid making copies
    ifstream file(path);
    string line;
    map<string,int> global_map;
    listOfMaps.push_back(global_map);
    //tries to open the file
    if (!file.is_open()) {
        cout << "Could not open the file." << endl;
        return;
    }
    getline(file, line); //reads the first line, to get the num of books.
    int numBooks = stoi(line); 
    //with the number of books, it creates n maps, to have individual bag of words for each book, 
    //it also fills the "global" map with all the words
    string bookPath; //variable to keep the paths Strings
    for(int i=1;i<=numBooks;i++){
        getline(file,bookPath);
        map<string,int> map;
        listOfMaps.push_back(map); //creates the map in the array
        file.close(); //closes the last file
        ifstream file(bookPath); //opens new file

        if (!file.is_open()) {
            cout << "Could not open the file." << endl;
            return;
        }
        
        //puts each unique word in the map i.
        while (getline(file, line)) {
            istringstream stream(line);
            string word;

            while (stream >> word) {
                if (listOfMaps[i].find(word) == listOfMaps[i].end()) { // If the word isn't in the map
                    listOfMaps[i][word] = 0;  //it saves it with the value of 0.
                }
                if (listOfMaps[0].find(word) == listOfMaps[0].end()) { // find the word in the global map.
                    listOfMaps[0][word] = 0;
                }
            }
        }
    }

    file.close();
}

    //Executes the WordCounters.cpp script
    //giving the array of maps as parameter.
    

};