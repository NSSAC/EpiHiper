#!/usr/bin/env bash

# BEGIN: Copyright 
# Copyright (C) 2021 Rector and Visitors of the University of Virginia 
# All rights reserved 
# END: Copyright 

# BEGIN: License 
# Licensed under the Apache License, Version 2.0 (the "License"); 
# you may not use this file except in compliance with the License. 
# You may obtain a copy of the License at 
#   http://www.apache.org/licenses/LICENSE-2.0 
# END: License 

accumulateReplicates () {
  for f in $(ls -d $1/$2/replicate_?); do
    accumulateProcesses $f
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
  }

  END {
    print initialization " " sqrt(initializationSD/count) " " output " " sqrt(outputSD/count) " " synchronize " " sqrt(synchronizeSD/count) " " ProcessTransmissions " " sqrt(ProcessTransmissionsSD/count) " " ProcessIntervention " " sqrt(ProcessInterventionSD/count) " " processCurrentActions " " sqrt(processCurrentActionsSD/count)
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
  }

  END {
    print initialization " " output " " synchronize " " ProcessTransmissions " " ProcessIntervention " " processCurrentActions
  }

  {
    initialization += $1
    output += $2
    synchronize += $3
    ProcessTransmissions += $4
    ProcessIntervention += $5
    processCurrentActions += $6
  }
  '

  popd  > /dev/null 2>&1
}

accumulateProcess () {
  grep 'CSimulation::' $1 | gawk -- '
  BEGIN {
    initialization = 0
    output = 0
    synchronize = 0
    ProcessTransmissions = 0
    ProcessIntervention = 0
    processCurrentActions = 0
  }

  END {
    print initialization/1000 " " output/1000 " " synchronize/1000 " "  ProcessTransmissions/1000 " " ProcessIntervention/1000 " " processCurrentActions/1000
  }

  $5 ~ "CSimulation::initialization:" {initialization += substr($8, 2, length($8) - 2)}
  $5 ~ "CSimulation::output:" {output += substr($8, 2, length($8) - 2)}
  $5 ~ "CSimulation::synchronize:" {synchronize += substr($8, 2, length($8) - 2)}
  $5 ~ "CSimulation::applyUpdateSequence:" {ProcessIntervention += substr($8, 2, length($8) - 2)}
  $5 ~ "CSimulation::ProcessTransmissions:" {ProcessTransmissions += substr($8, 2, length($8) - 2)}
  $5 ~ "CSimulation::ProcessIntervention:" {ProcessIntervention += substr($8, 2, length($8) - 2)}
  $5 ~ "CSimulation::processCurrentActions:" {processCurrentActions += substr($8, 2, length($8) - 2)}
  '
}

[ -e Performance-$1.csv ] && rm Performance-$1.csv

for f in A B C D; do
    echo -n $f" " >> Performance-$1.csv
    accumulateReplicates Scenario-$f $1 >> Performance-$1.csv
done

