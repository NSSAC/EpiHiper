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
DEF=rivanna.def
IMAGE=epihiper.rivanna.sif
ID=$(id -u)

[ -e $IMAGE ] && rm $IMAGE

# Local cache.rivanna
[ -e cache.rivanna ] || mkdir -p cache.rivanna
    
cd cache.rivanna

# EpiHiper
if [ ! -e EpiHiper ]; then
    scp rivanna:/project/biocomplexity/nssac/EpiHiper/build/src/EpiHiper* . 
    scp rivanna:/project/biocomplexity/nssac/EpiHiper/build/src/libEpiHiper* . 
fi

# Assure that the files are readable and executable by all
chmod 775 EpiHiper*
chmod 775 libEpiHiper*

# Intel Runtime Libraries
[ -e l_comp_lib_2018.5.274_comp.cpp_redist.tgz ] || \
    scp rivanna:/project/bii_nssac/EpiHiper/intel/src/l_comp_lib_2018.5.274_comp.cpp_redist.tgz .

# Intel MPI
[ -e l_mpi_2018.5.288.tgz ] || \
    scp rivanna:/project/bii_nssac/EpiHiper/intel/src/l_mpi_2018.5.288.tgz .
    
# IntelOPA
[ -e IntelOPA-Basic.RHEL76-x86_64.10.9.3.1.1.tgz ] || \
    scp rivanna:/project/bii_nssac/EpiHiper/intel/src/IntelOPA-Basic.RHEL76-x86_64.10.9.3.1.1.tgz .
    
cd ..

sudo "${SINGULARITY}" build $IMAGE $DEF | tee rivanna.log

[ -e cache.rivanna ] && sudo chown -R $ID cache.rivanna
[ -e $IMAGE ] && sudo chown -R $ID $IMAGE
