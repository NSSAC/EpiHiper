#!/usr/bin/env python3

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

import argparse
import pandas as pd
import random as random
import datetime as datetime
from operator import itemgetter
import copy as copy

parser = argparse.ArgumentParser(description="Statistcs means/std variation for all columns except the first in a csv file.")
parser.add_argument('inFile', help='Input file')
parser.add_argument('outFile', help='Output file')
args = parser.parse_args()

df = pd.read_csv(args.inFile, index_col=None, header=0)

Value = [
    '[current]',
    '[in]',
    '[out]'
]

AgeGroups = [
    'p',
    's',
    'a',
    'o',
    'g'
    ]

HealthStates = []

for h in df.columns:
    if h.find('[out]') != -1:
        HealthStates.append(h.split('_')[0])

analysis = []

for i, row in df.iterrows():
    Row = {'Tick': i}

    for s in HealthStates:
        for v in Value:
            Total = 0

            for a in AgeGroups:
                Total += row[s + '_' + a + v]

            Row[s + v] = Total

    analysis.append(Row)

out = pd.DataFrame(data=analysis, index=None)
out.to_csv(args.outFile, index = False)

