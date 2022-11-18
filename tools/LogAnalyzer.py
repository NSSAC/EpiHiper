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
import math
from collections import deque
import copy as copy
from pathlib import Path
import json


parser = argparse.ArgumentParser(description="Summarize initialization, triggers, and interventions.")
parser.add_argument('files', nargs='+', help='Log files to summarize')
args = parser.parse_args()

summary = dict()
ensembleStack = deque()

template = { 'mean': 0.0, 'sd': 0.0, 'count': 0}

def updateStatistics(stats, value):
    stats['count'] += 1
    v = float(value)
    delta = v - stats['mean']  
    stats['mean'] += delta/stats['count']

    # This uses the new mean, i.e., not DELTA * DELTA
    stats['sd'] += delta * (v - stats['mean'])


for file in args.files:
    print("Processing file: {}".format(file))
    fp = open(file, 'r')
    line = '-'

    while line:
        line = fp.readline()

        if line.find('[info]') < 0:
            continue

        # print("Line: {}".format(line.strip()))
        part = line.split()
        keyword = part[4]

        if keyword == 'Tick:':
            if not part[5] in summary:
                summary[part[5]] = dict()

            tickData = summary[part[5]]
            continue

        if keyword == 'CInitialization:':
            id = part[7].strip('"\'.')

            if not id in tickData:
                tickData[id] = dict()

            try:
                while True:
                    ensembleData = ensembleStack.pop()
                    if 'incremented' in ensembleData:
                        del ensembleData['incremented']
                    if 'currentSampled' in ensembleData:
                        del ensembleData['currentSampled']
                    if 'currentNotSampled' in ensembleData:
                        del ensembleData['currentNotSampled']
            except:
                pass
                                
            ensembleStack.clear()
            ensembleStack.append(tickData[id])
            continue

        if keyword == 'CIntervention:':
            id = part[6].strip('"\'.')

            if not id in tickData:
                tickData[id] = dict()

            try:
                while True:
                    ensembleData = ensembleStack.pop()
                    if 'incremented' in ensembleData:
                        del ensembleData['incremented']
                    if 'currentSampled' in ensembleData:
                        del ensembleData['currentSampled']
                    if 'currentNotSampled' in ensembleData:
                        del ensembleData['currentNotSampled']
            except:
                pass

            ensembleStack.clear()
            ensembleStack.append(tickData[id])
            continue

        try:
            ensembleData = ensembleStack[-1]

        except:
            continue

        if keyword == 'CActionEnsemble:':
            if part[5] == 'Target':
                targetSetSize = int(part[8].strip('"\'.,'))

                while True:
                    if 'currentSampled' in ensembleData:
                        if ensembleData['currentSampled'] == targetSetSize:
                            if not 'sampled' in ensembleData:
                                ensembleData['sampled'] = dict()

                            ensembleData['currentSampled'] = -1
                            ensembleStack.append(ensembleData['sampled'])
                            ensembleData = ensembleStack[-1]
                            break
                        elif ensembleData['currentNotSampled'] == targetSetSize:
                            if not 'notSampled' in ensembleData:
                                ensembleData['notSampled'] = dict()

                            ensembleData['currentNotSampled'] = -1
                            ensembleStack.append(ensembleData['notSampled'])
                            ensembleData = ensembleStack[-1]
                            break
                        else:
                            del ensembleData['currentSampled']
                            del ensembleData['currentNotSampled']

                    if 'incremented' in ensembleData:
                        del ensembleData['incremented']
                        ensembleStack.pop()
                        ensembleData = ensembleStack[-1]
                    else:
                        break

                if not 'targetSetSize' in ensembleData:                   
                    ensembleData['targetSetSize'] = 0
                    if part[9] == 'ignored':
                        ensembleData['ignored'] = True

                ensembleData['targetSetSize'] += targetSetSize
                ensembleData['incremented'] = True

                continue

            continue

        # [2020-06-01 15:39:38.703324] [info] [1:0] CSampling: Sampled set size: '316698', Not sampled set size: '229658'

        if keyword == 'CSampling:':
            ensembleData['currentSampled'] = int(part[8].strip('"\'.,'))
            ensembleData['currentNotSampled'] = int(part[13].strip('"\'.,'))
            continue

        continue

outfile_path = Path('EpiHiperLogSummary.json')
with outfile_path.open('w') as outfile_handle:
    json.dump(summary, outfile_handle, indent=2)
