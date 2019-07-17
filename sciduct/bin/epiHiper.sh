#!/usr/bin/env bash

if [ _$# != _0 ]; then
  echo usage: $0
  exit 1
fi

source /opt/intel/impi/2018.5.288/bin64/mpivars.sh

jobId=${SLURM_JOB_ID:-222222}
jobName=${SLURM_JOB_NAME:-testEpiHiper}
statusFile="/job/sciduct.output.json"

if [ _$PMI_RANK == _0 ]; then
  [ -e ${statusFile} ] || \
    /epihiper/bin/epiHiperStatus -i "${jobId}" -n "${jobName}" -s running -p 0 ${statusFile}  

  /epihiper/bin/epiHiperStatus -d "Running EpiHiper Simulation" ${statusFile}

  /epihiper/bin/EpiHiper --config "/input/runParameters.json"

  if [ $? != 0 ]; then
    /epihiper/bin/epiHiperStatus -s failed ${statusFile}
    exit 1
  fi

  /epihiper/bin/epiHiperStatus -s completed -d "Finished" -p 100 ${statusFile}
else
  /epihiper/bin/EpiHiper --config "/input/runParameters.json"
fi
