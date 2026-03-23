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

class ParallelCounter{
    public:
    void parallelCount(int rank, int size, const string& fileName){ //we ask for rank and size, also the path for the book files (they're all in a txt file).
        if(rank==0){ //the rank 0 will function as server
            //it creates the Map
            map<string, vector<int>> wordMap;

            //list of book paths
            
            //reads the files

            //sends each process the book they will analyze

            //receives the responses and adds them to its map.

        }
        else{
            //if youre not the server, you receive a book, and start analizing the word-counting.

            //each word you analize, it sends the information to the server

            //size of word

            //word

            //index of processes / book
        }
    }
};
