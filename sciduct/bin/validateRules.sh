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

if [ _$# != _0 ]; then
  echo usage: $0
  exit 1
fi

jobId=${SLURM_JOB_ID:-222222}
jobName=${SLURM_JOB_NAME:-testValidateRules}
statusFile="/job/sciduct.status.json"

[ -e ${statusFile} ] || \
    /epihiper/bin/epiHiperStatus -i "${jobId}" -n "${jobName}" -s running -p 0 ${statusFile}

/epihiper/bin/epiHiperStatus -d "Validating Rules" ${statusFile}

/epihiper/bin/epiHiperValidateRules "/input/runParameters"

let retval=$?

if [ ${retval} != 0 ]; then
  /epihiper/bin/epiHiperStatus -s failed ${statusFile}
  exit ${retval}
fi

/epihiper/bin/epiHiperStatus -s completed -d "Finished" -p 100 ${statusFile}
