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
#include <mpi.h>
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
void mpiCounter(const string& filename){
    
    MPI_Init(NULL, NULL);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int numBooks;
    vector<string> bookList;

    //the Master reads the filenames and broadcasts them
    if(rank == 0) {
        ifstream books(filename);
        if (!books.is_open()) {
            cout << "Could not open the file." << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        string line;
        getline(books, line);
        numBooks = stoi(line); //get the number of books

        if(numBooks != size) { //in case the number of books is not the same number as process.
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        bookList.resize(numBooks); //adds to the list of book paths
        for(int i = 0; i < numBooks; i++) {
            getline(books, bookList[i]);
        }
        books.close();
    }

    // Broadcast numBooks
    MPI_Bcast(&numBooks, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Prepare to broadcast book filenames
    if(rank != 0) bookList.resize(numBooks);

    for(int i = 0; i < numBooks; i++) {
        int len;
        if(rank == 0){
            len = bookList[i].size();
        }
        MPI_Bcast(&len, 1, MPI_INT, 0, MPI_COMM_WORLD);

        bookList[i].resize(len);
        MPI_Bcast(bookList[i].data(), len, MPI_CHAR, 0, MPI_COMM_WORLD);
    }

    // Each process analyzes exactly one book
    string book = bookList[rank];
    ifstream bookfile(book);
    if (!bookfile.is_open()) {
        return;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    string line;
    while (getline(bookfile, line)) {
        istringstream stream(line);
        string word;
        while (stream >> word) {;
            wordMap[word][rank] += 1;
        }
    }
    bookfile.close();

    // Gather all data on master
    if(rank == 0) {
        for(int p = 1; p < size; p++) { //"p" from process... starts in 1, since the process 0 is master :p
            for(const auto& [word, counts] : wordMap) {
                int wordLen;
                MPI_Recv(&wordLen, 1, MPI_INT, p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                string recvWord(wordLen, ' ');
                MPI_Recv(recvWord.data(), wordLen, MPI_CHAR, p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                int count;
                MPI_Recv(&count, 1, MPI_INT, p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                wordMap[recvWord][p] = count;
            }
        }

        // Write combined results
        string outputPath = R"(D:\Documentos\GitHub\ParralelWordCounter_MPI\Outputs\file_map_parallel.txt)";
        ofstream out(outputPath);
        if (!out.is_open()) {
            cout << "Failed to write to output file." << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        out << "word";
        for (int i = 0; i < numBooks; i++) {
            out << "\tbook" << i;
        }
        out << endl;

        for (const auto& [word, counts] : wordMap) {
            out << word;
            for (int count : counts) {
                out << "\t" << count;
            }
            out << "\n";
        }
        out.close();

    } else {
        for(const auto& [word, counts] : wordMap) {
            int wordLen = word.size();
            MPI_Send(&wordLen, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            MPI_Send(word.data(), wordLen, MPI_CHAR, 0, 0, MPI_COMM_WORLD);

            int count = counts[rank];
            MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
}


};

int main(int argc, char* argv[]) {
    string filePath = argv[2];
    string maps = argv[1];
    WordCounters a;

    // Initialize MPI 
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Only rank 0 does serial work
    if (rank == 0) {
        a.fileToMap(maps);
        a.serialCounter(filePath);
    }

    // All ranks prepare for MPI run
    a.fileToMap(maps);  // all ranks must load the same map
    a.mpiCounter(filePath);

    MPI_Finalize();
    return 0;
}