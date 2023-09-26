// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2023 Rector and Visitors of the University of Virginia 
//  
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal 
// in the Software without restriction, including without limitation the rights 
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
// copies of the Software, and to permit persons to whom the Software is 
// furnished to do so, subject to the following conditions: 
//  
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software. 
//  
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
// SOFTWARE 
// END: Copyright 

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
  size_t Default(std::numeric_limits< size_t >::max());
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

size_t toIntegerFieldValueList(const pqxx::result & result, CFieldValueList & list)
{
  int Default(0);
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

    case CFieldValueList::Type::integer:
      return toIntegerFieldValueList(result, list);
      break;

    case CFieldValueList::Type::boolean:
    case CFieldValueList::Type::traitData:
    case CFieldValueList::Type::traitValue:
    case CFieldValueList::Type::__SIZE:
      break;
    }

  return 0;
}

// static
CContext< std::string > CQuery::LocalConstraint = CContext< std::string >();

// static
size_t CQuery::Limit = 100000;

// static
void CQuery::init()
{
  if (LocalConstraint.size() == 0)
#pragma omp single
    {
      LocalConstraint.init();
      Limit = CSimConfig::getDBConnection().maxRecords;
    }


  std::string & Active = LocalConstraint.Active();

  if (Active.empty())
    {
      std::ostringstream Query;

      CNode * pBegin = CNetwork::Context.Active().beginNode();
      CNode * pEnd = CNetwork::Context.Active().endNode() - 1;
      Query << "pid BETWEEN " << pBegin->id << " AND " << pEnd->id;

      Active = Query.str();
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
      Query << " WHERE " << LocalConstraint.Active();
    }

  Query << " ORDER BY " << resultField;
  // std::cout << Query.str() << std::endl;

  bool success = true;

#pragma omp critical (sql_query)
  {
    pqxx::read_transaction * pWork = CConnection::work();

    if (pWork == NULL)
      success = false;
    else
      {
        try
          {
            size_t Offset = 0;
            while (toFieldValueList(ResultField, pWork->exec(Query.str() + limit(Offset)), result) == Limit
                   && Limit != 0)
              continue;
          }

        catch (const std::exception & e)
          {
            CLogger::error("CQuery::All: {}", CLogger::sanitize(e.what()));
            success = false;
          }

        pWork->commit();
        delete pWork;
      }

    CLogger::debug("CQuery::All: {} returned '{}' rows.", Query.str(), result.size());
  }

  return success;
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
    {
      CLogger::error("CQuery::in: Invalid table '{}'.", table); 
      return false;
    }

  const CField & ResultField = Table.getField(resultField);

  if (!ResultField.isValid())
    {
      CLogger::error("CQuery::in: Invalid result field '{}'.", resultField); 
      return false;
    }

  const CField & ConstraintField = Table.getField(constraintField);

  if (!ConstraintField.isValid())
    {
      CLogger::error("CQuery::in: Invalid result field '{}'.", constraintField); 
      return false;
    }

  if (constraints.size() > 0 && constraints.getType() != ConstraintField.getType())
    {
      CLogger::error("CQuery::in: type mismatch '{}' != '{}'.", (int) ConstraintField.getType(), (int) constraints.getType()); 
      return false;
    }

  if (constraints.getType() == CFieldValueList::Type::string)
    {
      CFieldValueList::const_iterator it = constraints.begin();
      CFieldValueList::const_iterator end = constraints.end();

      for (; it != end; ++it)
        if (!ConstraintField.isValidValue(it->toString()))
          {
            CLogger::error("CQuery::in: invalid enum value for '{}': '{}'.", ConstraintField.getId(), it->toString());
            return false;
          }
    }

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

        case CFieldValueList::Type::integer:
          Query << it->toInteger();
          break;

        case CFieldValueList::Type::boolean:
        case CFieldValueList::Type::traitData:
        case CFieldValueList::Type::traitValue:
        case CFieldValueList::Type::__SIZE:
          break;
        }
    }

  Query << ")";

  if (local)
    {
      Query << " AND " << LocalConstraint.Active();
    }

  Query << " ORDER BY " << resultField;
  // std::cout << Query.str() << std::endl;

  bool success = true;

#pragma omp critical (sql_query)
  {
    pqxx::read_transaction * pWork = CConnection::work();

    if (pWork == NULL)
      success = false;
    else
      {
        try
          {
            size_t Offset = 0;
            while (toFieldValueList(ResultField, pWork->exec(Query.str() + limit(Offset)), result) == Limit
                   && Limit != 0)
              continue;
          }

        catch (const std::exception & e)
          {
            CLogger::error("CQuery::{}: {}", in ?  "in" : "notIn", CLogger::sanitize(e.what()));
            success = false;
          }

        pWork->commit();
        delete pWork;

        CLogger::debug("CQuery::{}: {} returned '{}' rows.", in ?  "in" : "notIn", Query.str(), result.size());
      }
  }

  return success;
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
                   const CValueInterface & constraint,
                   const std::string & cmp)
{
  init();

  const CTable & Table = CSchema::INSTANCE.getTable(table);

  if (!Table.isValid())
    {
      CLogger::error("CQuery::where: Invalid table '{}}'.", table); 
      return false;
    }

  const CField & ResultField = Table.getField(resultField);

  if (!ResultField.isValid())
    {
      CLogger::error("CQuery::where: Invalid result field '{}'.", resultField); 
      return false;
    }

  const CField & ConstraintField = Table.getField(constraintField);

  if (!ConstraintField.isValid())
    {
      CLogger::error("CQuery::where: Invalid result field '{}'.", constraintField); 
      return false;
    }

  if (constraint.getType() != ConstraintField.getType())
    {
      CLogger::error("CQuery::where: type mismatch '{}' != '{}'.", (int) ConstraintField.getType(), (int) constraint.getType()); 
      return false;
    }

  if (constraint.getType() == CFieldValue::Type::string
      && !ConstraintField.isValidValue(constraint.toString()))
    {
      CLogger::error("CQuery::where: invalid enum value for '{}': '{}'.", ConstraintField.getId(), constraint.toString());
      return false;
    }

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

    case CFieldValueList::Type::integer:
      Query << constraint.toInteger();
      break;

    case CFieldValueList::Type::boolean:
    case CFieldValueList::Type::traitData:
    case CFieldValueList::Type::traitValue:
    case CFieldValueList::Type::__SIZE:
      break;
    }

  if (local)
    {
      Query << " AND " << LocalConstraint.Active();
    }

  Query << " ORDER BY " << resultField;
  // std::cout << Query.str() << std::endl;

  bool success = true;

#pragma omp critical (sql_query)
  {
    pqxx::read_transaction * pWork = CConnection::work();

    if (pWork == NULL)
      success = false;
    else
      {
        try
          {
            size_t Offset = 0;
            while (toFieldValueList(ResultField, pWork->exec(Query.str() + limit(Offset)), result) == Limit
                   && Limit != 0)
              continue;
          }

        catch (const std::exception & e)
          {
            CLogger::error("CQuery::where: {}", CLogger::sanitize(e.what()));
            success = false;
          }

        if (success)
          pWork->commit();

        delete pWork;

        CLogger::debug("CQuery::where: {} returned '{}' rows.", Query.str(), result.size());
      }
  }

  return success;
}
