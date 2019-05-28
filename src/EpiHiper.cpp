#include <iostream>
#include <cstdlib>
#include <mpi.h>

#include "SimConfig.h"
#include "Simulation.h"

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);
  int myRank;
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  if (myRank == 0) {
    std::cout << "EpiHiper version 1.0.0 (2019.05.23)" << std::endl;
  }

  if (argc != 2) {
    if (myRank == 0) {
      std::cout << "\nUsage:\n\t"
		<< "EpiHiper configfile"
		<< std::endl << std::endl;
    }
    MPI_Abort(MPI_COMM_WORLD,1); MPI_Finalize();
    exit(1);
  }

  SimConfig *cfg = new SimConfig(argv[1]);
  cfg->validate();
  if (cfg->isValid()) {
    Simulation *sim = new Simulation(*cfg);
    sim->validate();
    if (sim->isValid()) {
      sim->run();
    }
    delete sim;
  }
  delete cfg;

  MPI_Finalize(); return 0;
}

// Q: Make DB URL a CLA or part of simulation configuration file?
// It may change each time EpiHiper runs on the same configuration file.
