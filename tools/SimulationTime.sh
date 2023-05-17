#!/usr/bin/env bash

# BEGIN: Copyright 
# MIT License 
#  
# Copyright (C) 2021 - 2023 Rector and Visitors of the University of Virginia 
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

set -o xtrace

accumulateReplicates () {
  for f in $(find $1 -type d); do
    [ $1 == $f ] || accumulateProcesses $f
  done | gawk -- '
  BEGIN {
    count = 0
    initialization = 0
    initializationSD = 0
    output = 0
    outputSD = 0
    synchronize = 0
    synchronizeSD = 0
    ProcessTransmissions = 0
    ProcessTransmissionsSD = 0
    ProcessIntervention = 0
    ProcessInterventionSD = 0
    processCurrentActions = 0
    processCurrentActionsSD = 0
    total = 0
    totalSD = 0
    rawCommunication = 0
    rawCommunicationSD = 0
  }

  END {
    print total "," sqrt(totalSD/count) "," rawCommunication "," sqrt(rawCommunicationSD/count) "," initialization "," sqrt(initializationSD/count) "," output "," sqrt(outputSD/count) "," synchronize "," sqrt(synchronizeSD/count) "," ProcessTransmissions "," sqrt(ProcessTransmissionsSD/count) "," ProcessIntervention "," sqrt(ProcessInterventionSD/count) "," processCurrentActions "," sqrt(processCurrentActionsSD/count)
  }

  {
    count += 1

    delta = $1 - initialization
    initialization += delta/count
    initializationSD += delta *  ($1 - initialization)
    
    delta = $2 - output
    output += delta/count
    outputSD += delta * ($2 - output)
    
    delta = $3 - synchronize
    synchronize += delta/count
    synchronizeSD += delta * ($3 - synchronize)
    
    delta = $4 - ProcessTransmissions
    ProcessTransmissions += delta/count
    ProcessTransmissionsSD += delta * ($4 - ProcessTransmissions)
    
    delta = $5 - ProcessIntervention
    ProcessIntervention += delta/count
    ProcessInterventionSD += delta * ($5 - ProcessIntervention)
    
    delta = $6 - processCurrentActions
    processCurrentActions += delta/count
    processCurrentActionsSD += delta * ($6 - processCurrentActions)
    
    delta = $7 - total
    total += delta/count
    totalSD += delta * ($7 - total)
  }
  '
}
accumulateProcesses () {
  pushd $1 > /dev/null 2>&1

  for f in *.log; do
    accumulateProcess $f
  done | gawk -- '
  BEGIN {
    initialization = 0
    output = 0
    synchronize = 0
    ProcessTransmissions = 0
    ProcessIntervention = 0
    processCurrentActions = 0
    total = 0
    rawCommunication = 0
  }

  END {
    print initialization " " output " " synchronize " " ProcessTransmissions " " ProcessIntervention " " processCurrentActions " " total " " rawCommunication
  }

  {
    initialization += $1
    output += $2
    synchronize += $3
    ProcessTransmissions += $4
    ProcessIntervention += $5
    processCurrentActions += $6
    total += $7
    rawCommunication += $8
  }
  '

  popd  > /dev/null 2>&1
}

accumulateProcess () {
  grep 'CSimulation::\|CCommunicate::' $1 | gawk -- '
  BEGIN {
    initialization = 0
    output = 0
    synchronize = 0
    ProcessTransmissions = 0
    ProcessIntervention = 0
    processCurrentActions = 0
    rawCommunication = 0
  }

  END {
    print initialization/1000 " " output/1000 " " synchronize/1000 " "  ProcessTransmissions/1000 " " ProcessIntervention/1000 " " processCurrentActions/1000 " " (initialization + output + synchronize + ProcessTransmissions + ProcessIntervention + processCurrentActions)/1000 " " rawCommunication/1000
  }

  $6 ~ "CSimulation::initialization:" {initialization += substr($9, 2, length($9) - 2)}
  $6 ~ "CSimulation::output:" {output += substr($9, 2, length($9) - 2)}
  $6 ~ "CSimulation::synchronize:" {synchronize += substr($9, 2, length($9) - 2)}
  $6 ~ "CSimulation::applyUpdateSequence:" {ProcessIntervention += substr($9, 2, length($9) - 2)}
  $6 ~ "CSimulation::ProcessTransmissions:" {ProcessTransmissions += substr($9, 2, length($9) - 2)}
  $6 ~ "CSimulation::ProcessIntervention:" {ProcessIntervention += substr($9, 2, length($9) - 2)}
  $6 ~ "CSimulation::processCurrentActions:" {processCurrentActions += substr($9, 2, length($9) - 2)}
  $6 ~ "CCommunicate::" && $7 ~ "duration" {rawCommunication += substr($9, 2, length($9) - 2)} 
  '
}

echo "experiment,total,totalSD,rawCommunication,rawCommunicationSD,initialization,initializationSD,output,outputSD,synchronize,synchronizeSD,ProcessTransmissions,ProcessTransmissionsSD,ProcessIntervention,ProcessInterventionSD,processCurrentActions,processCurrentActionsSD" > Performance.csv

for d in $@; do
  echo -n "$(dirname $d)," >> Performance.csv 
  accumulateReplicates $d >> Performance.csv
done

