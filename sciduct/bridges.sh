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
DEF=bridges.def
IMAGE=epihiper.bridges.sif
ID=$(id -u)

[ -e $IMAGE ] && rm $IMAGE

# Local cache.bridges
[ -e cache.bridges ] || mkdir -p cache.bridges
    
cd cache.bridges

# EpiHiper
[ -e EpiHiper ] || \
    cp ~/workspace/EpiHiper-code/build/src/EpiHiper* .
    
# Intel MPI
[ -e l_mpi_2019.5.281.tgz ] || \
    wget http://registrationcenter-download.intel.com/akdlm/irc_nas/tec/15838/l_mpi_2019.5.281.tgz

cd ..

sudo "${SINGULARITY}" build $IMAGE $DEF | tee bridges.log

[ -e cache.bridges ] && sudo chown -R $ID cache.bridges
[ -e $IMAGE ] && sudo chown -R $ID $IMAGE
