#!/usr/bin/env bash

if [ _$# != _0 ]; then
  echo usage: $0
  exit 1
fi

jobId=${SLURM_JOB_ID:-222222}
jobName=${SLURM_JOB_NAME:-testValidateRules}
statusFile="/job/sciduct.output.json"

[ -e ${statusFile} ] || \
    /epihiper/bin/epiHiperStatus -i "${jobId}" -n "${jobName}" -s running -p 0 ${statusFile}

/epihiper/bin/epiHiperStatus -d "Validating Rules" ${statusFile}

/epihiper/bin/epiHiperValidateRules "/input/runParameters.json"

if [ $? != 0 ]; then
  /epihiper/bin/epiHiperStatus -s failed ${statusFile}
  exit 1
fi

/epihiper/bin/epiHiperStatus -s completed -d "Finished" -p 100 ${statusFile}
