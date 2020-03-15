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
DEF=aws.def
IMAGE=epihiper.aws.sif
ID=$(id -u)

[ -e $IMAGE ] && rm $IMAGE

# Local cache.aws
[ -e cache.aws ] || mkdir -p cache.aws
    
cd cache.aws

# EpiHiper
[ -e EpiHiper ] || \
    cp ~/workspace/EpiHiper-code/build/src/EpiHiper* .
    
# Intel MPI
[ -e l_mpi_2019.6.166.tgz ] || \
    wget http://registrationcenter-download.intel.com/akdlm/irc_nas/tec/16120/l_mpi_2019.6.166.tgz
    
cd ..

sudo "${SINGULARITY}" build $IMAGE $DEF | tee build.log

[ -e cache.aws ] && sudo chown -R $ID cache.aws
[ -e $IMAGE ] && sudo chown -R $ID $IMAGE
