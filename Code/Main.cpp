#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <chrono>
#include <mpi.h>
#include <iomanip>

//Including the counters
#include "SerialCounter.cpp"
#include "ParallelCounter.cpp"

using namespace std;
using namespace std::chrono;

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
    SerialCounter wc;

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