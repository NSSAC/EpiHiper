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

#include <cstring>
#include <algorithm>
#include <jansson.h>

#include "sets/CSetOperation.h"
#include "utilities/CLogger.h"
#include "utilities/CSimConfig.h"

CSetOperation::CSetOperation()
  : CSetContent(CSetContent::Type::operation)
  , mpCompute(NULL)
  , mSets()
{}

CSetOperation::CSetOperation(const CSetOperation & src)
  : CSetContent(src)
  , mpCompute(src.mpCompute)
  , mSets(src.mSets)
{}

CSetOperation::CSetOperation(const json_t * json)
  : CSetContent(CSetContent::Type::operation)
  , mpCompute(NULL)
  , mSets()
{
  fromJSON(json);
}

CSetOperation::~CSetOperation()
{
  mSets.clear();
}

// virtual
void CSetOperation::fromJSONProtected(const json_t * json)
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
      CLogger::error() << "Set operation: Invalid or missing value for 'operation'. " << CSimConfig::jsonToString(json);
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
      CLogger::error() << "Set operation: Invalid value for 'operation'. " << CSimConfig::jsonToString(json);
      return;
    }

  pValue = json_object_get(json, "sets");

  if (!json_is_array(pValue))
    {
      CLogger::error() << "Set operation: Invalid or missing value for 'sets'. " << CSimConfig::jsonToString(json);
      return;
    }

  for (size_t i = 0, imax = json_array_size(pValue); i < imax; ++i)
    {
      CSetContentPtr pSetContent = CSetContent::create(json_array_get(pValue, i));

      if (pSetContent
          && pSetContent->isValid())
        {
          mSets.insert(pSetContent);
          mPrerequisites.insert(pSetContent.get()); // DONE
        }
      else
        {
          pSetContent.reset();

          CLogger::error() << "Set operation: Invalid value for item '" << i << "'. " << CSimConfig::jsonToString(json);
          return;
        }
    }

  mValid = true;
}

// virtual 
bool CSetOperation::lessThanProtected(const CSetContent & rhs) const
{
  const CSetOperation * pRhs = static_cast< const CSetOperation * >(&rhs);

  if (mSets != pRhs->mSets)
    return mSets < pRhs->mSets;

  return reinterpret_cast< const void * >(mpCompute) < reinterpret_cast< const void * >(pRhs->mpCompute);
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

  std::set< CSetContent::CSetContentPtr >::const_iterator it = mSets.begin();
  std::set< CSetContent::CSetContentPtr >::const_iterator end = mSets.end();

  for (; it != end; ++it)
    {
      if (first)
        {
          *pInEdges = it->get()->getEdges();
          *pInNodes = it->get()->getNodes();
          DBFieldValues = it->get()->getDBFieldValues();

          first = false;
        }
      else
        {
          pOutEdges->clear();
          std::set_union(it->get()->beginEdges(), it->get()->endEdges(), pInEdges->begin(), pInEdges->end(), std::back_inserter(*pOutEdges));
          std::swap(pInEdges, pOutEdges);

          pOutNodes->clear();
          std::set_union(it->get()->beginNodes(), it->get()->endNodes(), pInNodes->begin(), pInNodes->end(), std::back_inserter(*pOutNodes));
          std::swap(pInNodes, pOutNodes);

          CDBFieldValues::const_iterator itDB = it->get()->getDBFieldValues().begin();
          CDBFieldValues::const_iterator endDB = it->get()->getDBFieldValues().end();

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
      Debug << Separator << it->get()->getComputableId() << ": " << it->get()->size();

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

  std::set< CSetContent::CSetContentPtr >::const_iterator it = mSets.begin();
  std::set< CSetContent::CSetContentPtr >::const_iterator end = mSets.end();

  for (; it != end; ++it)
    {
      if (first)
        {
          *pEout = it->get()->getEdges();
          *pNout = it->get()->getNodes();
          *pDBout = it->get()->getDBFieldValues();

          first = false;
        }
      else
        {
          pEout->clear();
          pNout->clear();
          pDBout->clear();

          std::set_intersection(pEin->begin(), pEin->end(), it->get()->beginEdges(), it->get()->endEdges(), std::inserter(*pEout, pEout->begin()));
          std::set_intersection(pNin->begin(), pNin->end(), it->get()->beginNodes(), it->get()->endNodes(), std::inserter(*pNout, pNout->begin()));

          CDBFieldValues::const_iterator itDB = it->get()->getDBFieldValues().begin();
          CDBFieldValues::const_iterator endDB = it->get()->getDBFieldValues().end();

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
      Debug << Separator << it->get()->getComputableId() << ": " << it->get()->size();

      if (Separator.empty())
        Separator = ", ";
    }

  Debug << ") returned '" << size() << "'.";

  return true;
}
