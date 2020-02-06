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

#include <fstream>
#include <jansson.h>

#include "db/CSchema.h"

#include "utilities/CSimConfig.h"
#include "db/CTable.h"
#include "utilities/CLogger.h"
// static
void CSchema::load(const std::vector< std::string > & schemaFiles)
{
  std::vector< std::string >::const_iterator it = schemaFiles.begin();
  std::vector< std::string >::const_iterator end = schemaFiles.end();

  for (; it != end; ++it)
    {
      json_t * pRoot = CSimConfig::loadJson(*it, JSON_DECODE_INT_AS_REAL);

      if (pRoot != NULL)
        {
          INSTANCE.fromJSON(pRoot);
        }

      json_decref(pRoot);
    }
}

CSchema::CSchema()
  : mId()
  , mLabel()
  , mTables()
  , mValid(true)
{}

CSchema::CSchema(const CSchema & src)
  : mId(src.mId)
  , mLabel(src.mLabel)
  , mTables(src.mTables)
  , mValid(src.mValid)
{}

// virtual
CSchema::~CSchema()
{}

void CSchema::fromJSON(const json_t * json)
{
  if (json_is_array(json))
    {
      for (size_t i = 0, imax = json_array_size(json); i < imax; ++i)
        if (!addTable(json_array_get(json, i)))
          {
            mValid = false; // DONE
            CLogger::error() << "Schema: Invalid table for item '" << i << "'.";
            return;
          }
    }
  else if (json_is_object(json))
    if (!addTable(json))
      {
        mValid = false; // DONE
        CLogger::error("Schema: Invalid 'table'.");
        return;
      }
}

bool CSchema::addTable(const json_t * json)
{
  CTable Table;
  Table.fromJSON(json);

  if (!Table.isValid())
    {
      return false;
    }

  CTable * pTable = &mTables.insert(std::make_pair(Table.getId(), Table)).first->second;

  std::map< std::string, CField >::const_iterator it = pTable->getFields().begin();
  std::map< std::string, CField >::const_iterator end = pTable->getFields().end();

  for (; it != end; ++it)
    {
      mFieldToTable[it->first] = pTable->getId();
    }

  return true;
}

const std::string & CSchema::getId() const
{
  return mId;
}

const bool & CSchema::isValid() const
{
  return mValid;
}

const CTable & CSchema::getTable(const std::string & table) const
{
  static CTable InvalidTable;

  std::map< std::string, CTable >::const_iterator found = mTables.find(table);

  if (found != mTables.end())
    return found->second;

  return InvalidTable;
}

const std::string & CSchema::getTableForField(const std::string & field) const
{
  static std::string InvalidTable;

  std::map< std::string, std::string >::const_iterator found = mFieldToTable.find(field);

  if (found != mFieldToTable.end())
    return found->second;

  return InvalidTable;
}
