#!/usr/bin/env python3

# BEGIN: Copyright 
# Copyright (C) 2020 - 2022 Rector and Visitors of the University of Virginia 
# All rights reserved 
# END: Copyright 

# BEGIN: License 
# Licensed under the Apache License, Version 2.0 (the "License"); 
# you may not use this file except in compliance with the License. 
# You may obtain a copy of the License at 
#   http://www.apache.org/licenses/LICENSE-2.0 
# END: License 

import argparse
import math
import pandas as pd
import copy as copy
from collections import OrderedDict

parser = argparse.ArgumentParser(description="Statistics means/std variation for all columns except the first in a csv file.")
parser.add_argument('files', nargs='+', help='Files to calculate statistics')
args = parser.parse_args()

statistics = dict()

template = { 'mean': 0.0, 'sd': 0.0, 'count': 0}

for file in args.files:
    print("Processing file: {}".format(file))
    df = pd.read_csv(file, index_col=None, header=0)

    # each row
    for i, row in df.iterrows():
        if not row['tick'] in statistics:
            statistics[row['tick']] = OrderedDict()
        
        rowStats = statistics[row['tick']]

        # column except 'tick'
        for col in df.columns:
            if col == 'tick': continue

            if not col in rowStats:
                rowStats[col] = copy.copy(template)
            
            colStats = rowStats[col]
            colStats['count'] += 1
            v = float(row[col])
            delta = v - colStats['mean']  
            colStats['mean'] += delta/colStats['count']

            # This uses the new mean, i.e., not DELTA * DELTA
            colStats['sd'] += delta * (v - colStats['mean'])

data = []

for t in sorted(statistics):
    row = OrderedDict()
    row['tick'] = t
    Stats = statistics[t]

    for col in Stats:
        row[col + '[mean]'] = Stats[col]['mean']
        row[col + '[sd]'] = str(math.sqrt(Stats[col]['sd']/Stats[col]['count']))
        
    data.append(row)

 

out = pd.DataFrame(data=data, index=None)
out.to_csv('statistics.csv', index = False)

