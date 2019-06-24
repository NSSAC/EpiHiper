// BEGIN: Copyright 
// Copyright (C) 2019 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#include <iostream>
#include <cstdlib>
#include <mpi.h>
#include <getopt.h>

#include "SimConfig.h"
#include "Simulation.h"
#include "traits/Trait.h"
#include "diseaseModel/Model.h"

std::string config = std::string();
int seed = -1;
std::string dbconn = std::string();

void printUsage() {
  std::cout << "\nUsage:\n\n"
    "EpiHiper --config <configFilename> --seed <integerValue>"
    " --dbconn <jsonString>\n"
    "--config: required string specifying EpiHiper configuration file\n"
    "--seed: optional integer (>=0) as seed for random number generation\n"
    "--dbconn <jsonString>: JSON with connection info to personTraitDB\n";
  //exit(EXIT_FAILURE);
}

bool parseArgs(int argc, char *argv[]) {
  const char* const short_opts = "c:s:d:";
  const option long_opts[] = {
    {"config", required_argument, nullptr, 'c'},
    {"seed", required_argument, nullptr, 's'},
    {"dbconn", required_argument, nullptr, 'd'},
    {nullptr, no_argument, nullptr, 0}
  };
  while (true) {
    const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);
    if (-1 == opt)
      break;
    switch(opt) {
    case 'c':
      config = std::string(optarg);
      break;
    case 's':
      seed = std::stoi(optarg);
      break;
    case 'd':
      dbconn = std::string(optarg);
      break;
    case '?':
    default:
      return false;
    }
  }
  return true;
}

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);
  int myRank;
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  if (myRank == 0) {
    std::cout << "EpiHiper version 0.0.1 (2019.06.14)" << std::endl;
  }

  if (argc < 3 || ! parseArgs(argc, argv)) {
    if (myRank == 0) {
      printUsage();
    }
    MPI_Abort(MPI_COMM_WORLD,1); MPI_Finalize();
    exit(EXIT_FAILURE);
  }

  SimConfig::init(config);

  if (SimConfig::isValid()) {
    std::map< std::string, Trait > Traits = Trait::load(SimConfig::getTraits());

    Model DiseaseModel;
    DiseaseModel.fromJSON(SimConfig::loadJson(SimConfig::getDiseaseModel()));

    Simulation sim(seed, dbconn);
    sim.validate();
    if (sim.isValid()) {
      sim.dummyRun();
    }
  }

  SimConfig::release();
  MPI_Finalize(); return 0;
}
