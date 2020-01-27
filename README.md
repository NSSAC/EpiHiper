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

### Run the minimal example
```
# make sure this is executed on a compute node (within a slurm job)
mkdir output
cd output
srun <build>/src/EpiHiper --seed 1234567 --dbconn \
  --config /project/biocomplexity/nssac/EpiHiper/test/001/interventionExampleRunParameters.json  dummydb
# output file is created as output/EpiHiper_output.txt
```

This blackbox version reads a EpiHiper-CSV format network file, for each of the specified ticks, generates random infections, and writes them to EpiHiper standard output file. So it needs at least (i) a configuration file (e.g. input/runParametersExample.json) which specifies the paths to the network file and the output file; (ii) an integer as seed for its random number generation; (iii) the network file (in EpiHiper-CSV format).

It does not parse the disease model file. Its output looks like an SI model. The output is syntactically correct and can be used as input for developing analysis tools. But it won't make sense epidemiologically. It also ignores intervention file and personTraitDB connection information.
