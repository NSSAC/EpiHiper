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

#include "math/CEdgeProperty.h"
#include "math/CValue.h"
#include "network/CEdge.h"
#include "traits/CTrait.h"
#include "actions/COperation.h"
#include "utilities/CLogger.h"

// static 
std::vector< std::set< std::shared_ptr< CSetCollectorInterface > > > CEdgeProperty::Collectors((size_t) CEdgeProperty::Property::__SIZE);

CEdgeProperty::CEdgeProperty()
  : CValueInterface(Type::boolean, NULL)
  , mProperty(Property::__SIZE)
  , mpPropertyOf(NULL)
  , mpExecute(NULL)
  , mValid(false)
{}

CEdgeProperty::CEdgeProperty(const CEdgeProperty & src)
  : CValueInterface(src)
  , mProperty(src.mProperty)
  , mpPropertyOf(src.mpPropertyOf)
  , mpExecute(src.mpExecute)
  , mValid(src.mValid)
{}

CEdgeProperty::CEdgeProperty(const json_t * json)
  : CValueInterface(Type::boolean, NULL)
  , mProperty(Property::__SIZE)
  , mpPropertyOf(NULL)
  , mpExecute(NULL)
  , mValid(false)
{
  fromJSON(json);
}

CEdgeProperty::~CEdgeProperty()
{}

void CEdgeProperty::fromJSON(const json_t * json)
{
  mValid = false; // DONE
  json_t * pObject = json_object_get(json, "edge");

  if (!json_is_object(pObject))
    {
      return;
    }

  json_t * pValue = json_object_get(pObject, "property");

  if (json_is_string(pValue))
    {
      const char * Property = json_string_value(pValue);

      if (strcmp(Property, "duration") == 0)
        {
          mProperty = Property::duration;
          mType = Type::number;
          mpPropertyOf = &CEdgeProperty::duration;
          mpExecute = &CEdgeProperty::setDuration;
        }
      else if (strcmp(Property, "weight") == 0)
        {
          mProperty = Property::weight;
          mType = Type::number;
          mpPropertyOf = &CEdgeProperty::weight;
          mpExecute = &CEdgeProperty::setWeight;
        }
      else if (strcmp(Property, "active") == 0)
        {
          mProperty = Property::active;
          mType = Type::boolean;
          mpPropertyOf = &CEdgeProperty::active;
          mpExecute = &CEdgeProperty::setActive;
        }
      else if (strcmp(Property, "targetId") == 0)
        {
          mProperty = Property::targetId;
          mType = Type::id;
          mpPropertyOf = &CEdgeProperty::targetId;
          mpExecute = &CEdgeProperty::setTargetId;
        }
      else if (strcmp(Property, "sourceId") == 0)
        {
          mProperty = Property::sourceId;
          mType = Type::id;
          mpPropertyOf = &CEdgeProperty::sourceId;
          mpExecute = &CEdgeProperty::setSourceId;
        }
      else if (strcmp(Property, "targetActivity") == 0)
        {
          mProperty = Property::targetActivity;
          mType = Type::traitData;
          mpPropertyOf = &CEdgeProperty::targetActivity;
          mpExecute = &CEdgeProperty::setTargetActivity;
        }
      else if (strcmp(Property, "sourceActivity") == 0)
        {
          mProperty = Property::sourceActivity;
          mType = Type::traitData;
          mpPropertyOf = &CEdgeProperty::sourceActivity;
          mpExecute = &CEdgeProperty::setSourceActivity;
        }
      else if (strcmp(Property, "locationId") == 0)
        {
          mProperty = Property::locationId;
          mType = Type::id;
          mpPropertyOf = &CEdgeProperty::locationId;
          mpExecute = &CEdgeProperty::setLocationId;
        }
      else if (strcmp(Property, "edgeTrait") == 0)
        {
          mProperty = Property::edgeTrait;
          mType = Type::traitData;
          mpPropertyOf = &CEdgeProperty::edgeTrait;
          mpExecute = &CEdgeProperty::setEdgeTrait;
        }
      else
        {
          CLogger::error("Edge property: Invalid property '{}'", Property);
          return;
        }

      mValid = true;
      return;
    }
}

bool CEdgeProperty::operator != (const CEdgeProperty & rhs) const
{
  return mProperty != rhs.mProperty;
}

