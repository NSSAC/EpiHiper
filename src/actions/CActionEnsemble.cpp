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

#include <jansson.h>

#include "utilities/CLogger.h"
#include "utilities/CSimConfig.h"
#include "actions/CActionEnsemble.h"
#include "sets/CSetContent.h"

CActionEnsemble::CActionEnsemble()
  : mOnce()
  , mForEach()
  , mSampling()
  , mValid(false)
{}

CActionEnsemble::CActionEnsemble(const CActionEnsemble & src)
  : mOnce(src.mOnce)
  , mForEach(src.mForEach)
  , mSampling(src.mSampling)
  , mValid(src.mValid)
{
  std::vector< CActionDefinition * >::const_iterator it = src.mOnce.begin();
  std::vector< CActionDefinition * >::const_iterator end = src.mOnce.end();

  for (; it != end; ++it)
    {
      mOnce.push_back(new CActionDefinition(**it));
    }

  it = src.mForEach.begin();
  end = src.mForEach.end();

  for (; it != end; ++it)
    {
      mForEach.push_back(new CActionDefinition(**it));
    }
}

CActionEnsemble::CActionEnsemble(const json_t * json)
  : mOnce()
  , mForEach()
  , mSampling()
  , mValid(false)
{
  fromJSON(json);
}

// virtual
CActionEnsemble::~CActionEnsemble()
{
  std::vector< CActionDefinition * >::iterator it = mOnce.begin();
  std::vector< CActionDefinition * >::iterator end = mOnce.end();

  for (; it != end; ++it)
    {
      delete *it;
    }

  it = mForEach.begin();
  end = mForEach.end();

  for (; it != end; ++it)
    {
      delete *it;
    }
}

void CActionEnsemble::fromJSON(const json_t * json)
{
  /*
    "actionEnsemble": {
      "type": "object",
      "minProperties": 1,
      "properties": {
        "once": {
          "description": "A list of actions to be executed once.",
          "type": "array",
          "minItems": 1,
          "items": {"$ref": "#/definitions/action"}
        },
        "foreach": {
          "description": "A list of actions to be executed for each element in the filter.",
          "type": "array",
          "minItems": 1,
          "items": {"$ref": "#/definitions/action"}
        },
        "sampling": {"$ref": "#/definitions/sampling"}
      }
    },
  */

  mValid = true;

  json_t * pValue = json_object_get(json, "once");

  for (size_t i = 0, imax = json_array_size(pValue); i < imax; ++i)
    {
      CActionDefinition * pActionDefinition = new CActionDefinition(json_array_get(pValue, i));

      if (pActionDefinition->isValid())
        {
          mOnce.push_back(pActionDefinition);
        }
      else
        {
          CLogger::error("Action ensemble: Invalid action in object 'once'.");
          mValid = false; // DONE
          delete pActionDefinition;
        }
    }

  pValue = json_object_get(json, "foreach");

  for (size_t i = 0, imax = json_array_size(pValue); i < imax; ++i)
    {
      CActionDefinition * pActionDefinition = new CActionDefinition(json_array_get(pValue, i));

      if (pActionDefinition->isValid())
        {
          mForEach.push_back(pActionDefinition);
        }
      else
        {
          CLogger::error("Action ensemble: Invalid action in object 'foreach'.");
          mValid = false; // DONE

          delete pActionDefinition;
        }
    }

  mSampling.fromJSON(json_object_get(json, "sampling"));

  if (!mSampling.isValid())
    {
      CLogger::error("Action ensemble: Invalid value for 'sampling': {}", CSimConfig::jsonToString(json));
      return;
    }

  mValid = true;
}

const bool & CActionEnsemble::isValid() const
{
  return mValid;
}

bool CActionEnsemble::process(const CSetContent & targets)
{
  CLogger::info("CActionEnsemble: Target set contains '{}' items.", targets.size());
  
  std::vector< CActionDefinition * >::const_iterator it = mOnce.begin();
  std::vector< CActionDefinition * >::const_iterator end = mOnce.end();

  if (!mOnce.empty())
    CLogger::info("CActionEnsemble: Process '{}' action definitions in 'once'.", mOnce.size());

  for (; it != end; ++it)
    (*it)->process();

  if (!mForEach.empty())
    {
      CLogger::info("CActionEnsemble: Process '{}' action definitions in 'forEach'.", mForEach.size());
      end = mForEach.end();

      std::vector< CEdge * >::const_iterator itEdges = targets.beginEdges();
      std::vector< CEdge * >::const_iterator endEdges = targets.endEdges();

      for (; itEdges != endEdges; ++itEdges)
        for (it = mForEach.begin(); it != end; ++it)
          (*it)->process(*itEdges);

      std::vector< CNode * >::const_iterator itNodes = targets.beginNodes();
      std::vector< CNode * >::const_iterator endNodes = targets.endNodes();

      for (; itNodes != endNodes; ++itNodes)
        for (it = mForEach.begin(); it != end; ++it)
          (*it)->process(*itNodes);
    }
    
  if (!mSampling.isEmpty())
    CLogger::info("CActionEnsemble: Process 'sampling'.");
    
  mSampling.process(targets);

  return !CLogger::hasErrors();
}
