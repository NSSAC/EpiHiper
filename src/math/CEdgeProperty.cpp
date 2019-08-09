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

#include "math/CEdgeProperty.h"
#include "network/CEdge.h"
#include "traits/CTrait.h"
#include "actions/COperation.h"

CEdgeProperty::CEdgeProperty()
  : CValueInterface(Type::boolean, NULL)
  , mpPropertyOf(NULL)
  , mpCreateOperation(NULL)
  , mNodeProperty()
  , mValid(true)
{}

CEdgeProperty::~CEdgeProperty()
{}

void CEdgeProperty::fromJSON(const json_t * json)
{
  json_t * pValue = json_object_get(json, "property");

  if (json_is_string(pValue))
    {
      if (strcmp(json_string_value(pValue), "weight") == 0)
        {
          mType = Type::number;
          mpPropertyOf = &CEdgeProperty::weight;
          mpCreateOperation = &CEdgeProperty::setWeight;
        }
      else if (strcmp(json_string_value(pValue), "active") == 0)
        {
          mType = Type::boolean;
          mpPropertyOf = &CEdgeProperty::active;
          mpCreateOperation = &CEdgeProperty::setActive;
        }
      else if (strcmp(json_string_value(pValue), "targetActivity") == 0)
        {
          mType = Type::traitData;
          mpPropertyOf = &CEdgeProperty::targetActivity;
          mpCreateOperation = &CEdgeProperty::setTargetActivity;
        }
      else if (strcmp(json_string_value(pValue), "sourceActivity") == 0)
        {
          mType = Type::traitData;
          mpPropertyOf = &CEdgeProperty::sourceActivity;
          mpCreateOperation = &CEdgeProperty::setSourceActivity;
        }
      else if (strcmp(json_string_value(pValue), "edgeTrait") == 0)
        {
          mType = Type::traitData;
          mpPropertyOf = &CEdgeProperty::edgeTrait;
          mpCreateOperation = &CEdgeProperty::setEdgeTrait;
        }
      else
        {
          mValid = false;
        }

      return;
    }

  mValid = false;
}

const bool & CEdgeProperty::isValid() const
{
  return mValid;
}

// static
CNode * CEdgeProperty::targetNode(CEdge * pEdge)
{
  return pEdge->pTarget;
}

// static
CNode * CEdgeProperty::sourceNode(CEdge * pEdge)
{
  return pEdge->pSource;
}

CValueInterface & CEdgeProperty::propertyOf(CEdge * pEdge)
{
  return (this->*mpPropertyOf)(pEdge);
}

COperation * CEdgeProperty::createOperation(CEdge * pEdge, const CValueInterface & value)
{
  return (this->*mpCreateOperation)(pEdge, value);
}

CValueInterface &  CEdgeProperty::targetActivity(CEdge * pEdge)
{
  mpValue = &pEdge->targetActivity;
  return *this;
}

CValueInterface &  CEdgeProperty::sourceActivity(CEdge * pEdge)
{
  mpValue = &pEdge->sourceActivity;
  return *this;
}

CValueInterface &  CEdgeProperty::edgeTrait(CEdge * pEdge)
{
  mpValue = &pEdge->edgeTrait;
  return *this;
}

CValueInterface &  CEdgeProperty::active(CEdge * pEdge)
{
  mpValue = &pEdge->active;
  return *this;
}

CValueInterface &  CEdgeProperty::weight(CEdge * pEdge)
{
  mpValue = &pEdge->weight;
  return *this;
}

COperation * CEdgeProperty::setTargetActivity(CEdge * pEdge, const CValueInterface & value)
{
  return new COperationInstance< CEdge, CTraitData::value >(*pEdge, value.toTraitValue(), &CEdge::setTargetActivity);
}

COperation * CEdgeProperty::setSourceActivity(CEdge * pEdge, const CValueInterface & value)
{
  return new COperationInstance< CEdge, CTraitData::value >(*pEdge, value.toTraitValue(), &CEdge::setSourceActivity);
}

COperation * CEdgeProperty::setEdgeTrait(CEdge * pEdge, const CValueInterface & value)
{
  return new COperationInstance< CEdge, CTraitData::value >(*pEdge, value.toTraitValue(), &CEdge::setEdgeTrait);
}

COperation * CEdgeProperty::setActive(CEdge * pEdge, const CValueInterface & value)
{
  return new COperationInstance< CEdge, bool >(*pEdge, value.toBoolean(), &CEdge::setActive);
}

COperation * CEdgeProperty::setWeight(CEdge * pEdge, const CValueInterface & value)
{
  return new COperationInstance< CEdge, double >(*pEdge, value.toNumber(), &CEdge::setWeight);
}
