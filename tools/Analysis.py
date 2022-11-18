#!/usr/bin/env python3

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

import argparse
import pandas as pd
import random as random
import datetime as datetime
from operator import itemgetter
import copy as copy
from collections import OrderedDict

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
    Row = OrderedDict()
    Row['tick'] = i
    
    for s in HealthStates:
        for v in Value:
            Total = 0

            for a in AgeGroups:
                Total += row[s + '_' + a + v]

            Row[s + v] = Total

    analysis.append(Row)

out = pd.DataFrame(data=analysis, index=None)
out.to_csv(args.outFile, index = False)

