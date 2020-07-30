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

Base=normalized
Start=-10

while getopts "o:s:" opt; do
  case ${opt} in
    o ) # process option o
      Base="$OPTARG"
      ;;
    s )  # process option o
      Start="$OPTARG"
      ;;
    \? ) echo "Usage: cmd [-o] [-a]"
      ;;
  esac
done

shift $((OPTIND-1))

filename=$(basename -- "${1}")
extension="${filename##*.}"
filename="${filename%.*}"

ThreadInfo=$(echo ${filename} | sed -e 's/^[^\[]*//' -e 's/\[/ /g' -e 's/:/ /g' -e 's/]/ /g')
ThreadInfo=($(echo $ThreadInfo))
(( Thread = ${ThreadInfo[1]} * ${ThreadInfo[2]:-1} + ${ThreadInfo[3]:-0} ))

out="${Base}.${Thread}.${extension}"

gawk -- '
BEGIN {
  Tick = 0
}

function printRow()
{
  printf "[%d] [%d] ", '${Thread}', Tick
  for (i=5; i <= NF; i++) printf "%s ", $i
  printf "\n"
}

$0 ~ "Tick:" {
  Tick = $6 + 0
}

{
  if (Tick >= '${Start}')
    printRow()
}' "${1}" > $out
