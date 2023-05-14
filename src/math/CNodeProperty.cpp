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
  , mpCreateOperation(NULL)
  , mValid(false)
{}

CNodeProperty::CNodeProperty(const CNodeProperty & src)
  : CValueInterface(src)
  , mProperty(src.mProperty)
  , mpPropertyOf(src.mpPropertyOf)
  , mpCreateOperation(src.mpCreateOperation)
  , mValid(src.mValid)
{}

CNodeProperty::CNodeProperty(const json_t * json)
  : CValueInterface(Type::boolean, NULL)
  , mProperty(Property::__SIZE)
  , mpPropertyOf(NULL)
  , mpCreateOperation(NULL)
  , mValid(false)
{
  fromJSON(json);
}

CNodeProperty::~CNodeProperty()
{}

// virtual
CValueInterface * CNodeProperty::copy() const
{
  return new CNodeProperty(*this);
}

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
          mpCreateOperation = &CNodeProperty::setId;
        }
      else if (strcmp(Property, "susceptibilityFactor") == 0)
        {
          mProperty = Property::susceptibilityFactor;
          mType = Type::number;
          mpPropertyOf = &CNodeProperty::susceptibilityFactor;
          mpCreateOperation = &CNodeProperty::setSusceptibilityFactor;
        }
      else if (strcmp(Property, "infectivityFactor") == 0)
        {
          mProperty = Property::infectivityFactor;
          mType = Type::number;
          mpPropertyOf = &CNodeProperty::infectivityFactor;
          mpCreateOperation = &CNodeProperty::setInfectivityFactor;
        }
      else if (strcmp(Property, "healthState") == 0)
        {
          mProperty = Property::healthState;
          mType = Type::id;
          mpPropertyOf = &CNodeProperty::healthState;
          mpCreateOperation = &CNodeProperty::setHealthState;
        }
      else if (strcmp(Property, "nodeTrait") == 0)
        {
          mProperty = Property::nodeTrait;
          mType = Type::traitData;
          mpPropertyOf = &CNodeProperty::nodeTrait;
          mpCreateOperation = &CNodeProperty::setNodeTrait;
        }
      else
        {
          CLogger::error() << "Node property: Invalid property '" << Property << "'";
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

CValue CNodeProperty::propertyOf(const CNode * pNode)
{
  return (this->*mpPropertyOf)(const_cast< CNode * >(pNode));
}

COperation * CNodeProperty::createOperation(CNode * pNode, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info)
{
  return (this->*mpCreateOperation)(pNode, value, pOperator, info);
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

CValue CNodeProperty::id(CNode * pNode)
{
  return CValue(pNode->id);
}

CValue CNodeProperty::susceptibilityFactor(CNode * pNode)
{
  return CValue(pNode->susceptibilityFactor);
}

CValue CNodeProperty::infectivityFactor(CNode * pNode)
{
  return CValue(pNode->infectivityFactor);
}

CValue CNodeProperty::healthState(CNode * pNode)
{
  return CValue(pNode->healthState);
}

CValue CNodeProperty::nodeTrait(CNode * pNode)
{
  return CValue(pNode->nodeTrait);
}

COperation * CNodeProperty::setId(CNode * pNode, const CValueInterface & /* value */, CValueInterface::pOperator /* pOperator */, const CMetadata & /* info */)
{
  CLogger::critical() << "Invalid operation 'setId' for node: " << pNode->id;
  return NULL;
}

COperation * CNodeProperty::setSusceptibilityFactor(CNode * pNode, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info)
{
  return new COperationWithCollector< CNode, double >(pNode, value.toNumber(), pOperator, &CNode::setSusceptibilityFactor, Collectors[(size_t) mProperty], info);
}

COperation * CNodeProperty::setInfectivityFactor(CNode * pNode, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info)
{
  return new COperationWithCollector< CNode, double >(pNode, value.toNumber(), pOperator, &CNode::setInfectivityFactor, Collectors[(size_t) mProperty], info);
}

COperation * CNodeProperty::setHealthState(CNode * pNode, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info)
{
  CMetadata Info(info);
  Info.set("StateChange", true);

  return new COperationWithCollector< CNode, size_t >(pNode, value.toId(), pOperator, &CNode::setHealthState, Collectors[(size_t) mProperty], Info);
}

COperation * CNodeProperty::setNodeTrait(CNode * pNode, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info)
{
  return new COperationWithCollector< CNode, CTraitData::value >(pNode, value.toTraitValue(), pOperator, &CNode::setNodeTrait, Collectors[(size_t) mProperty], info);
}

void CNodeProperty::registerSetCollector(std::shared_ptr< CSetCollectorInterface > pCollector) const
{
  Collectors[(size_t) mProperty].insert(pCollector);
  CLogger::debug() << "CNodeProperty::registerSetCollector: property '" << (size_t) mProperty << "'. size '" << Collectors[(size_t) mProperty].size() << "'";
}    

void CNodeProperty::deregisterSetCollector(std::shared_ptr< CSetCollectorInterface > pCollector) const
{
  if (!Collectors[(size_t) mProperty].empty())
    {
      Collectors[(size_t) mProperty].erase(pCollector);
      CLogger::debug() << "CNodeProperty::deregisterSetCollector: property '" << (size_t) mProperty << "'. size '" << Collectors[(size_t) mProperty].size() << "'";
    }
}    
