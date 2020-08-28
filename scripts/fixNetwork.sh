#!/usr/bin/env bash

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

mv $1 $$.tmp
sed -e 's/^\([0-9]*,\)\([0-9]*,[0-9]*,\)/\11:\21:/' $$.tmp > $1 
