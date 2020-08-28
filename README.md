# EpiHiper

EpiHiper version 0.0.1 is a minimal blackbox. It can be built with a C++/MPI compiler, and is executable on a cluster. To install it on Rivanna and run the minimal example (see under `example/`):

### Obtain Code
```
git clone git@github.com:NSSAC/EpiHiper-code.git
cd EpiHiper-code
```

### Build EpiHiper
__Rivanna__: The code is already deployed on rivanna in: `/project/biocomplexity/nssac/EpiHiper/git/EpiHiper-code`. For the following instructions we use that as the location of the git repository.

Create a build directory of you choice.

__Build using Open MPI 3.14__
```
mkdir build
cd build
module purge
module load gcc/7.1.0 openmpi/3.1.4 cmake/3.12.3 
export CC=gcc
export CXX=g++
cmake /project/biocomplexity/nssac/EpiHiper/git/EpiHiper-code
make
```

__Build using Intel MPI 18.0__
```
mkdir build
cd build
module purge
module load intel/18.0 intelmpi/18.0 cmake/3.12.3 
export CC=icc
export CXX=icc
cmake /project/biocomplexity/nssac/EpiHiper/git/EpiHiper-code
make
```

__Build options__
* `-DCMAKE_BUILD_TYPE=[Debug|Release]` Enable debug build. (default: Release)
* `-DENABLE_LOCATION_ID=[ON|OFF]` Enable location Id support for networks. (default: OFF)
* `-DENABLE_MPI==[ON|OFF]` Enable MPI parallelization. (default: ON)
* `-DENABLE_OMP==[ON|OFF]` Enable OpenMP parallelization. (default: OFF)

Note: It is possible to enable both MPI and OpenMP simultaneously.

### Run 
```
mpirun -np #MPI --bind-to none -x OMP_NUM_THREADS=#OMP <build>/src/EpiHiper \ 
  --config /project/biocomplexity/nssac/EpiHiper/test/001/interventionExampleRunParameters.json
```
Here `#MPI` is the number of MPI tasks and `#OMP` is the number of threads per task.
