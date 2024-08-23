// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2024 Rector and Visitors of the University of Virginia 
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
#include <cstring>
#include <algorithm>
#include <jansson.h>

#include "sets/CSetOperation.h"
#include "utilities/CLogger.h"
#include "utilities/CSimConfig.h"
#include "network/CNetwork.h"

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
{}

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
      CLogger::error("Set operation: Invalid or missing value for 'operation'. {}", CSimConfig::jsonToString(json));
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
      CLogger::error("Set operation: Invalid value for 'operation'. {}", CSimConfig::jsonToString(json));
      return;
    }

  pValue = json_object_get(json, "sets");

  if (!json_is_array(pValue))
    {
      CLogger::error("Set operation: Invalid or missing value for 'sets'. {}", CSimConfig::jsonToString(json));
      return;
    }

  for (size_t i = 0, imax = json_array_size(pValue); i < imax; ++i)
    {
      shared_pointer pSetContent = CSetContent::create(json_array_get(pValue, i));

      if (pSetContent
          && pSetContent->isValid())
        {
          mSets.insert(pSetContent.get());
          mPrerequisites.insert(pSetContent.get()); // DONE
        }
      else
        {
          pSetContent.reset();

          CLogger::error("Set operation: Invalid value for item '{}'. {}", i, CSimConfig::jsonToString(json));
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

  return pointerLessThan(mpCompute, pRhs->mpCompute);
}

// virtual
bool CSetOperation::computeSetContent()
{
  if (mValid
      && mpCompute != NULL)
    return (this->*mpCompute)();

  return false;
}

// virtual
void CSetOperation::setScopeProtected()
{
  std::set< CSetContent * >::const_iterator it = mSets.begin();
  std::set< CSetContent * >::const_iterator end = mSets.end();

  for (; it != end; ++it)
    (*it)->setScope(Scope::global);
}

bool CSetOperation::computeUnion()
{
  SetContent & Active = activeContent();
  CDBFieldValues & DBFieldValues = Active.dBFieldValues;

  SetContent C1, C2, *pIn, *pOut;
  pIn = &C1;
  pOut = &C2;

  bool first = true;

  std::set< CSetContent * >::const_iterator it = mSets.begin();
  std::set< CSetContent * >::const_iterator end = mSets.end();

  for (; it != end; ++it)
    {
      SetContent & Active = (*it)->activeContent();

      if (first)
        {
          *pOut = Active;
          DBFieldValues = Active.dBFieldValues;

          first = false;
        }
      else
        {
          pOut->clear();

          std::set_union(Active.edges.begin(), Active.edges.end(), pIn->edges.begin(), pIn->edges.end(), std::back_inserter(pOut->edges));
          std::set_union(Active.mNodes.begin(), Active.mNodes.end(), pIn->mNodes.begin(), pIn->mNodes.end(), std::back_inserter(pOut->mNodes));

          CDBFieldValues::const_iterator itDB = Active.dBFieldValues.begin();
          CDBFieldValues::const_iterator endDB = Active.dBFieldValues.end();

          for (; itDB != endDB; ++itDB)
            DBFieldValues[itDB->first].insert(itDB->second.begin(), itDB->second.end());
        }
      
      std::swap(pIn, pOut);
    }

  Active.edges = pIn->edges;
  Active.mNodes = pIn->mNodes;

  if (CLogger::level() <= CLogger::LogLevel::debug)
    {
      std::string Separator;
      std::ostringstream Message;
      Message << "CSetOperation: computeUnion (";

      for (it = mSets.begin(); it != end; ++it)
        {
          Message << Separator << (*it)->getComputableId() << ": " << (*it)->size();

          if (Separator.empty())
            Separator = ", ";
        }

      Message << ") returned '" << size() << "'.";
      CLogger::debug(Message.str());
    }

  return true;
}

bool CSetOperation::computeIntersection()
{
  SetContent C1, C2, *pIn, *pOut;
  pIn = &C1;
  pOut = &C2;

  bool first = true;


  std::set< CSetContent * >::const_iterator it = mSets.begin();
  std::set< CSetContent * >::const_iterator end = mSets.end();

  for (; it != end; ++it)
    {
      SetContent & Active = (*it)->activeContent();

      if (first)
        {
          *pOut = Active;
          first = false;
        }
      else
        {
          pOut->clear();

          std::set_intersection(pIn->edges.begin(), pIn->edges.end(), Active.edges.begin(), Active.edges.end(), std::inserter(pOut->edges, pOut->edges.begin()));
          std::set_intersection(pIn->mNodes.begin(), pIn->mNodes.end(), Active.mNodes.begin(), Active.mNodes.end(), std::inserter(pOut->mNodes, pOut->mNodes.begin()));

          CDBFieldValues::const_iterator itDB = Active.dBFieldValues.begin();
          CDBFieldValues::const_iterator endDB = Active.dBFieldValues.end();

          for (; itDB != endDB; ++itDB)
            {
              CDBFieldValues::iterator In = pIn->dBFieldValues.find(itDB->first);

              if (In != pIn->dBFieldValues.end())
                {
                  CValueList & Out = pOut->dBFieldValues[itDB->first];
                  std::set_intersection(In->second.begin(), In->second.end(), itDB->second.begin(), itDB->second.end(), std::inserter(Out, Out.begin()));
                }
            }
        }

      std::swap(pIn, pOut);
    }

  SetContent & Active = activeContent();
  Active = *pIn;

  if (CLogger::level() <= CLogger::LogLevel::debug)
    {
      std::string Separator;
      std::ostringstream Message;
      Message << "CSetOperation: computeIntersection (";

      for (it = mSets.begin(); it != end; ++it)
        {
          Message << Separator << (*it)->getComputableId() << ": " << (*it)->size();

          if (Separator.empty())
            Separator = ", ";
        }

      Message << ") returned '" << size() << "'.";
      CLogger::debug(Message.str());
    }

  return true;
}
