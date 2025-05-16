#include <iostream>
#include <vector>
#include <mpi.h>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace std::chrono;

// MPI message tags
enum {
    TAG_NUMBOOKS = 1,
    TAG_BOOKLEN,
    TAG_WORDCOUNT,
    TAG_WORDLEN,
    TAG_WORD,
    TAG_VALUE
};

class WordCounters {
    map<string, vector<int>> wordMap;

public:
    void fileToMap(const string& fileName) {
        wordMap.clear();
        ifstream infile(fileName);
        if (!infile.is_open()) {
            cerr << "Could not open map file: " << fileName << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
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

    void parallelCounter(const string& fileName){
        
    }
};

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 3) {
        if (rank == 0) cerr << "Usage: program <map_file> <paths_file>" << endl;
        MPI_Finalize();
        return 1;
    }

    string mapFile  = argv[1];
    string pathsFile = argv[2];
    WordCounters wc;

    double serialTime = 0.0;
    if (rank == 0) {
        wc.fileToMap(mapFile);
        auto t0 = high_resolution_clock::now();
        wc.serialCounter(pathsFile);
        auto t1 = high_resolution_clock::now();
        serialTime = duration<double>(t1 - t0).count();
    }

    MPI_Barrier(MPI_COMM_WORLD);

    wc.fileToMap(mapFile);
    double startMPI = MPI_Wtime();
    //wc.mpiCounter(pathsFile, rank, size);
    double endMPI   = MPI_Wtime();
    double parallelTime = endMPI - startMPI;

    if (rank == 0) {
        ofstream out("Outputs/execution_times.txt");
        out << fixed << setprecision(5);
        out << "serial\tparallel\n";
        out << serialTime << "\t" << parallelTime << endl;
    }

    MPI_Finalize();
    return 0;
}