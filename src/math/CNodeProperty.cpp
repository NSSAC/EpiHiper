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

#include <cstring>
#include <jansson.h>

#include "math/CNodeProperty.h"
#include "math/CValue.h"
#include "network/CNode.h"
#include "network/CEdge.h"
#include "diseaseModel/CModel.h"
#include "actions/COperation.h"
#include "utilities/CLogger.h"

// static 
std::vector< std::set< std::shared_ptr< CSetCollectorInterface > > > CNodeProperty::Collectors((size_t) CNodeProperty::Property::__SIZE);

CNodeProperty::CNodeProperty()
  : CValueInterface(Type::boolean, NULL)
  , mProperty(Property::__SIZE)
  , mpPropertyOf(NULL)
  , mpExecute(NULL)
  , mValid(false)
{}

CNodeProperty::CNodeProperty(const CNodeProperty & src)
  : CValueInterface(src)
  , mProperty(src.mProperty)
  , mpPropertyOf(src.mpPropertyOf)
  , mpExecute(src.mpExecute)
  , mValid(src.mValid)
{}

CNodeProperty::CNodeProperty(const json_t * json)
  : CValueInterface(Type::boolean, NULL)
  , mProperty(Property::__SIZE)
  , mpPropertyOf(NULL)
  , mpExecute(NULL)
  , mValid(false)
{
  fromJSON(json);
}

CNodeProperty::~CNodeProperty()
{}

void CNodeProperty::fromJSON(const json_t * json)
{
  mValid = false; 
  json_t * pObject = json_object_get(json, "node");

  if (!json_is_object(pObject))
    {
      return;
    }

  json_t * pValue = json_object_get(pObject, "property");

  if (json_is_string(pValue))
    {
      const char * Property = json_string_value(pValue);

      if (strcmp(Property, "id") == 0)
        {
          mProperty = Property::id;
          mType = Type::id;
          mpPropertyOf = &CNodeProperty::id;
          mpExecute = &CNodeProperty::setId;
        }
      else if (strcmp(Property, "susceptibilityFactor") == 0)
        {
          mProperty = Property::susceptibilityFactor;
          mType = Type::number;
          mpPropertyOf = &CNodeProperty::susceptibilityFactor;
          mpExecute = &CNodeProperty::setSusceptibilityFactor;
        }
      else if (strcmp(Property, "infectivityFactor") == 0)
        {
          mProperty = Property::infectivityFactor;
          mType = Type::number;
          mpPropertyOf = &CNodeProperty::infectivityFactor;
          mpExecute = &CNodeProperty::setInfectivityFactor;
        }
      else if (strcmp(Property, "healthState") == 0)
        {
          mProperty = Property::healthState;
          mType = Type::id;
          mpPropertyOf = &CNodeProperty::healthState;
          mpExecute = &CNodeProperty::setHealthState;
        }
      else if (strcmp(Property, "nodeTrait") == 0)
        {
          mProperty = Property::nodeTrait;
          mType = Type::traitData;
          mpPropertyOf = &CNodeProperty::nodeTrait;
          mpExecute = &CNodeProperty::setNodeTrait;
        }
      else
        {
          CLogger::error("Node property: Invalid property '{}'", Property);
          return;
        }

      mValid = true;
    }
    
  return;
}

bool CNodeProperty::operator != (const CNodeProperty & rhs) const
{
  return mProperty != rhs.mProperty;
}

bool CNodeProperty::operator < (const CNodeProperty & rhs) const
{
  return mProperty < rhs.mProperty;
}

const bool & CNodeProperty::isValid() const
{
  return mValid;
}

const CNodeProperty::Property & CNodeProperty::getProperty() const
{
  return mProperty;
}

CValueInterface CNodeProperty::propertyOf(const CNode * pNode) const
{
  return (this->*mpPropertyOf)(const_cast< CNode * >(pNode));
}

bool CNodeProperty::execute(CNode * pNode, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info)
{
  return (this->*mpExecute)(pNode, value, pOperator, info);
}

bool CNodeProperty::isReadOnly() const
{
  return mpPropertyOf == &CNodeProperty::id;
}

// static
std::pair< CEdge *, CEdge * > CNodeProperty::edges(CNode * pNode)
{
  return std::make_pair(pNode->Edges, pNode->Edges + pNode->EdgesSize);
}

CValueInterface CNodeProperty::id(CNode * pNode) const
{
  return CValueInterface(pNode->id);
}

CValueInterface CNodeProperty::susceptibilityFactor(CNode * pNode) const
{
  return CValueInterface(pNode->susceptibilityFactor);
}

CValueInterface CNodeProperty::infectivityFactor(CNode * pNode) const
{
  return CValueInterface(pNode->infectivityFactor);
}

CValueInterface CNodeProperty::healthState(CNode * pNode) const
{
  return CValueInterface(pNode->healthState);
}

CValueInterface CNodeProperty::nodeTrait(CNode * pNode) const
{
  return CValueInterface(pNode->nodeTrait);
}

bool CNodeProperty::setId(CNode * pNode, const CValueInterface & /* value */, CValueInterface::pOperator /* pOperator */, const CMetadata & /* info */)
{
  CLogger::critical("Invalid operation 'setId' for node: '{}'", pNode->id);
  return false;
}

bool CNodeProperty::setSusceptibilityFactor(CNode * pNode, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info)
{
  return COperation::execute< CNode, double >(pNode, value.toNumber(), pOperator, &CNode::setSusceptibilityFactor, Collectors[(size_t) mProperty], info);
}

bool CNodeProperty::setInfectivityFactor(CNode * pNode, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info)
{
  return COperation::execute< CNode, double >(pNode, value.toNumber(), pOperator, &CNode::setInfectivityFactor, Collectors[(size_t) mProperty], info);
}

bool CNodeProperty::setHealthState(CNode * pNode, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info)
{
  CMetadata Info(info);
  Info.set("StateChange", true);

  return COperation::execute< CNode, size_t >(pNode, value.toId(), pOperator, &CNode::setHealthState, Collectors[(size_t) mProperty], Info);
}

bool CNodeProperty::setNodeTrait(CNode * pNode, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info)
{
  return COperation::execute< CNode, CTraitData::value >(pNode, value.toTraitValue(), pOperator, &CNode::setNodeTrait, Collectors[(size_t) mProperty], info);
}

void CNodeProperty::registerSetCollector(std::shared_ptr< CSetCollectorInterface > pCollector) const
{
  Collectors[(size_t) mProperty].insert(pCollector);
  CLogger::debug("CNodeProperty::registerSetCollector: property '{}'. size '{}'", (size_t) mProperty, Collectors[(size_t) mProperty].size());
}    

void CNodeProperty::deregisterSetCollector(std::shared_ptr< CSetCollectorInterface > pCollector) const
{
  if (!Collectors[(size_t) mProperty].empty())
    {
      Collectors[(size_t) mProperty].erase(pCollector);
      CLogger::debug("CNodeProperty::deregisterSetCollector: property '{}'. size '{}'", (size_t) mProperty, Collectors[(size_t) mProperty].size());
    }
}    
