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
#include "network/CNetwork.h"
#include "network/CNode.h"

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
std::string CQuery::LocalConstraint = "";

// static
void CQuery::init()
{
  if (LocalConstraint.empty())
    {
      std::ostringstream Query;

      CNode * pBegin = CNetwork::INSTANCE->beginNode();
      CNode * pEnd = CNetwork::INSTANCE->endNode() - 1;
      Query << "pid BETWEEN " << pBegin->id << " AND " <<  pEnd->id;

      LocalConstraint = Query.str();
    }
}
// static
bool CQuery::all(const std::string & table,
                   const std::string & resultField,
                   CFieldValueList & result,
                   const bool & local)
{
  init();
  pqxx::read_transaction * pWork = CConnection::work();

  if (pWork == NULL) return false;

  const CTable & Table = CSchema::INSTANCE.getTable(table);

  if (!Table.isValid()) return false;

  const CField & ResultField = Table.getField(resultField);

  if (!ResultField.isValid()) return false;

  std::ostringstream Query;
  Query << "SELECT DISTINCT " << resultField << " FROM " << table;

  if (local)
    {
      Query << " WHERE " << LocalConstraint;
    }

  Query << ";";

  try
  {
    toFieldValueList(ResultField, pWork->exec(Query.str()), result);
  }

  catch (const pqxx::pqxx_exception & e)
  {
    std::cerr << e.base().what() << std::endl;
  }

  delete pWork;

  return true;
}

// static
bool CQuery::in(const std::string & table,
                   const std::string & resultField,
                   CFieldValueList & result,
                   const bool & local,
                   const std::string & constraintField,
                   const CValueList & constraints,
                   const bool & in)
{
  // We need to handle the case where the constraints are empty.
  if (constraints.size() == 0)
    {
      if (in)
        {
          return true;
        }
      else
        {
          return all(table, resultField, result, local);
        }
    }

  init();
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

  Query << "SELECT DISTINCT " << resultField << " FROM " << table << " WHERE " << constraintField << ((!in) ? " NOT  IN (" : " IN (");

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

  if (local)
    {
      Query << " AND " << LocalConstraint;
    }

  try
  {
    toFieldValueList(ResultField, pWork->exec(Query.str()), result);
  }

  catch (const pqxx::pqxx_exception & e)
  {
    std::cerr << e.base().what() << std::endl;
  }

  delete pWork;

  return true;
}


// static
bool CQuery::notIn(const std::string & table,
                 const std::string & resultField,
                 CFieldValueList & result,
                 const bool & local,
                 const std::string & constraintField,
                 const CValueList & constraint)
{
  return in(table, resultField, result, local, constraintField, constraint, false);
}

// static
bool CQuery::where(const std::string & table,
                 const std::string & resultField,
                 CFieldValueList & result,
                 const bool & local,
                 const std::string & constraintField,
                 const CValue & constraint,
                 const std::string & cmp)
{
  init();
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
  Query << "SELECT DISTINCT " << resultField << " FROM " << table << " WHERE " << constraintField << " " << cmp << " ";

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

  if (local)
    {
      Query << " AND " << LocalConstraint;
    }

  Query << ";";

  try
  {
    toFieldValueList(ResultField, pWork->exec(Query.str()), result);
  }

  catch (const pqxx::pqxx_exception & e)
  {
    std::cerr << e.base().what() << std::endl;
  }

 delete pWork;

  return true;
}

