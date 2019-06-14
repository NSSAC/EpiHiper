# EpiHiper

EpiHiper version 0.0.1 is a minimal blackbox. It can be built with a C++/MPI compiler, and is executable on a cluster. To install it on Rivanna and run the minimal example (see under `example/`):

```
# obtain code from github
git clone git@github.com:NSSAC/EpiHiper-code.git

# build EpiHiper blackbox executable
cd EpiHiper-code
mkdir build
cd build
module purge
module add intel intelmpi # can use other MPI modules, e.g. openmpi
cmake ../src -DCMAKE_CXX_COMPILER=mpiicpc
make

# run the minimal example
cd ../example
# Network file is not kept in this repository due to its large size;
# it's available on Rivanna.
ln -s /project/biocomplexity/EpiHiper/data/va_montgomery_2009.txt input/
mkdir output

# make sure this is executed on a compute node (within a slurm job)
srun ../build/EpiHiper --config input/runParametersExample.json --seed 1234567 --dbconn dummydb
# output file is created as output/EpiHiper_output.txt
```

This blackbox version reads a EpiHiper-CSV format network file, for each of the specified ticks, generates random infections, and writes them to EpiHiper standard output file. So it needs at least (i) a configuration file (e.g. input/runParametersExample.json) which specifies the paths to the network file and the output file; (ii) an integer as seed for its random number generation; (iii) the network file (in EpiHiper-CSV format).

It does not parse the disease model file. Its output looks like an SI model. The output is syntactically correct and can be used as input for developing analysis tools. But it won't make sense epidemiologically. It also ignores intervention file and personTraitDB connection information.
