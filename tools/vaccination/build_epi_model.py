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

"""
Create an EpiHiper county model input file from . . .
1. fips data file with total number affected per day
2. a template describing categories of affected individuals
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

county_node_set = {
  "ann:description": "This needs to be created for each county.",
  "id": "county_NNNNN",
  "scope": "local",
  "content": {
    "elementType": "node",
    "scope": "local",
    "left": {"field": "county_fips"},
    "operator": "==",
    "right": {"value": "NNNNN"}
  }
}

agegroup_node_set = {
  "id": "agegroup_A",
  "scope": "local",
  "content": {
    "elementType": "node",
    "scope": "local",
    "left": {"field": "age_group"},
    "operator": "==",
    "right": {"value": "A"}
  }
}

age_groups = tuple('psaog')
num_groups = len(age_groups)

day_trigger = {
  "ann:id": "triggerVacinationDay_DD",
  "ann:description": "This needs to be created for each scheduled vacination day.",
  "trigger": {
    "left": {"observable": "time"},
    "operator": "==",
    "right": {
      "value": {"number": 0}
    }
  },
  "interventionIds": []
}

day_county_agegroup_intervention = {
  "ann:description": "This needs to be created for each county by age group set NNNNN_?. Note the sets are provided by county initilization",
  "id": "interventionVacinationDay_DD_NNNNN_A",
  "target": {
    "operation": "intersection",
    "sets": [
      {
        "set": {"idRef": "county_NNNNN"}
      },
      {
        "set": {"idRef": "agegroup_A"}
      },
      {
        "set": {"idRef": "availableForVaccination"}
      }
    ]
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
                  "property": "nodeTrait",
                  "feature": "vaccinated"
                }
              },
              "operator": "=",
              "value": {
                "trait": "nodeTrait",
                "feature": "vaccinated",
                "enum": "true"
              }
            }
          ]
        },
        {
          "delay": 10,
          "condition": {
            "left": {
              "node": {"property": "healthState"}
            },
            "operator": "==",
            "right": {
              "value": {"healthState": "S_A"}
            }
          },
          "operations": [
            {
              "target": {
                "node": {"property": "healthState"}
              },
              "operator": "=",
              "value": {"healthState": "HypRxProt_A"}
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

    assert 'seeds_file' in input
    seeds_file_path = Path(input['seeds_file'])
    assert seeds_file_path.exists()
    logger.info(f'seeds_file_path = {seeds_file_path}')
   
    template_data = {
      "epiHiperSchema": "https://raw.githubusercontent.com/NSSAC/EpiHiper-Schema/master/schema/interventionSchema.json",
      "sets": [], 
      "triggers": [], 
      "interventions": []
    }

    for A in age_groups:
      agegroup_node = deepcopy(agegroup_node_set)
      agegroup_node['id'] = agegroup_node['id'].replace('A', A)
      agegroup_node['content']['right']['value'] = agegroup_node['content']['right']['value'].replace('A', A)
      template_data['sets'].append(agegroup_node)
      
    # Load fips data into a dictionary
    seed_data = {}

    with seeds_file_path.open() as seeds_file_handle:
      # Eat the header line
      seeds_file_handle.readline()

      csv_reader = csv.reader(seeds_file_handle, delimiter = ',')

      logger.info("loading seed file data")
      for day, fips, agegroup, number in csv_reader:
        logger.debug(f"day: {day},   fips: {fips},   agegroup: {agegroup},   number: {number}")

        seed_data.setdefault(int(day), dict())
        seed_data[int(day)].setdefault(fips, dict())
        seed_data[int(day)][fips].setdefault(agegroup, dict())
        seed_data[int(day)][fips][agegroup] = int(number)

      logger.debug(f"seed_data = {pformat(seed_data)}")

    logger.info(f"adding all the country nodes to the 'set' array")

    logger.info(f"adding the trigger for each day")
    for day in seed_data.keys():
      day_data = seed_data[day]
      trigger = deepcopy(day_trigger)
      trigger['ann:id'] = trigger['ann:id'].replace('DD', str(day))
      trigger['trigger']['right']['value']['number'] = day

      for fips in day_data.keys():
        fips_data = day_data[fips]
        county_node = deepcopy(county_node_set)
        county_node['id'] = county_node['id'].replace('NNNNN', str(fips))
        county_node['content']['right']['value'] = county_node['content']['right']['value'].replace('NNNNN', str(fips))
        template_data['sets'].append(county_node)

        for agegroup in fips_data.keys():
          intervention = deepcopy(day_county_agegroup_intervention)
          intervention['id'] = intervention['id'].replace('DD', str(day))
          intervention['id'] = intervention['id'].replace('NNNNN', fips)
          intervention['id'] = intervention['id'].replace('A', agegroup)
          intervention['target']['sets'][0]['set']['idRef'] = intervention['target']['sets'][0]['set']['idRef']. replace('NNNNN', fips)     
          intervention['target']['sets'][1]['set']['idRef'] = intervention['target']['sets'][1]['set']['idRef']. replace('A', agegroup)     
          intervention['sampling']['number'] = fips_data[agegroup]
          intervention['sampling']['sampled']['foreach'][1]['condition']['right']['value']['healthState'] = \
            intervention['sampling']['sampled']['foreach'][1]['condition']['right']['value']['healthState'].replace('A', agegroup)
          intervention['sampling']['sampled']['foreach'][1]['operations'][0]['value']['healthState'] = \
            intervention['sampling']['sampled']['foreach'][1]['operations'][0]['value']['healthState'].replace('A', agegroup)
          template_data['interventions'].append(intervention)
          trigger['interventionIds'].append(intervention['id'])

      template_data['triggers'].append(trigger)

    logger.debug(f"template_data = {pformat(template_data)}")

#    logger.info(f"validating json data")
#    epihiper_schema_source = "https://raw.githubusercontent.com/NSSAC/EpiHiper-Schema/master/schema/initializationSchema.json"
#    response = requests.get(epihiper_schema_source)
#
#    if not response.ok:
#        logger.warning(f"can't get schema from {epihiper_schema_source}")
#        epihiper_schema_source = Path('initializationSchema.json')
#        logger.warning(f"falling back to local (outdated?) {epihiper_schema_source.absolute()}")
#        assert epihiper_schema_source.exists()
#        epihiper_schema = json.load(epihiper_schema_source.open('r'))
#    else:
#        logger.info(f"epihiper_schema_source = {epihiper_schema_source}")
#        epihiper_schema = response.text
#
#    logger.debug(f"epihiper_schema = {pformat(epihiper_schema)}")
#
#    validate_json(template_data, epihiper_schema)

    if 'output_file' in input:
        outfile_path = Path(input['output_file'])
    else:
        outfile_path = Path(seeds_file_path.parent / seeds_file_path.name.replace('csv', 'json'))

    logger.info(f"writing output to {outfile_path.absolute()}")
    with outfile_path.open('w') as outfile_handle:
        json.dump(template_data, outfile_handle, indent=2)

if __name__ == '__main__':
    import argparse

    # Create the parser
    cli_parser = argparse.ArgumentParser(description='Build an EpiHiper model from fips counts a template json.')

    cli_parser.add_argument('seeds_file',
                           type = str,
                           default = None,           # Value to use if it is empty
                           help ='file containing daily counts by county')

    cli_parser.add_argument('output_file',
                           type = str,
                           nargs = '?',
                           default = None,           # Value to use if it is empty
                           help ='specify the output file (default: template name file "filled" insterted and "template" removed')

    # Generate appropriate choices to limit command line input. Can be dynamically generated.
    loglevel_choices = tuple( getLevelName(level).lower() for level in ( CRITICAL, ERROR, WARNING, INFO, DEBUG ) )

    cli_parser.add_argument('-l',
                           '--log_level',   # Long version of flag argument,
                           choices = loglevel_choices, # Limit the valid choices. --help will display these.
                           type = str,
                           default = None,           # Value to use if it is empty
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
