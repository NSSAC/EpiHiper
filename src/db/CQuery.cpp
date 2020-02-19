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

#include <sstream>

#include "db/CQuery.h"
#include "db/CConnection.h"
#include "db/CSchema.h"
#include "db/CFieldValueList.h"
#include "db/CFieldValue.h"
#include "network/CNetwork.h"
#include "network/CNode.h"
#include "utilities/CSimConfig.h"

size_t toIdFieldValueList(const pqxx::result & result, CFieldValueList & list)
{
  size_t Default(-1);
  size_t OldSize = list.size();

  for (const pqxx::row & Row : result)
    for (const pqxx::field & Field : Row)
      list.append(CFieldValue(Field.as(Default)));

  return list.size() - OldSize;
}

size_t toStringFieldValueList(const pqxx::result & result, CFieldValueList & list)
{
  std::string Default("");
  size_t OldSize = list.size();

  for (const pqxx::row & Row : result)
    for (const pqxx::field & Field : Row)
      list.append(CFieldValue(Field.as(Default)));

  return list.size() - OldSize;
}

size_t toNumberFieldValueList(const pqxx::result & result, CFieldValueList & list)
{
  double Default(std::numeric_limits< double >::quiet_NaN());
  size_t OldSize = list.size();

  for (const pqxx::row & Row : result)
    for (const pqxx::field & Field : Row)
      list.append(CFieldValue(Field.as(Default)));

  return list.size() - OldSize;
}

size_t toFieldValueList(const CField & field, const pqxx::result & result, CFieldValueList & list)
{
  switch (field.getType())
    {
    case CFieldValueList::Type::id:
      return toIdFieldValueList(result, list);
      break;

    case CFieldValueList::Type::string:
      return toStringFieldValueList(result, list);
      break;

    case CFieldValueList::Type::number:
      return toNumberFieldValueList(result, list);
      break;
    }

  return 0;
}

// static
std::string CQuery::LocalConstraint = "";

// static
size_t CQuery::Limit = 100000;

// static
void CQuery::init()
{
  if (LocalConstraint.empty())
    {
      std::ostringstream Query;

      CNode * pBegin = CNetwork::INSTANCE->beginNode();
      CNode * pEnd = CNetwork::INSTANCE->endNode() - 1;
      Query << "pid BETWEEN " << pBegin->id << " AND " << pEnd->id;

      LocalConstraint = Query.str();

      Limit = CSimConfig::getDBConnection().maxRecords;
    }
}

// static
std::string CQuery::limit(size_t & offset)
{
  if (Limit == 0)
    {
      return "";
    }

  std::ostringstream Query;
  Query << " LIMIT " << Limit;

  if (offset != 0)
    Query << " OFFSET " << offset;

  offset += Limit;

  return Query.str();
}

// static
bool CQuery::all(const std::string & table,
                 const std::string & resultField,
                 CFieldValueList & result,
                 const bool & local)
{
  init();

  const CTable & Table = CSchema::INSTANCE.getTable(table);

  if (!Table.isValid())
    return false;

  const CField & ResultField = Table.getField(resultField);

  if (!ResultField.isValid())
    return false;

  std::ostringstream Query;
  Query << "SELECT DISTINCT " << resultField << " FROM " << table;

  if (local)
    {
      Query << " WHERE " << LocalConstraint;
    }

  Query << " ORDER BY " << resultField;
  // std::cout << Query.str() << std::endl;

  pqxx::read_transaction * pWork = CConnection::work();

  if (pWork == NULL)
    return false;

  try
    {
      size_t Offset = 0;
      while (toFieldValueList(ResultField, pWork->exec(Query.str() + limit(Offset)), result) == Limit
             && Limit != 0)
        continue;
    }

  catch (const std::exception & e)
    {
      CLogger::error(CLogger::sanitize(e.what()));
    }

  pWork->commit();
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
  init();

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

  const CTable & Table = CSchema::INSTANCE.getTable(table);

  if (!Table.isValid())
    return false;

  const CField & ResultField = Table.getField(resultField);

  if (!ResultField.isValid())
    return false;

  const CField & ConstraintField = Table.getField(constraintField);

  if (!ConstraintField.isValid())
    return false;

  if (constraints.size() > 0 && constraints.getType() != ConstraintField.getType())
    return false;

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

  Query << ")";

  if (local)
    {
      Query << " AND " << LocalConstraint;
    }

  Query << " ORDER BY " << resultField;
  // std::cout << Query.str() << std::endl;

  pqxx::read_transaction * pWork = CConnection::work();

  if (pWork == NULL)
    return false;

  try
    {
      size_t Offset = 0;
      while (toFieldValueList(ResultField, pWork->exec(Query.str() + limit(Offset)), result) == Limit
             && Limit != 0)
        continue;
    }

  catch (const std::exception & e)
    {
      CLogger::error(CLogger::sanitize(e.what()));
    }

  pWork->commit();
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

  const CTable & Table = CSchema::INSTANCE.getTable(table);

  if (!Table.isValid())
    return false;

  const CField & ResultField = Table.getField(resultField);

  if (!ResultField.isValid())
    return false;

  const CField & ConstraintField = Table.getField(constraintField);

  if (!ConstraintField.isValid())
    return false;

  if (constraint.getType() != ConstraintField.getType())
    return false;

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

  Query << " ORDER BY " << resultField;
  // std::cout << Query.str() << std::endl;

  pqxx::read_transaction * pWork = CConnection::work();

  if (pWork == NULL)
    return false;

  try
    {
      size_t Offset = 0;
      while (toFieldValueList(ResultField, pWork->exec(Query.str() + limit(Offset)), result) == Limit
             && Limit != 0)
        continue;
    }

  catch (const std::exception & e)
    {
      CLogger::error(CLogger::sanitize(e.what()));
    }

  pWork->commit();
  delete pWork;

  return true;
}
