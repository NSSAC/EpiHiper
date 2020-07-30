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

### Running EpiHiper
The following assumes that you have a properly configured runParameters.json file. The schema for the file can be found [here](https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/runParametersSchema.json).

__Compiled with: MPI = off, OpenMP = off__
```
EpiHiper --config runParameters.json
```
__Compiled with: MPI = on, OpenMP = off__ 
```
mpirun -np <MPI processes> EpiHiper --config runParameters.json
```
__Compiled with: MPI = off, OpenMP = on__ 
```
OMP_NUM_THREADS=<OpenMP threads> EpiHiper --config runParameters.json
```
__Compiled with: MPI = on, OpenMP = on__ 
```
mpirun -np <MPI processes> --bind-to none -x OMP_NUM_THREADS=<OpenMP threads> EpiHiper --config runParameters.json
```
The will execute EpiHiper in parallel with `<MPI processes> * <OpenMP threads>` threads.
