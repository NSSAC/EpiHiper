#!/usr/bin/env bash

# BEGIN: Copyright 
# Copyright (C) 2019 Rector and Visitors of the University of Virginia 
# All rights reserved 
# END: Copyright 

# BEGIN: License 
# Licensed under the Apache License, Version 2.0 (the "License"); 
# you may not use this file except in compliance with the License. 
# You may obtain a copy of the License at 
#   http://www.apache.org/licenses/LICENSE-2.0 
# END: License 

SINGULARITY=${SINGULARITY:-"$(which singularity)"}
DEF=rivanna.def
IMAGE=epihiper.rivanna.sif
ID=$(id -u)

[ -e $IMAGE ] && rm $IMAGE

# Local cache.rivanna
[ -e cache.rivanna ] || mkdir -p cache.rivanna
    
cd cache.rivanna

# EpiHiper
[ -e EpiHiper ] || \
    scp rivanna1.hpc.virginia.edu:/project/biocomplexity/nssac/EpiHiper/build/src/EpiHiper .

# Intel Runtime Libraries
[ -e l_comp_lib_2018.5.274_comp.cpp_redist.tgz ] || \
    wget https://software.intel.com/sites/default/files/managed/b0/e9/l_comp_lib_2018.5.274_comp.cpp_redist.tgz

# Intel MPI
[ -e l_mpi_2018.5.288.tgz ] || \
    wget http://registrationcenter-download.intel.com/akdlm/irc_nas/tec/15614/l_mpi_2018.5.288.tgz
    
# IntelOPA
[ -e IntelOPA-Basic.RHEL76-x86_64.10.9.3.1.1.tgz ] || \
    wget https://downloadmirror.intel.com/28866/eng/IntelOPA-Basic.RHEL76-x86_64.10.9.3.1.1.tgz
    
cd ..

sudo "${SINGULARITY}" build $IMAGE $DEF | tee rivanna.log

[ -e cache.rivanna ] && sudo chown -R $ID cache.rivanna
[ -e $IMAGE ] && sudo chown -R $ID $IMAGE
