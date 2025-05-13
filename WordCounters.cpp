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
    void fileToMap(const string& filename) {
        wordMap.clear();
        ifstream infile(filename);
        if (!infile.is_open()) {
            cerr << "Could not open map file: " << filename << endl;
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

    void serialCounter(const string& filename) {
        ifstream books(filename);
        if (!books.is_open()) {
            cerr << "Could not open paths file: " << filename << endl;
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

    void mpiCounter(const string& filename, int rank, int size) {
        int numBooks = 0;
        vector<string> bookList;
        string line;

        if (rank == 0) {
            ifstream books(filename);
            getline(books, line);
            numBooks = stoi(line);
            bookList.resize(numBooks);
            for (int i = 0; i < numBooks; ++i)
                getline(books, bookList[i]);
        }

        MPI_Bcast(&numBooks, 1, MPI_INT, 0, MPI_COMM_WORLD);
        bookList.resize(numBooks);
        for (int i = 0; i < numBooks; ++i) {
            int len = bookList[i].size();
            MPI_Bcast(&len, 1, MPI_INT, 0, MPI_COMM_WORLD);
            bookList[i].resize(len);
            MPI_Bcast(bookList[i].data(), len, MPI_CHAR, 0, MPI_COMM_WORLD);
        }

        if (rank >= numBooks) return;  // <- Evita procesos de más

        map<string, vector<int>> localMap;
        ifstream bookfile(bookList[rank]);
        if (!bookfile.is_open()) {
            cerr << "Rank " << rank << ": Could not open book file: " << bookList[rank] << endl;
            MPI_Abort(MPI_COMM_WORLD, 2);
        }

        while (getline(bookfile, line)) {
            istringstream stream(line);
            string word;
            while (getline(stream, word, ',')) {
                if (localMap.find(word) == localMap.end()) {
                    localMap[word] = vector<int>(numBooks, 0);
                }
                localMap[word][rank]++;
            }
        }

        if (rank == 0) {
            for (int p = 1; p < size; ++p) {
                if (p >= numBooks) continue;

                int wordCount;
                MPI_Recv(&wordCount, 1, MPI_INT, p, TAG_WORDCOUNT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                for (int w = 0; w < wordCount; ++w) {
                    int wordLen;
                    MPI_Recv(&wordLen, 1, MPI_INT, p, TAG_WORDLEN, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    string word(wordLen, ' ');
                    MPI_Recv(word.data(), wordLen, MPI_CHAR, p, TAG_WORD, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    int count;
                    MPI_Recv(&count, 1, MPI_INT, p, TAG_VALUE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    if (wordMap.find(word) == wordMap.end()) {
                        wordMap[word] = vector<int>(numBooks, 0);
                    }
                    wordMap[word][p] = count;
                }
            }

            for (auto& [w, counts] : localMap) {
                if (wordMap.find(w) == wordMap.end()) {
                    wordMap[w] = vector<int>(numBooks, 0);
                }
                wordMap[w][0] = counts[0];
            }

            ofstream out("Outputs/file_map_parallel.txt");
            out << "word";
            for (int i = 0; i < numBooks; ++i) out << "\tbook" << i;
            out << "\n";
            for (auto& [w, counts] : wordMap) {
                out << w;
                for (int c : counts) out << "\t" << c;
                out << "\n";
            }
        } else {
            int wordCount = localMap.size();
            MPI_Send(&wordCount, 1, MPI_INT, 0, TAG_WORDCOUNT, MPI_COMM_WORLD);
            for (auto& [w, counts] : localMap) {
                int len = w.size();
                MPI_Send(&len, 1, MPI_INT, 0, TAG_WORDLEN, MPI_COMM_WORLD);
                MPI_Send(w.data(), len, MPI_CHAR, 0, TAG_WORD, MPI_COMM_WORLD);
                int val = counts[rank];
                MPI_Send(&val, 1, MPI_INT, 0, TAG_VALUE, MPI_COMM_WORLD);
            }
        }
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
    wc.mpiCounter(pathsFile, rank, size);
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