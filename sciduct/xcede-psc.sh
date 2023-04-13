#!/usr/bin/env bash

# BEGIN: Copyright 
# MIT License 
#  
# Copyright (C) 2020 - 2023 Rector and Visitors of the University of Virginia 
#  
# Permission is hereby granted, free of charge, to any person obtaining a copy 
# of this software and associated documentation files (the "Software"), to deal 
# in the Software without restriction, including without limitation the rights 
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
# copies of the Software, and to permit persons to whom the Software is 
# furnished to do so, subject to the following conditions: 
#  
# The above copyright notice and this permission notice shall be included in all 
# copies or substantial portions of the Software. 
#  
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
# SOFTWARE 
# END: Copyright 

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
