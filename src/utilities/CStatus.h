// BEGIN: Copyright 
// Copyright (C) 2019 - 2020 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

/*
 * CStatus.h
 *
 *  Created on: Sep 4, 2019
 *      Author: shoops
 */

#ifndef SRC_UTILITIES_CSTATUS_H_
#define SRC_UTILITIES_CSTATUS_H_

#include <string>
#include <limits>

struct json_t;

class CStatus
{
public:
  static void load(const std::string & id, const std::string & name, const std::string &file);

  static void update(const std::string & status, const double & progress = std::numeric_limits< double >::quiet_NaN());

  static void success();

private:
  static json_t * pJSON;
  static json_t * pId;
  static json_t * pName;
  static json_t * pStatus;
  static json_t * pDetail;
  static json_t * pProgress;
  static std::string fileName;
};

#endif /* SRC_UTILITIES_CSTATUS_H_ */
