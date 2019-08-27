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

/*
 * CQuery.h
 *
 *  Created on: Aug 8, 2019
 *      Author: shoops
 */

#ifndef SRC_DB_CQUERY_H_
#define SRC_DB_CQUERY_H_

#include <string>

class CFieldValue;
class CFieldValueList;

struct CQuery
{
public:
  static bool all(const std::string & table,
                   const std::string & resultField,
                   CFieldValueList & result,
                   const bool & local);

  static bool in(const std::string & table,
                   const std::string & resultField,
                   CFieldValueList & result,
                   const bool & local,
                   const std::string & constraintField,
                   const CFieldValueList & constraint,
                   const bool & in = true);

  static bool notIn(const std::string & table,
                   const std::string & resultField,
                   CFieldValueList & result,
                   const bool & local,
                   const std::string & constraintField,
                   const CFieldValueList & constraint);

  static bool where(const std::string & table,
                   const std::string & resultField,
                   CFieldValueList & result,
                   const bool & local,
                   const std::string & constraintField,
                   const CFieldValue & constraint,
                   const std::string & cmp);

private:
  static std::string LocalConstraint;
  static void init();
};

#endif /* SRC_DB_CQUERY_H_ */
