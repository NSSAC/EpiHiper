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
DEF=validation.def
IMAGE=epiHiperValidation.sif
ID=$(id -u)

[ -e $IMAGE ] && rm $IMAGE

sudo "${SINGULARITY}" build $IMAGE $DEF | tee build.log

[ -e $IMAGE ] && sudo chown -R $ID $IMAGE
