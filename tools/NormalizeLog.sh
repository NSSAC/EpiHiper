#! /usr/bin/env bash

# BEGIN: Copyright 
# MIT License 
#  
# Copyright (C) 2020 - 2022 Rector and Visitors of the University of Virginia 
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