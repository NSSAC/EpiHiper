#! /usr/bin/env bash

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

Filter='CTransmissionAction:|CProgressionAction:|CActionDefinition:|CNode.\\[|CSizeOf:|CRandom:|CModel::stateChanged'
echo Filter = "('${Filter}')"

gawk -- '
BEGIN {
  Tick = 0
}

function printRow()
{
  printf "[%d] ", Tick
  for (i=5; i <= NF; i++) printf "%s ", $i
  printf "\n"
}

$0 ~ "Tick:" {
  Tick = $6 
}

$0 ~ "('${Filter}')" {
  printRow()
}' $1
