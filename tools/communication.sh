#!/usr/bin/env bash

grep 'CCommunicate::\(broadcast\|send\)' EpiHiper.\[40\:* | awk -- '
BEGIN {
    total = 0
    count = 0
}

END {
    print "total: " total
    print "count: " count
}

{
    count += 1
    total += substr($6, 2, length($6) - 2)
}
'