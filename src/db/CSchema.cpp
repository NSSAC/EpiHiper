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
      json_t * pRoot = CSimConfig::loadJsonPreamble(*it, JSON_DECODE_INT_AS_REAL);

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
