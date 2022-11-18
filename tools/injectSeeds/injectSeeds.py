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

"""
Create an EpiHiper external seeding input file from . . .
1. seeding data file with total number affected per day
"""

import numpy as np
import csv
import json
import requests
from jsonschema import validate as validate_json
from copy import deepcopy
from pathlib import Path
from pprint import pformat # format strings with pretty print for readability
from logging import (getLogger, getLevelName, basicConfig as logging_basicConfig,
        CRITICAL, ERROR, WARNING, INFO, DEBUG)
logger = getLogger(__name__)
logger.setLevel(WARNING)

# The following will set up the root logger handler with a default formatter
logging_basicConfig()

logger.info(f'Starting logging level: {getLevelName(logger.level)}')

day_trigger = {
  "ann:id": "triggerSeedingDay_DD",
  "ann:description": "This needs to be created for each scheduled seeding day.",
  "trigger": {
    "left": {"observable": "time"},
    "operator": "==",
    "right": {
      "value": {"number": 0}
    }
  },
  "interventionIds": []
}

day_seed_intervention = {
  "ann:description": "This needs to be created for each scheduled seeding day. Note the set 'availableForSeeding' provided.",
  "id": "interventionSeedingDay_DD",
  "target": {
    "set": {"idRef": "availableForSeeding"}
  },
  "sampling": {
    "type": "absolute",
    "number": 0,
    "sampled": {
      "foreach": [
        {
          "operations": [
            {
              "target": {
                "node": {
                  "property": "healthState"
                }
              },
              "operator": "=",
              "value": {
                "healthState": "E_a"
              }
            }
          ]
        }
      ]
    }
  }
}

def main(input):
    """
    A main entry point into this module or library of functionality.
    """

    if 'log_level' in input:
        logger.setLevel(input['log_level'].upper())

    logger.info(f'Logging level in main function: {getLevelName(logger.level)}')

    assert 'seed_filename' in input
    seeds_file_path = Path(input['seed_filename'])
    assert seeds_file_path.exists()
    logger.info(f'seeds_file_path = {seeds_file_path}')

    assert 'intervention_filename' in input
    intervention_file_path = Path(input['intervention_filename'])
    assert intervention_file_path.exists()
    logger.info(f'intervention_file_path = {intervention_file_path}')
   
    assert 'output_filename' in input
    output_file_path = Path(input['output_filename'])
   
    try:
        jsonFile = open(intervention_file_path,"r")
        
    except:
        sys.exit("ERROR: File '" + intervention_file_path + "' does not exist.")
        
    template_data = json.load(jsonFile)
    jsonFile.close()
 
    # Load fips data into a dictionary
    seed_data = {}

    with seeds_file_path.open() as seeds_file_handle:
      # Eat the header line
      seeds_file_handle.readline()

      csv_reader = csv.reader(seeds_file_handle, delimiter = ',')

      logger.info("loading seed file data")
      for tick, seeds in csv_reader:
        logger.debug(f"day: {tick},   seeds: {seeds}")

        seed_data.setdefault(int(tick), dict())
        seed_data[int(tick)].setdefault('seeds', int(seeds))

      logger.debug(f"seed_data = {pformat(seed_data)}")

    logger.info(f"adding all interventions")

    logger.info(f"adding the trigger for each day")
    for tick in seed_data.keys():
      tick_data = seed_data[tick]

      if tick_data['seeds'] == 0: continue

      trigger = deepcopy(day_trigger)
      trigger['ann:id'] = trigger['ann:id'].replace('DD', str(tick))
      trigger['trigger']['right']['value']['number'] = tick

      intervention = deepcopy(day_seed_intervention)
      intervention['id'] = intervention['id'].replace('DD', str(tick))
      intervention['sampling']['number'] = tick_data['seeds']
      template_data['interventions'].append(intervention)

      trigger['interventionIds'].append(intervention['id'])
      template_data['triggers'].append(trigger)

    logger.info(f"writing output to {output_file_path.absolute()}")
    with output_file_path.open('w') as outfile_handle:
        json.dump(template_data, outfile_handle, indent=2)

if __name__ == '__main__':
    import argparse

    # Create the parser
    cli_parser = argparse.ArgumentParser(description='Build an EpiHiper intervention file seeding counts a template json.')

    cli_parser.add_argument('-s',
                          '--seed_filename',
                          dest = 'seed_filename',
                          type = str,
                          required=True,
                          help="Seed filename")

    cli_parser.add_argument('-i',
                          '--intervention_filename',
                          dest = 'intervention_filename',
                          type = str,
                          required = True,
                          help="Intervention file to be augmented")

    cli_parser.add_argument('-o',
                          '--output_filename',
                          dest = 'output_filename',
                          type = str,
                          required = True,
                          help="Output filename")

    # Generate appropriate choices to limit command line input. Can be dynamically generated.
    loglevel_choices = tuple( getLevelName(level).lower() for level in ( CRITICAL, ERROR, WARNING, INFO, DEBUG ) )

    cli_parser.add_argument('-l',
                            '--log_level',   # Long version of flag argument,
                            choices = loglevel_choices, # Limit the valid choices. --help will display these.
                            type = str,
                            default = 'error',           # Value to use if it is empty
                            help ='Set the amount of information about the run you wish to see.')

    args = cli_parser.parse_args()

    config = {}

#    # The default config
#    default_config_file = Path('default_config.json').resolve(strict = True)
#    config = json.load(default_config_file.open())
#    default_config_report = f"config from default file: {pformat(config)}"

#    # Update with custom config
#    custom_config_report = None
#    if args.config_filename and args.config_filename.endswith('.json'):
#        custom_config_file = Path(args.config_filename).resolve(strict = True)
#        config.update(json.load(custom_config_file.open()))
#        custom_config_report = f"config update with custom file: {pformat(config)}"

    # Make sure the config is valid.
#    config_schema_file = Path('schema.json').resolve(strict = True)
#    config_schema = json.load(config_schema_file.open())
#    validate_json(instance = config, schema = config_schema)

    # Update with config options from the parser (only use ones with values set).
    commandline_config = {key: val for key, val in vars(args).items() if val}

    if commandline_config:
        config.update(commandline_config)
        commandline_config_report = f"config update from command line: {pformat(config)}"

    # Finally set the log level, so it's consistent, before logging output.
    if 'log_level' in config:
        logger.setLevel(config['log_level'].upper())

#logger.info(default_config_report)
#    if custom_config_report: logger.info(custom_config_report)
    if commandline_config: logger.info(commandline_config_report)

    main(config)