bool CEdgeProperty::operator < (const CEdgeProperty & rhs) const
{
  return mProperty < rhs.mProperty;
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

CValueInterface CEdgeProperty::propertyOf(const CEdge * pEdge) const
{
  return (this->*mpPropertyOf)(const_cast< CEdge * >(pEdge));
}

bool CEdgeProperty::execute(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info)
{
  return (this->*mpExecute)(pEdge, value, pOperator, info);
}

bool CEdgeProperty::isReadOnly() const
{
  return mpPropertyOf == &CEdgeProperty::targetId
         || mpPropertyOf == &CEdgeProperty::sourceId
         || mpPropertyOf == &CEdgeProperty::targetActivity
         || mpPropertyOf == &CEdgeProperty::sourceActivity
         || mpPropertyOf == &CEdgeProperty::locationId
         || mpPropertyOf == &CEdgeProperty::duration;
}

CValueInterface CEdgeProperty::targetId(CEdge * pEdge) const
{
  return CValueInterface(pEdge->targetId);
}
  
CValueInterface CEdgeProperty::sourceId(CEdge * pEdge) const
{
  return CValueInterface(pEdge->sourceId);
}

CValueInterface CEdgeProperty::targetActivity(CEdge * pEdge) const
{
  return CValueInterface(pEdge->targetActivity);
}

CValueInterface CEdgeProperty::sourceActivity(CEdge * pEdge) const
{
  return CValueInterface(pEdge->sourceActivity);
}

#ifdef USE_LOCATION_ID
CValueInterface CEdgeProperty::locationId(CEdge * pEdge) const
{
  return CValueInterface(pEdge->locationId);
}
#else
CValueInterface CEdgeProperty::locationId(CEdge * /* pEdge */) const
{
  static size_t Invalid(std::numeric_limits< size_t >::max());
  return CValueInterface(Invalid);
}
#endif // USE_LOCATION_ID

CValueInterface CEdgeProperty::edgeTrait(CEdge * pEdge) const
{
  return CValueInterface(pEdge->edgeTrait);
}

CValueInterface CEdgeProperty::active(CEdge * pEdge) const
{
  return CValueInterface(pEdge->active);
}

CValueInterface CEdgeProperty::weight(CEdge * pEdge) const
{
  return CValueInterface(pEdge->weight);
}

CValueInterface CEdgeProperty::duration(CEdge * pEdge) const
{
  return CValueInterface(pEdge->duration);
}

bool CEdgeProperty::setTargetId(CEdge * pEdge, const CValueInterface & /* value */, CValueInterface::pOperator /* pOperator */, const CMetadata & /* info */)
{
  CLogger::critical("Invalid operation 'setTargetId' for edge: {}, {}" , pEdge->targetId, pEdge->sourceId);
  return false;
}

bool CEdgeProperty::setSourceId(CEdge * pEdge, const CValueInterface & /* value */, CValueInterface::pOperator /* pOperator */, const CMetadata & /* info */)
{
  CLogger::critical("Invalid operation 'setSourceId' for edge: {}, {}" , pEdge->targetId, pEdge->sourceId);
  return false;
}

bool CEdgeProperty::setTargetActivity(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info)
{
  return COperation::execute< CEdge, CTraitData::value >(pEdge, value.toTraitValue(), pOperator, &CEdge::setTargetActivity, Collectors[(size_t) mProperty], info);
}

bool CEdgeProperty::setSourceActivity(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info)
{
  return COperation::execute< CEdge, CTraitData::value >(pEdge, value.toTraitValue(), pOperator, &CEdge::setSourceActivity, Collectors[(size_t) mProperty], info);
}

bool CEdgeProperty::setLocationId(CEdge * pEdge, const CValueInterface & /* value */, CValueInterface::pOperator /* pOperator */, const CMetadata & /* info */)
{
  CLogger::critical("Invalid operation 'setLocationId' for edge: {}, {}" , pEdge->targetId, pEdge->sourceId);
  return false;
}

bool CEdgeProperty::setEdgeTrait(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info)
{
  return COperation::execute< CEdge, CTraitData::value >(pEdge, value.toTraitValue(), pOperator, &CEdge::setEdgeTrait, Collectors[(size_t) mProperty], info);
}

bool CEdgeProperty::setActive(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info)
{
  return COperation::execute< CEdge, bool >(pEdge, value.toBoolean(), pOperator, &CEdge::setActive, Collectors[(size_t) mProperty], info);
}

bool CEdgeProperty::setWeight(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info)
{
  return COperation::execute< CEdge, double >(pEdge, value.toNumber(), pOperator, &CEdge::setWeight, Collectors[(size_t) mProperty], info);
}

bool CEdgeProperty::setDuration(CEdge * pEdge, const CValueInterface & /* value */, CValueInterface::pOperator /* pOperator */, const CMetadata & /* info */)
{
  CLogger::critical("Invalid operation 'setDuration' for edge: {}, {}" , pEdge->targetId, pEdge->sourceId);
  return false;
}

void CEdgeProperty::registerSetCollector(std::shared_ptr< CSetCollectorInterface > pCollector) const
{
  Collectors[(size_t) mProperty].insert(pCollector);
  CLogger::debug("CEdgeProperty::registerSetCollector: property '{}'. size '{}'", (size_t) mProperty, Collectors[(size_t) mProperty].size());
}

void CEdgeProperty::deregisterSetCollector(std::shared_ptr< CSetCollectorInterface > pCollector) const
{
  if (!Collectors[(size_t) mProperty].empty())
    {
      Collectors[(size_t) mProperty].erase(pCollector);
      CLogger::debug("CEdgeProperty::deregisterSetCollector: property '{}'. size '{}'",(size_t) mProperty, Collectors[(size_t) mProperty].size());
    }
}

