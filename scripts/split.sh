#!/usr/bin/env bash

# BEGIN: Copyright 
# Copyright (C) 2020 Rector and Visitors of the University of Virginia 
# All rights reserved 
# END: Copyright 

# BEGIN: License 
# Licensed under the Apache License, Version 2.0 (the "License"); 
# you may not use this file except in compliance with the License. 
# You may obtain a copy of the License at 
#   http://www.apache.org/licenses/LICENSE-2.0 
# END: License 

FILE=$1
let "PARTS = $2"

FILE_BASE=$(basename ${FILE} .txt)

head -n 1 $1 > ${FILE_BASE}.json

let "LINES = $(head -n1 ${FILE_BASE}.json | jq -r .numberOfEdges)"
echo ${LINES}

let "LINES_PER_PART = ${LINES} / ${PARTS} + 1"
echo ${LINES_PER_PART}

sed '1,2d' ${FILE} | split -d -l ${LINES_PER_PART} - ${FILE_BASE}.data.1
