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
  for f in $(ls -d $1-replicate?/va/output/replicate_1); do
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
    applyUpdateSequence = 0
    applyUpdateSequenceSD = 0
    ProcessTransmissions = 0
    ProcessTransmissionsSD = 0
    ProcessIntervention = 0
    ProcessInterventionSD = 0
    processCurrentActions = 0
    processCurrentActionsSD = 0
  }

  END {
    print initialization " " sqrt(initializationSD/count) " " output " " sqrt(outputSD/count) " " synchronize " " sqrt(synchronizeSD/count) " "  applyUpdateSequence " " sqrt(applyUpdateSequenceSD/count) " " ProcessTransmissions " " sqrt(ProcessTransmissionsSD/count) " " ProcessIntervention " " sqrt(ProcessInterventionSD/count) " " processCurrentActions " " sqrt(processCurrentActionsSD/count)
  }

  {
    count += 1

    delta = $1 - initialization
    initialization += delta/count
    initializationSD += delta * initialization 
    
    delta = $2 - output
    output += delta/count
    outputSD += delta * output 
    
    delta = $3 - synchronize
    synchronize += delta/count
    synchronizeSD += delta * synchronize 
    
    delta = $4 - applyUpdateSequence
    applyUpdateSequence += delta/count
    applyUpdateSequenceSD += delta * applyUpdateSequence 
    
    delta = $5 - ProcessTransmissions
    ProcessTransmissions += delta/count
    ProcessTransmissionsSD += delta * ProcessTransmissions 
    
    delta = $6 - ProcessIntervention
    ProcessIntervention += delta/count
    ProcessInterventionSD += delta * ProcessIntervention 
    
    delta = $7 - processCurrentActions
    processCurrentActions += delta/count
    processCurrentActionsSD += delta * processCurrentActions 
  }
  '
}
accumulateProcesses () {
  pushd $1 > /dev/null 2>&1

  for f in *.log; do
    accumulateProcess $f
  done | gawk -- '
  BEGIN {
    count = 0
    initialization = 0
    output = 0
    synchronize = 0
    applyUpdateSequence = 0
    ProcessTransmissions = 0
    ProcessIntervention = 0
    processCurrentActions = 0
  }

  END {
    print initialization " " output " " synchronize " " applyUpdateSequence " " ProcessTransmissions " " ProcessIntervention " " processCurrentActions
  }

  {
    count += 1

    delta = $1 - initialization
    initialization += delta/count
    
    delta = $2 - output
    output += delta/count
    
    delta = $3 - synchronize
    synchronize += delta/count
    
    delta = $4 - applyUpdateSequence
    applyUpdateSequence += delta/count
    
    delta = $5 - ProcessTransmissions
    ProcessTransmissions += delta/count
    
    delta = $6 - ProcessIntervention
    ProcessIntervention += delta/count
    
    delta = $7 - processCurrentActions
    processCurrentActions += delta/count
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
    applyUpdateSequence = 0
    ProcessTransmissions = 0
    ProcessIntervention = 0
    processCurrentActions = 0
  }

  END {
    print initialization/1000 " " output/1000 " " synchronize/1000 " "  applyUpdateSequence/1000 " "  ProcessTransmissions/1000 " " ProcessIntervention/1000 " " processCurrentActions/1000
  }

  $5 ~ "CSimulation::initialization:" {initialization += substr($8, 2, length($8) - 2)}
  $5 ~ "CSimulation::output:" {output += substr($8, 2, length($8) - 2)}
  $5 ~ "CSimulation::synchronize:" {synchronize += substr($8, 2, length($8) - 2)}
  $5 ~ "CSimulation::applyUpdateSequence:" {applyUpdateSequence += substr($8, 2, length($8) - 2)}
  $5 ~ "CSimulation::ProcessTransmissions:" {ProcessTransmissions += substr($8, 2, length($8) - 2)}
  $5 ~ "CSimulation::ProcessIntervention:" {ProcessIntervention += substr($8, 2, length($8) - 2)}
  $5 ~ "CSimulation::processCurrentActions:" {processCurrentActions += substr($8, 2, length($8) - 2)}
  '
}

accumulateReplicates $1

# accumulateProcesses $1
