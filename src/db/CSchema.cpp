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

#include <fstream>
#include <jansson.h>

#include "db/CSchema.h"
#include "db/CTable.h"
#include "SimConfig.h"

// static
CSchema CSchema::INSTANCE;

// static
void CSchema::load(const std::vector< std::string > & schemaFiles)
{
  std::vector< std::string >::const_iterator it = schemaFiles.begin();
  std::vector< std::string >::const_iterator end = schemaFiles.end();

  for (; it != end; ++it)
    {
	  std::ifstream is(it->c_str());

	  if (is.fail())
	    {
	      std::cerr << "Network file: '" << *it << "' cannot be opened." << std::endl;
	      return;
	    }

	  std::string Line;
	  std::getline(is, Line);

	  json_error_t error;

	  json_t * pRoot = json_loads(Line.c_str(), 0, &error);

	  if (pRoot == NULL)
	    {
	      std::cerr << "Network file: '" << *it << "' error on line " << error.line << ": " << error.text << std::endl;;
	    }

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
        {
          CTable Table;
          Table.fromJSON(json_array_get(json, i));
          mValid &= Table.isValid();

          mTables.insert(std::make_pair(Table.getId(), Table));
        }

      return;
    }

  if (json_is_object(json))
    {
      CTable Table;
      Table.fromJSON(json);
      mValid &= Table.isValid();

      mTables.insert(std::make_pair(Table.getId(), Table));
    }
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
