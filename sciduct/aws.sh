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
DEF=aws.def
IMAGE=epihiper.aws.sif
ID=$(id -u)

[ -e $IMAGE ] && rm $IMAGE

# Local cache.aws
[ -e cache.aws ] || mkdir -p cache.aws
    
cd cache.aws

# EpiHiper
cp ~/workspace/EpiHiper-code/build/src/EpiHiper* .

cd ..

sudo "${SINGULARITY}" build $IMAGE $DEF | tee aws.log

[ -e cache.aws ] && sudo chown -R $ID cache.aws
[ -e $IMAGE ] && sudo chown -R $ID $IMAGE