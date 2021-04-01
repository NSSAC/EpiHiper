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

shopt -s globstar

OPTIND=1

show_help() {
    echo "Usage: $(basename $0)"
}

MASTER=false

while getopts "h" opt; do
    case "$opt" in
        h)
            show_help
            exit 0
            ;;
    esac
done

shift $((OPTIND-1))

analizeSingle () {
    start=$(head -n 1 $1 | gawk -- '{print substr($2, 1, length($2) - 1)}')
    end=$(tail -n 1 $1 | gawk -- '{print substr($2, 1, length($2) - 1)}')
    communicate=$(grep '\(Tick:\|CCommunicate\)' $1 | gawk -- '
BEGIN {
    total = 0
    count = 0
    communication = 0
    writing = 0
    synchronization = 0
    memory = 0
}

END {
    print total/1000 ", " count ", " communication ", "  writing ", "  synchronization ", " memory
}

$5 ~ "Tick:" {
    tmp = substr($8, 1, length($8) - 1)
    memory = (memory < tmp ? tmp : memory)
}

$5 ~ "CCommunicate::(send|broadcast'$2')" {
    count += 1
    total += substr($6, 2, length($6) - 2)
}

$5 ~ "CCommunicate::(master|roundRobin)" {
    communication += substr($8, 2, length($8) - 2)
}

$5 ~ "CCommunicate::sequential" {
    writing += substr($8, 2, length($8) - 2)
}

$5 ~ "CCommunicate::barrierRMA" {
    synchronization += substr($8, 2, length($8) - 2)
}
')

echo $1, $start, $end, $communicate
}

MASTER=""

echo "file, start, end, message total [kB], message count, message time [us], writing [us], synchronization [us], memory [kB]"

for f in EpiHiper.*.log; do
    analizeSingle $f $MASTER
    MASTER=false
done
