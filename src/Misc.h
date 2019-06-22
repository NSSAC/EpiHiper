// BEGIN: Copyright 
// Copyright (C) 2019 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

// miscellaneous helper functions

#ifndef EPIHIPER_MISC_H
#define EPIHIPER_MISC_H

#include <string>
#include <vector>

// remove leading and trailing white spaces (and specified characters)
std::string trim(const std::string& s, const std::string& r);

// split string by given char delimiter
std::vector<std::string> split(const std::string& s, char c);

// choose an integer randomly in [0,i)
int myRandom(int i);

#endif
