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

county_node_template = {
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

age_groups = tuple('psaog')
num_groups = len(age_groups)

county_agegroup_template = {
  "ann:description": "This needs to be created for each county by age group.",
  "id": "NNNNN_A",
  "scope": "local",
  "content": {
    "operation": "intersection",
    "sets": [
      {
        "set": {"idRef": "county_NNNNN"}
      },
      {
        "set": {"idRef": "agegroup_A"}
      }
    ]
  }
}

county_agegroup_init_template = {
  "ann:description": "This needs to be created for each county by age group set NNNNN_?.",
  "target": {
    "set": {"idRef": "NNNNN_A"}
  }
}
 
sampling_template = {
  "type": "absolute",
  "number": None,
  "sampled": {
    "foreach": [
      {
        "delay": None,
        "operations": [
          {
            "target": {
              "node": {"property": "healthState"}
            },
            "operator": "=",
            "value": {"healthState": "E_AGEGROUP"}
          }
        ]
      }
    ]
  },
  "nonsampled": {}
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
   
    assert 'template_file' in input
    template_file_path = Path(input['template_file'])
    assert template_file_path.exists()
    logger.info(f'template_file_path = {template_file_path}')
   
    # Load fips data into a dictionary
    fips_data = {}
    with seeds_file_path.open() as seeds_file_handle:
        # Eat the header line
        header = seeds_file_handle.readline()

        csv_reader = csv.reader(seeds_file_handle, delimiter = ',')

        logger.info("loading seed file data")
        for fips, day, number in csv_reader:
            logger.debug(f"fips: {fips}   day: {day},   number: {number}")

            fips_data.setdefault(fips, dict())[int(day)] = int(number)

        logger.debug(f"fips_data = {pformat(fips_data)}")

    logger.info("loading template file data")
    template_data = json.load(template_file_path.open())

    logger.info(f"adding all the country nodes to the 'set' array")
    for fip in fips_data.keys():
        county_node = deepcopy(county_node_template)
        county_node['id'] = county_node['id'].replace('NNNNN', str(fip))
        county_node['content']['right']['value'] = county_node['content']['right']['value'].replace('NNNNN', str(fip))
        template_data['sets'].append(county_node)

    logger.info(f"adding the county to agegroup combinations to the 'set' array")
    for fip in fips_data.keys():
        for agegroup in age_groups:
            county_agegroup = deepcopy(county_agegroup_template)
            county_agegroup['id'] = county_agegroup['id'].replace('NNNNN_A', f"{fip}_{agegroup}")
            sets_list = county_agegroup['content']['sets']
            for template_set in sets_list:
                template_set['set']['idRef'] = template_set['set']['idRef'].replace(
                        'NNNNN', fip).replace('agegroup_A', f"agegroup_{agegroup}")
            template_data['sets'].append(county_agegroup)

    logger.info(f"adding the initialization numbers, divided between the age groups, for the days")
    for fip in fips_data.keys():
        day_nums = sorted(fips_data[fip].items())
        last_day = day_nums[-1][0]
        sampling_result = dict()
        for day, num in day_nums:
            sampling_result[day] = np.random.multinomial(num,[0.00675988,0.01757569,0.46433406,0.27091976,0.24041061],size=1)
        for index in range(num_groups):
            agegroup = age_groups[index]
            county_agegroup_init = deepcopy(county_agegroup_init_template)
            county_agegroup_init['target']['set']['idRef'] = county_agegroup_init['target']['set']['idRef'].replace(
                    'NNNNN_A', f"{fip}_{agegroup}")

            county_agegroup_init['sampling'] = deepcopy(sampling_template)
            sampling = county_agegroup_init['sampling']
            for day, num in day_nums:
                #sampling['number'] = round(num/num_groups)
                sampling['number'] = int(sampling_result[day][0][index])
                sampling['sampled']['foreach'][0]['delay'] = day
                sampling['sampled']['foreach'][0]['operations'][0]['value']['healthState'] = sampling['sampled']['foreach'][0]['operations'][0]['value']['healthState'].replace('AGEGROUP', agegroup )
                if day == last_day: break
                sampling['nonsampled']['sampling'] = deepcopy(sampling_template)
                sampling = sampling['nonsampled']['sampling']

            sampling.pop('nonsampled')
            template_data['initializations'].append(county_agegroup_init)

    logger.debug(f"template_data = {pformat(template_data['initializations'])}")

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
        outfile_path = Path(template_file_path.parent / template_file_path.name.replace(
            template_file_path.stem, template_file_path.stem + 'filled').replace('template', ''))

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

    cli_parser.add_argument('template_file',
                           type = str,
                           default = None,           # Value to use if it is empty
                           help ='json file containing expected structure and group breakdown')

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
