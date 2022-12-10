# EpiHiper

EpiHiper version 2.0.0. Is an agent based epidemic simulator

### Obtain Code
```
git clone https://github.com/NSSAC/EpiHiper.git
cd EpiHiper
```

### Dependencies
Requirements for building EpiHiper:
 - C++ 11 compatible compiler, e.g. gcc 7.5.0
 - Cmake version 3.11 or above
 - PostgreSQL library (provided by package: libpq5) 
 - PostgreSQL header  (provided by package: postgresql-devel)

### Building a single threaded executable
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
```
mkdir build
cd build
cmake -G "Unix Makefiles" -DENABLE_MPI=ON -DENABLE_OMP=OFF -DENABLE_LOCATION_ID=OFF ..
make -j
```

### Building an OpenMP (multi-threaded) executable
Additional dependencies:
 - OpenMP run time library (provided by package: libgomp1)
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


### Runing a multi-threaded multi-process executable
```
mpirun -np #MPI --bind-to none -x OMP_NUM_THREADS=#OMP <build>/src/EpiHiper \ 
  --config /project/biocomplexity/nssac/EpiHiper/test/001/interventionExampleRunParameters.json
```
Here `#MPI` is the number of MPI tasks and `#OMP` is the number of threads per task.
