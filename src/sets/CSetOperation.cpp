// BEGIN: Copyright 
// Copyright (C) 2019 - 2021 Rector and Visitors of the University of Virginia 
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
#include "utilities/CLogger.h"

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
    mSets.insert((*it)->copy());

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
CSetContent * CSetOperation::copy() const
{
  return new CSetOperation(*this);
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

  mValid = false; // DONE
  json_t * pValue = json_object_get(json, "operation");

  if (!json_is_string(pValue))
    {
      CLogger::error("Set operation: Invalid or missing value for 'operation'.");
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
      CLogger::error("Set operation: Invalid value for 'operation'.");
      return;
    }

  pValue = json_object_get(json, "sets");

  if (!json_is_array(pValue))
    {
      CLogger::error("Set operation: Invalid or missing value for 'sets'.");
      return;
    }

  for (size_t i = 0, imax = json_array_size(pValue); i < imax; ++i)
    {
      CSetContent * pSetContent = CSetContent::create(json_array_get(pValue, i));

      if (pSetContent != NULL
          && pSetContent->isValid())
        {
          mSets.insert(pSetContent);
          mPrerequisites.insert(pSetContent);
        }
      else
        {
          if (pSetContent != NULL)
            {
              delete pSetContent;
            }

          CLogger::error() << "Set operation: Invalid value for item '" << i << "'.";
          return;
        }
    }

  determineIsStatic();
  mValid = true;
}

// virtual
bool CSetOperation::computeProtected()
{
  if (mValid
      && mpCompute != NULL)
    return (this->*mpCompute)();

  return false;
}

bool CSetOperation::computeUnion()
{
  std::vector< CNode * > & Nodes = getNodes();
  std::vector< CEdge * > & Edges = getEdges();
  CDBFieldValues & DBFieldValues = getDBFieldValues();

  Edges.clear();
  Nodes.clear();
  DBFieldValues.clear();

  bool first = true;

  std::vector< CNode * > TmpNodes;
  std::vector< CEdge * > TmpEdges;

  std::vector< CNode * > * pInNodes = &TmpNodes;
  std::vector< CEdge * > * pInEdges = &TmpEdges;
  std::vector< CNode * > * pOutNodes = &Nodes;
  std::vector< CEdge * > * pOutEdges = &Edges;

  std::set< CSetContent * >::const_iterator it = mSets.begin();
  std::set< CSetContent * >::const_iterator end = mSets.end();

  for (; it != end; ++it)
    {
      if (first)
        {
          *pInEdges = (*it)->getEdges();
          *pInNodes = (*it)->getNodes();
          DBFieldValues = (*it)->getDBFieldValues();

          first = false;
        }
      else
        {
          pOutEdges->clear();
          std::set_union((*it)->beginEdges(), (*it)->endEdges(), pInEdges->begin(), pInEdges->end(), std::back_inserter(*pOutEdges));
          std::swap(pInEdges, pOutEdges);

          pOutNodes->clear();
          std::set_union((*it)->beginNodes(), (*it)->endNodes(), pInNodes->begin(), pInNodes->end(), std::back_inserter(*pOutNodes));
          std::swap(pInNodes, pOutNodes);

          CDBFieldValues::const_iterator itDB = (*it)->getDBFieldValues().begin();
          CDBFieldValues::const_iterator endDB = (*it)->getDBFieldValues().end();

          for (; itDB != endDB; ++itDB)
            DBFieldValues[itDB->first].insert(itDB->second.begin(), itDB->second.end());
        }
    }

  if (pInEdges != &Edges)
    {
      Edges = *pInEdges;
    }

  if (pInNodes != &Nodes)
    {
      Nodes = *pInNodes;
    }

  CLogger::debug Debug;
  Debug << "CSetOperation: computeUnion (";
  std::string Separator;

  for (it = mSets.begin(); it != end; ++it)
    {
      Debug << Separator << (*it)->size();

      if (Separator.empty())
        Separator = ", ";
    }

  Debug << ") returned '" << size() << "'.";

  return true;
}

bool CSetOperation::computeIntersection()
{
  std::vector< CNode * > N1, N2, *pNin, *pNout;
  std::vector< CEdge * > E1, E2, *pEin, *pEout;
  CDBFieldValues DB1, DB2, *pDBin, *pDBout;

  pNin = &N1;
  pNout = &N2;
  pEin = &E1;
  pEout = &E2;
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

          CDBFieldValues::const_iterator itDB = (*it)->getDBFieldValues().begin();
          CDBFieldValues::const_iterator endDB = (*it)->getDBFieldValues().end();

          for (; itDB != endDB; ++itDB)
            {
              CDBFieldValues::iterator found = pDBin->find(itDB->first);

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

  getEdges() = *pEin;
  getNodes() = *pNin;
  getDBFieldValues() = *pDBin;

  CLogger::debug Debug;
  Debug << "CSetOperation: computeIntersection (";
  std::string Separator;

  for (it = mSets.begin(); it != end; ++it)
    {
      Debug << Separator << (*it)->size();

      if (Separator.empty())
        Separator = ", ";
    }

  Debug << ") returned '" << size() << "'.";

  return true;
}
