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

#include <iostream>
#include <jansson.h>

#include "variables/CVariableList.h"
#include "utilities/CLogger.h"

CVariableList::CVariableList()
  : base()
  , mId2Index()
  , mValid(true)
  , mChangedVariables()
  , mInvalidVariable()
{
  CComputable::Instances.erase(&mInvalidVariable);
  mChangedVariables.init();
}

CVariableList::CVariableList(const CVariableList & src)
  : base(src)
  , mId2Index(src.mId2Index)
  , mValid(src.mValid)
  , mChangedVariables(src.mChangedVariables)
  , mInvalidVariable(src.mInvalidVariable)
{
  CComputable::Instances.erase(&mInvalidVariable);
}

// virtual
CVariableList::~CVariableList()
{
  mChangedVariables.release();
}

void CVariableList::fromJSON(const json_t * json)
{
  if (json == NULL)
    return;

  mValid = false; // DONE

  if (!json_is_array(json))
    {
      CLogger::error("Variable list: Invalid.");
      return;
    }

  for (size_t i = 0, imax = json_array_size(json); i < imax; ++i)
    {
      CVariable *pVariable = new CVariable(json_array_get(json, i));

      if (!pVariable->isValid())
        {
          CLogger::error("Variable list: Invalid value for item '{}'.", i);
          delete pVariable;
          return;
        }

      if (!append(pVariable))
        {
          CLogger::error("Variable list: Duplicate variable ID '{}'.", pVariable->getId());
          delete pVariable;
          return;
        }
    }

  mValid = true;
}

void CVariableList::toBinary(std::ostream & os) const
{
  size_t Size = size();

  os.write(reinterpret_cast<const char *>(&Size), sizeof(size_t));

  const_iterator it = begin();
  const_iterator itEnd = end();

  for (; it != itEnd; ++it)
    {
      (*it)->toBinary(os);
    }
}

void CVariableList::fromBinary(std::istream & is)
{
  size_t Size;

  is.read(reinterpret_cast<char *>(&Size), sizeof(size_t));

  mValid &= (Size == size());

  if (!mValid) return;

  base::iterator it = base::begin();
  base::iterator itEnd = base::end();

  for (; it != itEnd; ++it)
    {
      (*it)->fromBinary(is);
    }
}

CVariable & CVariableList::operator[](const size_t & index)
{
  if (index < size())
    return *base::operator[](index);

  return mInvalidVariable;
}

CVariable & CVariableList::operator[](const std::string & id)
{
  std::map< std::string, size_t >::iterator found = mId2Index.find(id);

  if (found != mId2Index.end())
    return operator[](found->second);

  return mInvalidVariable;
}

CVariable & CVariableList::operator[](const json_t * json)
{
  /*
    "variableReference": {
      "$id": "#variableReference",
      "description": "A variable reference.",
      "type": "object",
      "required": ["variable"],
      "properties": {
        "variable": {
          "type": "object",
          "required": ["idRef"],
          "properties": {
            "idRef": {"$ref": "#/definitions/uniqueIdRef"}
          }
        }
      }
    },
  */

  json_t * pVariable = json_object_get(json, "variable");

  if (!json_is_object(pVariable))
    return mInvalidVariable;

  json_t * pIdRef = json_object_get(pVariable, "idRef");

  if (!json_is_string(pIdRef))
    return mInvalidVariable;

  return operator[](json_string_value(pIdRef));
}

void CVariableList::resetAll(const bool & force)
{
  CComputableSet & Active = INSTANCE.mChangedVariables.Active();

  if (INSTANCE.mChangedVariables.isThread(&Active))
    Active.clear();

  CCommunicate::barrierRMA();

#pragma omp single
  {
    INSTANCE.mChangedVariables.Master().clear();

    base::iterator it;
    base::iterator itEnd = base::end();

    for (it = base::begin(); it != itEnd; ++it)
      {
        if ((*it)->reset(force)
            || (*it)->getValue())
          INSTANCE.mChangedVariables.Master().insert(*it);
      }
  }
}

void CVariableList::synchronizeChangedVariables()
{
  CCommunicate::barrierRMA();

#pragma omp single
  {
    CComputableSet * pSet = INSTANCE.mChangedVariables.beginThread();

    // Consolidate local changes from all threads.
    if (INSTANCE.mChangedVariables.isThread(pSet))
      {
        CComputableSet::const_iterator it = pSet->begin();
        CComputableSet::const_iterator end = pSet->end();

        for (; it != end; ++it)
          {
            INSTANCE.mChangedVariables.Master().insert(it->second);
            const_cast< CVariable *>(static_cast< const CVariable * >(it->second))->updateMaster();
          }
      }

    // Consolidate changes to global variables
    base::iterator it = base::begin();
    base::iterator end = base::end();

    for (; it != end; ++it)
      {
        if ((*it)->getValue())
          {
            INSTANCE.mChangedVariables.Master().insert(*it);
            (*it)->updateMaster();
          }
      }
  }
  
}

bool CVariableList::append(CVariable * pVariable)
{
  if (mId2Index.find(pVariable->getId()) != mId2Index.end())
    return false;

  mId2Index[pVariable->getId()] = size();
  push_back(pVariable);

  return true;
}

CVariableList::const_iterator CVariableList::begin() const
{
  return base::begin();
}
  
CVariableList::const_iterator CVariableList::end() const
{
  return base::end();
}

CContext< CComputableSet > & CVariableList::changedVariables()
{
  return mChangedVariables;
}
