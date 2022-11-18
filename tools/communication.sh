#!/usr/bin/env bash

# BEGIN: Copyright 
# MIT License 
#  
# Copyright (C) 2021 - 2022 Rector and Visitors of the University of Virginia 
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
