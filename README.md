[![Documentation Status](https://readthedocs.org/projects/epihiper/badge/?version=latest)](https://epihiper.readthedocs.io/en/latest/?badge=latest)

# EpiHiper

EpiHiper version 2.0.0. Is an agent based epidemic simulator

### Obtain Code
```
git clone https://github.com/NSSAC/EpiHiper.git
```

### Dependencies
Requirements for building EpiHiper:
 - C++ 11 compatible compiler, e.g. gcc 7.1.0
 - Cmake version 3.11 or above
 - PostgreSQL library (provided by package: libpq5) 
 - PostgreSQL header  (provided by package: postgresql-devel)

 All other dependencies are provided through git submodules, which are automatically retrieved during configuration and compiled during make. These dependencies are:
 - [jansson](https://github.com/akheron/jansson.git)
 - [libpqxx](https://github.com/jtv/libpqxx.git)
 - [spdlog](https://github.com/gabime/spdlog.git)

### Building a single threaded executable
From the root directory of the repository execute:
```
mkdir build
cd build
cmake -G "Unix Makefiles" -DENABLE_MPI=OFF -DENABLE_OMP=OFF -DENABLE_LOCATION_ID=OFF ..
make -j
```

### Building an MPI (multi-process) executable
Additional dependencies:
 - OpenMPI version 3.14 and above (provided by package: openmpi3)
 - OpenMPI header files (provided by package: openmpi3-devel)

From the root directory of the repository execute:
```
mkdir build
cd build
cmake -G "Unix Makefiles" -DENABLE_MPI=ON -DENABLE_OMP=OFF -DENABLE_LOCATION_ID=OFF ..
make -j
```

### Building an OpenMP (multi-threaded) executable
Additional dependencies:
 - OpenMP run time library (provided by package: libgomp1)

From the root directory of the repository execute:
```
mkdir build
cd build
cmake -G "Unix Makefiles" -DENABLE_MPI=OFF -DENABLE_OMP=ON -DENABLE_LOCATION_ID=OFF ..
make -l
```

__Note__: It is possible to combine OpenMP and MPI

### Configuration options:
 - `-DCMAKE_BUILD_TYPE=[Debug|Release]` Enable debug build. (default: Release)
 - `-DENABLE_LOCATION_ID=[ON|OFF]` Enable location Id support for networks. (default: OFF)
 - `-DENABLE_MPI==[ON|OFF]` Enable MPI parallelization. (default: ON)
 - `-DENABLE_OMP==[ON|OFF]` Enable OpenMP parallelization. (default: OFF)

### Example
This repository contains a small example which can be run in seconds on regular laptop or desktop.
The `config.json` file is the single file which must be provided to EpiHiper in the command line.
To avoid installing a PostgreSQL database the example does not use the person trait database, i.e., 
initialization and intervention do not rely on additional demographic information of the individuals in the network. 
Nevertheless the example directory contains such a [data base](example/personTraitDB.txt) in order to illustrate the format.
The [example](example/initialization.json) initializes 2 nodes with infections and vaccinates 3 other with a 90% effective
vaccine (see susceptibility of state V in the [disease model](example/diseaseModel.json)). The [example](example/config.json) 
uses a fixed seed in order to create a verifiable result. The [expected result](example/expectedResult/) is created with a single threaded executable and thus can only be recreated with a single threaded simulation.

The format of the configuration files is defined through JSON schema which can be found [here](https://github.com/NSSAC/EpiHiper-Schema/tree/master/schema). Alongside the schema there are more complex example in the [test directory](https://github.com/NSSAC/EpiHiper-Schema/tree/master/test)

Running the example creates multiple output files:
 - `stateTransistions.csv` which records all health state changes of the individuals
 - `output.csv` which provides aggregate state numbers.
 - `status.json` which provides the current completion status of EpiHiper (important for long running processes)
 - `EpiHiper.*.log` where each thread creates its individual log file. 
### Runing example as a single threaded executable
From the root directory of the repository execute:
```
build/src/EpiHiper --config example/config.json
```

### Runing as multi-threaded multi-process executable
From the root directory of the repository execute:
```
mpirun -np #MPI --bind-to none -x OMP_NUM_THREADS=#OMP build/src/ --config example/config.json
```
Here `#MPI` is the number of MPI tasks and `#OMP` is the number of threads per task.


