#!/usr/bin/env bash

if [ _$# != _0 ]; then
  echo usage: validateSchema.sh
  exit 1
fi

jobId=${SLURM_JOB_ID:-222222}
jobName=${SLURM_JOB_NAME:-testValidateSchema}
statusFile="/job/sciduct.output.json"

epiHiperStatus -i "${jobId}" -n "${jobName}" -s running -p 0 ${statusFile}  

epiHiperStatus -d "Validating Schema" ${statusFile}
epiHiperValidateSchema "/input/runParameters.json"

if [ $? != 0 ]; then
  epiHiperStatus -s failed ${statusFile}
  exit 1
fi

epiHiperStatus -s completed -d "Finished" -p 100 ${statusFile}
