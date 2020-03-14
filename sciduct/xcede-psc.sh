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
DEF=xcede-psc.def
IMAGE=epihiper-intel2019.5.281.sif
ID=$(whoami)

[ -e $IMAGE ] && rm $IMAGE

# Local cache
[ -e cache ] || mkdir -p cache
    
cd cache

# EpiHiper
[ -e EpiHiper ] || \
    cp ~/workspace/EpiHiper-code/build/src/EpiHiper* .
    
# Intel MPI
[ -e l_mpi_2019.5.281.tgz ] || \
    wget http://registrationcenter-download.intel.com/akdlm/irc_nas/tec/15838/l_mpi_2019.5.281.tgz
    
cd ..

sudo "${SINGULARITY}" build $IMAGE $DEF | tee build.log

[ -e cache ] && sudo chown -R $ID cache
[ -e $IMAGE ] && sudo chown -R $ID $IMAGE
