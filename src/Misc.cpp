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

#include <sstream>
#include "Misc.h"

std::string trim(const std::string& s, const std::string& r) {
  std::size_t foundStart = s.find_first_not_of(r);
  if (foundStart == std::string::npos) return "";
  std::size_t foundEnd = s.find_last_not_of(r);
  return s.substr(foundStart, foundEnd-foundStart+1);
}

std::vector<std::string> split(const std::string& s, char c) {
  std::vector<std::string> items;
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, c)) {
    items.push_back(item);
  }
  return items;
}

