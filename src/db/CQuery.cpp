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

#include "db/CQuery.h"
#include "db/CConnection.h"
#include "db/CSchema.h"
#include "db/CFieldValueList.h"
#include "db/CFieldValue.h"

void toIdFieldValueList(const pqxx::result & result, CFieldValueList & list)
{
  size_t Default(-1);

  for (const pqxx::row &Row: result)
   for (const pqxx::field &Field: Row)
     list.append(CFieldValue(Field.as(Default)));
}

void toStringFieldValueList(const pqxx::result & result, CFieldValueList & list)
{
  std::string Default("");

  for (const pqxx::row &Row: result)
    for (const pqxx::field &Field: Row)
      list.append(CFieldValue(Field.as(Default)));
}

void toNumberFieldValueList(const pqxx::result & result, CFieldValueList & list)
{
  double Default(std::numeric_limits< double >::quiet_NaN());

  for (const pqxx::row &Row: result)
    for (const pqxx::field &Field: Row)
      list.append(CFieldValue(Field.as(Default)));
}

void toFieldValueList(const CField & field, const pqxx::result & result, CFieldValueList & list)
{
  switch (field.getType())
  {
    case CFieldValueList::Type::id:
      toIdFieldValueList(result, list);
      break;

    case CFieldValueList::Type::string:
      toStringFieldValueList(result, list);
      break;

    case CFieldValueList::Type::number:
      toNumberFieldValueList(result, list);
      break;
  }
}

// static
bool CQuery::all(const std::string & table,
                   const std::string & resultField,
                   CFieldValueList & result)
{
  pqxx::read_transaction * pWork = CConnection::work();

  if (pWork == NULL) return false;

  const CTable & Table = CSchema::INSTANCE.getTable(table);

  if (!Table.isValid()) return false;

  const CField & ResultField = Table.getField(resultField);

  if (!ResultField.isValid()) return false;

  std::ostringstream Query;
  Query << "select " << resultField << " from " << table << ";";

  toFieldValueList(ResultField, pWork->exec(Query.str()), result);

  delete pWork;

  return true;
}

// static
bool CQuery::in(const std::string & table,
                   const std::string & resultField,
                   CFieldValueList & result,
                   const std::string & constraintField,
                   const CFieldValueList & constraints,
                   const bool & in)
{
  pqxx::read_transaction * pWork = CConnection::work();

  if (pWork == NULL) return false;

  const CTable & Table = CSchema::INSTANCE.getTable(table);

  if (!Table.isValid()) return false;

  const CField & ResultField = Table.getField(resultField);

  if (!ResultField.isValid()) return false;

  const CField & ConstraintField = Table.getField(constraintField);

  if (!ConstraintField.isValid()) return false;

  if (constraints.size() > 0 && constraints.getType() != ConstraintField.getType()) return false;

  std::ostringstream Query;
  Query << "select " << resultField << " from " << table << " where " << constraintField << (!in) ? " not  in (" : " in (";

  bool FirstTime = true;
  CFieldValueList::const_iterator it = constraints.begin();
  CFieldValueList::const_iterator end = constraints.end();

  for (; it != end; ++it)
    {
      if (FirstTime)
        {
          FirstTime = false;
        }
      else
        {
          Query << ", ";
        }

      switch (constraints.getType())
      {
        case CFieldValueList::Type::id:
          Query << it->toId();
          break;

        case CFieldValueList::Type::string:
          Query << "'" << it->toString() << "'";
          break;

        case CFieldValueList::Type::number:
          Query << it->toNumber();
          break;
      }
    }

  Query << ");";

  toFieldValueList(ResultField, pWork->exec(Query.str()), result);

  delete pWork;

  return true;
}


// static
bool CQuery::notIn(const std::string & table,
                 const std::string & resultField,
                 CFieldValueList & result,
                 const std::string & constraintField,
                 const CFieldValueList & constraint)
{
  return in(table, resultField, result, constraintField, constraint, false);
}

// static
bool CQuery::where(const std::string & table,
                 const std::string & resultField,
                 CFieldValueList & result,
                 const std::string & constraintField,
                 const CFieldValue & constraint,
                 const std::string & cmp)
{
  pqxx::read_transaction * pWork = CConnection::work();

  if (pWork == NULL) return false;

  const CTable & Table = CSchema::INSTANCE.getTable(table);

  if (!Table.isValid()) return false;

  const CField & ResultField = Table.getField(resultField);

  if (!ResultField.isValid()) return false;

  const CField & ConstraintField = Table.getField(constraintField);

  if (!ConstraintField.isValid()) return false;

  if (constraint.getType() != ConstraintField.getType()) return false;

  std::ostringstream Query;
  Query << "select " << resultField << " from " << table << " where " << constraintField << " " << cmp << " ";

  switch (constraint.getType())
  {
    case CFieldValueList::Type::id:
      Query << constraint.toId();
      break;

    case CFieldValueList::Type::string:
      Query << "'" << constraint.toString() << "'";
      break;

    case CFieldValueList::Type::number:
      Query << constraint.toNumber();
      break;
  }

  Query << ";";

  toFieldValueList(ResultField, pWork->exec(Query.str()), result);

  delete pWork;

  return true;
}

