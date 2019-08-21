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

#include "db/CQuery.h"
#include "db/CConnection.h"
#include "db/CSchema.h"
#include "db/CFieldValueList.h"

// static
bool CQuery::exec(const std::string & table,
                   const std::string & resultField,
                   CFieldValueList & result)
{
  pqxx::work * pWork = CConnection::work();

  if (pWork == NULL) return false;

  const CTable & Table = CSchema::INSTANCE.getTable(table);

  if (!Table.isValid()) return false;

  const CField & ResultField = Table.getField(resultField);

  if (!ResultField.isValid()) return false;

  // TODO CRITICAL Execute qurey and compile result

  delete pWork;
  return false;
}

// static
bool CQuery::exec(const std::string & table,
                   const std::string & resultField,
                   CFieldValueList & result,
                   const std::string & constraintField,
                   const CFieldValueList & constraint)
{
  pqxx::work * pWork = CConnection::work();

  if (pWork == NULL) return false;

  const CTable & Table = CSchema::INSTANCE.getTable(table);

  if (!Table.isValid()) return false;

  const CField & ResultField = Table.getField(resultField);

  if (!ResultField.isValid()) return false;

  const CField & ConstraintField = Table.getField(constraintField);

  if (!ConstraintField.isValid()) return false;

  if (constraint.size() > 0 && constraint.getType() != ConstraintField.getType()) return false;

  // TODO CRITICAL Execute qurey and compile result
  std::string Query = "select " + resultField + " from " + table + " where " + constraintField + " in ";

  delete pWork;
  return false;
}
