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
 * CDBDefinition.h
 *
 *  Created on: Aug 8, 2019
 *      Author: shoops
 */

#ifndef SRC_DB_CSCHEMA_H_
#define SRC_DB_CSCHEMA_H_

#include <map>
#include "db/CTable.h"

class CSchema
{
public:
  static CSchema INSTANCE;

  static void load(const std::vector< std::string > & schemaFiles);

  CSchema();

  CSchema(const CSchema & src);

  virtual ~CSchema();
  
  virtual void fromJSON(const json_t * json);

  const std::string & getId() const;

  const bool & isValid() const;

  const CTable & getTable(const std::string & table) const;

  const std::string & getTableForField(const std::string & field) const;

private:
  bool addTable(const json_t * json);

  std::string mId;
  std::string mLabel;
  std::map< std::string, CTable > mTables;
  std::map< std::string, std::string > mFieldToTable;
  bool mValid;
};

#endif /* SRC_DB_CSCHEMA_H_ */
