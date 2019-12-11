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
#include <jansson.h>

#include "math/CNodeProperty.h"
#include "network/CNode.h"
#include "network/CEdge.h"
#include "diseaseModel/CModel.h"
#include "actions/COperation.h"

CNodeProperty::CNodeProperty()
  : CValueInterface(Type::boolean, NULL)
  , mpPropertyOf(NULL)
  , mpCreateOperation(NULL)
  , mValid(true)
{}

CNodeProperty::CNodeProperty(const CNodeProperty & src)
  : CValueInterface(src)
  , mpPropertyOf(src.mpPropertyOf)
  , mpCreateOperation(src.mpCreateOperation)
  , mValid(src.mValid)
{}

CNodeProperty::CNodeProperty(const json_t * json)
  : CValueInterface(Type::boolean, NULL)
  , mpPropertyOf(NULL)
  , mpCreateOperation(NULL)
  , mValid(true)
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
  json_t * pObject = json_object_get(json, "node");

  if (!json_is_object(pObject))
    {
      mValid = false;
      return;
    }

  json_t * pValue = json_object_get(pObject, "property");

  if (!json_is_string(pValue))
    {
      mValid = false;
      return;
    }

  if (strcmp(json_string_value(pValue), "id") == 0)
    {
      mType = Type::id;
      mpPropertyOf = &CNodeProperty::id;
      mpCreateOperation = &CNodeProperty::setId;
    }
  else if (strcmp(json_string_value(pValue), "susceptibilityFactor") == 0)
    {
      mType = Type::number;
      mpPropertyOf = &CNodeProperty::susceptibilityFactor;
      mpCreateOperation = &CNodeProperty::setSusceptibilityFactor;
    }
  else if (strcmp(json_string_value(pValue), "infectivityFactor") == 0)
    {
      mType = Type::number;
      mpPropertyOf = &CNodeProperty::infectivityFactor;
      mpCreateOperation = &CNodeProperty::setInfectivityFactor;
    }
  else if (strcmp(json_string_value(pValue), "healthState") == 0)
    {
      mType = Type::id;
      mpPropertyOf = &CNodeProperty::healthState;
      mpCreateOperation = &CNodeProperty::setHealthState;
    }
  else  if (strcmp(json_string_value(pValue), "nodeTrait") == 0)
    {
      mType = Type::traitData;
      mpPropertyOf = &CNodeProperty::nodeTrait;
      mpCreateOperation = &CNodeProperty::setNodeTrait;
    }
  else
    {
      mValid = false;
    }

  return;
}

const bool & CNodeProperty::isValid() const
{
  return mValid;
}

CValueInterface & CNodeProperty::propertyOf(const CNode * pNode)
{
  return (this->*mpPropertyOf)(const_cast< CNode * >(pNode));
}

COperation * CNodeProperty::createOperation(CNode * pNode, const CValueInterface & value, CValueInterface::pOperator pOperator)
{
  return (this->*mpCreateOperation)(pNode, value, pOperator);
}

// static
std::pair< CEdge *, CEdge * > CNodeProperty::edges(CNode * pNode)
{
  return std::make_pair(pNode->Edges, pNode->Edges + pNode->EdgesSize);
}

CValueInterface & CNodeProperty::id(CNode * pNode)
{
  mpValue = &pNode->id;
  return *this;
}

CValueInterface & CNodeProperty::susceptibilityFactor(CNode * pNode)
{
  mpValue = &pNode->susceptibilityFactor;
  return *this;
}

CValueInterface & CNodeProperty::infectivityFactor(CNode * pNode)
{
  mpValue = &pNode->infectivityFactor;
  return *this;
}

CValueInterface & CNodeProperty::healthState(CNode * pNode)
{
  mpValue = &pNode->healthState;
  return *this;
}

CValueInterface & CNodeProperty::nodeTrait(CNode * pNode)
{
  mpValue = &pNode->nodeTrait;
  return *this;
}

COperation * CNodeProperty::setId(CNode * pNode, const CValueInterface & value, CValueInterface::pOperator pOperator)
{
  std::ostringstream os;

  os << "Invalid operation for node: " << pNode->id;

  FatalError(CCommunicate::ErrorCode::InvalidOperation, os.str());

  return NULL;
}

COperation * CNodeProperty::setSusceptibilityFactor(CNode * pNode, const CValueInterface & value, CValueInterface::pOperator pOperator)
{
  return new COperationInstance< CNode, double >(pNode, value.toNumber(), pOperator, &CNode::setSusceptibilityFactor);
}

COperation * CNodeProperty::setInfectivityFactor(CNode * pNode, const CValueInterface & value, CValueInterface::pOperator pOperator)
{
  return new COperationInstance< CNode, double >(pNode, value.toNumber(), pOperator, &CNode::setInfectivityFactor);
}

COperation * CNodeProperty::setHealthState(CNode * pNode, const CValueInterface & value, CValueInterface::pOperator pOperator)
{
  return new COperationInstance< CNode, size_t >(pNode, value.toId(), pOperator, &CNode::setHealthState, CMetadata("StateChange", true));
}

COperation * CNodeProperty::setNodeTrait(CNode * pNode, const CValueInterface & value, CValueInterface::pOperator pOperator)
{
  return new COperationInstance< CNode, CTraitData::value >(pNode, value.toTraitValue(), pOperator, &CNode::setNodeTrait);
}



