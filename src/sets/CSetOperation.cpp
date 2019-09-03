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

#include <cstring>
#include <algorithm>
#include <jansson.h>

#include "sets/CSetOperation.h"

CSetOperation::CSetOperation()
  : CSetContent()
  , mpCompute(NULL)
  , mSets()
{}

CSetOperation::CSetOperation(const CSetOperation & src)
  : CSetContent(src)
  , mpCompute(src.mpCompute)
  , mSets()
{
  std::set< CSetContent * >::const_iterator it = src.mSets.begin();
  std::set< CSetContent * >::const_iterator end = src.mSets.end();

  for (; it != end; ++it)
    mSets.insert(CSetContent::copy(*it));

  mSets.erase(NULL);
}

CSetOperation::CSetOperation(const json_t * json)
  : CSetContent()
  , mpCompute(NULL)
  , mSets()
{
  fromJSON(json);
}


CSetOperation::~CSetOperation()
{
  std::set< CSetContent * >::iterator it = mSets.begin();
  std::set< CSetContent * >::iterator end = mSets.end();

  for (; it != end; ++it)
    {
      CSetContent * pSetContent = *it;
      CSetContent::destroy(pSetContent);
    }

  mSets.clear();
}

// virtual
void CSetOperation::fromJSON(const json_t * json)
{
  /*
    "required": [
      "operation",
      "sets"
    ],
    "properties": {
      "operation": {
        "type": "string",
        "enum": [
          "union",
          "intersection"
        ]
      },
      "sets": {
        "type": "array",
        "items": {"$ref": "#/definitions/setContent"}
      }
    },
  */

  json_t * pValue = json_object_get(json, "operation");

  if (!json_is_string(pValue))
    {
      mValid = false;
      return;
    }

  if (strcmp(json_string_value(pValue), "union") == 0)
    {
      mpCompute = &CSetOperation::computeUnion;
    }
  else if (strcmp(json_string_value(pValue), "intersection") == 0)
    {
      mpCompute = &CSetOperation::computeIntersection;
    }
  else
    {
      mValid = false;
      return;
    }

  pValue = json_object_get(json, "sets");

  if (!json_is_array(pValue))
    {
      mValid = false;
      return;
    }

  mValid = true;

  for (size_t i = 0, imax = json_array_size(pValue); i < imax; ++i)
    {
      CSetContent * pSetContent = CSetContent::create(json_array_get(pValue, i));

      if (pSetContent != NULL && pSetContent->isValid())
        {
          mSets.insert(pSetContent);
          mPrerequisites.insert(pSetContent);
        }
      else
        mValid = false;
    }
}

// virtual
void CSetOperation::compute()
{
  if (mValid &&
      mpCompute != NULL)
    (this->*mpCompute)();
}

void CSetOperation::computeUnion()
{
  mEdges.clear();
  mNodes.clear();
  mDBFieldValues.clear();

  bool first = true;

  std::set< CSetContent * >::const_iterator it = mSets.begin();
  std::set< CSetContent * >::const_iterator end = mSets.end();

  for (; it != end; ++it)
    {
      if (first)
        {
          mEdges = (*it)->getEdges();
          mNodes = (*it)->getNodes();
          mDBFieldValues = (*it)->getDBFieldValues();

          first = false;
        }
      else
        {
          mEdges.insert((*it)->beginEdges(), (*it)->endEdges());
          mNodes.insert((*it)->beginNodes(), (*it)->endNodes());

          std::map< CValueList::Type, CValueList >::const_iterator itDB = (*it)->getDBFieldValues().begin();
          std::map< CValueList::Type, CValueList >::const_iterator endDB = (*it)->getDBFieldValues().end();

          for (; itDB != endDB; ++itDB)
            mDBFieldValues[itDB->first].insert(itDB->second.begin(), itDB->second.end());
        }
    }
}

void CSetOperation::computeIntersection()
{
  std::set< CNode * > N1, N2, * pNin, * pNout;;
  std::set< CEdge * > E1, E2,  *pEin, * pEout;
  std::map< CValueList::Type, CValueList > DB1, DB2, * pDBin, *pDBout;

  pNin = &N1;
  pNout = &N2;
  pEin = &E1;
  pEout = &E1;
  pDBin = &DB1;
  pDBout = &DB2;

  bool first = true;

  std::set< CSetContent * >::const_iterator it = mSets.begin();
  std::set< CSetContent * >::const_iterator end = mSets.end();

  for (; it != end; ++it)
    {
      if (first)
        {
          *pEout = (*it)->getEdges();
          *pNout = (*it)->getNodes();
          *pDBout = (*it)->getDBFieldValues();

          first = false;
        }
      else
        {
          pEout->clear();
          pNout->clear();
          pDBout->clear();

          std::set_intersection(pEin->begin(), pEin->end(), (*it)->beginEdges(), (*it)->endEdges(), std::inserter(*pEout, pEout->begin()));
          std::set_intersection(pNin->begin(), pNin->end(), (*it)->beginNodes(), (*it)->endNodes(), std::inserter(*pNout, pNout->begin()));

          std::map< CValueList::Type, CValueList >::const_iterator itDB = (*it)->getDBFieldValues().begin();
          std::map< CValueList::Type, CValueList >::const_iterator endDB = (*it)->getDBFieldValues().end();

          for (; itDB != endDB; ++itDB)
            {
              std::map< CValueList::Type, CValueList >::iterator found = pDBin->find(itDB->first);

              if (found != pDBin->end())
                {
                  CValueList & insert = (*pDBout)[itDB->first];
                  std::set_intersection(found->second.begin(), found->second.end(), itDB->second.begin(), itDB->second.end(), std::inserter(insert, insert.begin()));
                }
            }
        }

      std::swap(pEin, pEout);
      std::swap(pNin, pNout);
      std::swap(pDBin, pDBout);
    }

  mEdges = *pEin;
  mNodes = *pNin;
  mDBFieldValues = *pDBin;
}

