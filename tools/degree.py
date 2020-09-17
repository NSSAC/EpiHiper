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

import csv
import os
import argparse
import hashlib

# ----------------------------------------------------------------------
def CreateParser() :
    cli_parser = argparse.ArgumentParser(description='Update hids in activity seqeunce')

    cli_parser.add_argument('-n',
                            '--network_file',
                            required = True,
                            dest = 'network_file',
                            type = str,
                            help="Network file")

    cli_parser.add_argument('-o',
                            '--output_file',
                            required = False,
                            dest = 'output_file',
                            type = str,
                            default = None,
                            help="Output file")

    return cli_parser


# ----------------------------------------------------------------------

def main() :

    # ----------------------------------------------------------------------
    # Parser construction
    cli_parser = CreateParser()
    args = cli_parser.parse_args()

    if not os.path.exists(args.network_file) :
        print(f"Missing file <{args.network_file}>")
        exit(-1)
    else :
        print(f"Using network file:  <{args.network_file}>")

    if args.output_file == None:
      args.output_file = os.path.splitext(args.network_file)[0] + '_degree.csv'

    network = open(args.network_file)
    # skip JSON header
    network.readline() 
    network_reader = csv.DictReader(network)

    JSON = '{"epiHiperSchema":"https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/csvDataResourceSchema.json","description":"Degree","name":"degree","title":"Degree","profile":"tabular-data-resource","path":"' +  os.path.basename(args.output_file) + '","schema":{"fields":[{"title":"Person Id","name":"pid","type":"number"},{"title":"Degree","name":"degree","type":"number"},{"title":"Weighted Degree","name":"weighted_degree","type":"number"},"primaryKey":["pid"]}}'
    output = open(args.output_file, 'w')
    output.write(JSON + '\n')

    header = "pid,degree,weighted_degree"
    output_writer = csv.DictWriter(output, fieldnames=header.split(','))
    output_writer.writeheader()

    current_pid = -1
    degree = 0
    weighted_degree = 0

    for row in network_reader:
        if row['targetPID'] != current_pid:
            if current_pid != -1:
                output_writer.writerow({'pid': current_pid, 'degree': degree, 'weighted_degree': weighted_degree/86400})

            current_pid = row['targetPID']
            degree = 0
            weighted_degree = 0
        
        degree += 1
        weighted_degree += int(row['duration'])

    output_writer.writerow({'pid': current_pid, 'degree': degree, 'weighted_degree': weighted_degree/86400})

main()
