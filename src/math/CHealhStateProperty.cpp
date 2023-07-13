#include <cstring>
#include <jansson.h>

#include "math/CHealthStateProperty.h"
#include "math/CValue.h"
#include "diseaseModel/CHealthState.h"
#include "actions/COperation.h"
#include "utilities/CLogger.h"

CHealthStateProperty::CHealthStateProperty()
  : CValueInterface(Type::boolean, NULL)
  , mProperty(Property::__SIZE)
  , mpHealthState(nullptr)
  , mpPropertyOf(nullptr)
  , mpExecute(nullptr)
  , mValid(false)
{}

CHealthStateProperty::CHealthStateProperty(const CHealthStateProperty & src)
  : CValueInterface(src)
  , mProperty(src.mProperty)
  , mpHealthState(src.mpHealthState)
  , mpPropertyOf(src.mpPropertyOf)
  , mpExecute(src.mpExecute)
  , mValid(src.mValid)
{}

CHealthStateProperty::CHealthStateProperty(const json_t * json)
  : CValueInterface(Type::boolean, NULL)
  , mProperty(Property::__SIZE)
  , mpHealthState(nullptr)
  , mpPropertyOf(nullptr)
  , mpExecute(nullptr)
  , mValid(false)
{
  fromJSON(json);
}

// virtual 
CHealthStateProperty::~CHealthStateProperty()
{}

// virtual 
void CHealthStateProperty::fromJSON(const json_t * json)
{
  mValid = false;
  json_t * pObject = json_object_get(json, "healthState");

  if (!json_is_object(pObject))
    {
      return;
    }

  mpHealthState = nullptr;
  json_t * pValue = json_object_get(pObject, "idRef");

  if (json_is_string(pValue))
    {
      mpHealthState = CModel::GetState(json_string_value(pValue));

      if (mpHealthState == nullptr)
        {
          CLogger::error("Health state property: Invalid health state '{}'", json_string_value(pValue));
          return;
        }
    }

  if (mpHealthState == nullptr)
    {
      CLogger::error("Health state property: Missing health state");
      return;
    }

  pValue = json_object_get(pObject, "property");

  if (json_is_string(pValue))
    {
      const char * Property = json_string_value(pValue);

      if (strcmp(Property, "id") == 0)
        {
          mProperty = Property::id;
          mType = Type::id;
          mpPropertyOf = &CHealthStateProperty::id;
          mpExecute = &CHealthStateProperty::setId;
        }
      else if (strcmp(Property, "susceptibility") == 0)
        {
          mProperty = Property::susceptibility;
          mType = Type::number;
          mpPropertyOf = &CHealthStateProperty::susceptibility;
          mpExecute = &CHealthStateProperty::setSusceptibility;
        }
      else if (strcmp(Property, "infectivity") == 0)
        {
          mProperty = Property::infectivity;
          mType = Type::number;
          mpPropertyOf = &CHealthStateProperty::infectivity;
          mpExecute = &CHealthStateProperty::setInfectivity;
        }
      else
        {
          CLogger::error("Health state property: Invalid property '{}'", Property);
          return;
        }

      mValid = true;
      return;
    }
}

bool CHealthStateProperty::operator != (const CHealthStateProperty & rhs) const
{
  return mProperty != rhs.mProperty;
}

bool CHealthStateProperty::operator < (const CHealthStateProperty & rhs) const
{
    return mProperty < rhs.mProperty;
}

const bool & CHealthStateProperty::isValid() const
{
  return mValid;
}

CValueInterface CHealthStateProperty::propertyValue() const
{
  return (*mpPropertyOf)(const_cast< CHealthState * >(mpHealthState));
}

bool CHealthStateProperty::execute(const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info)
{
  return (*mpExecute)(mpHealthState, value, pOperator, info);
}

bool CHealthStateProperty::isReadOnly() const
{
  return mpPropertyOf == &CHealthStateProperty::id;
}

CValueInterface CHealthStateProperty::id(CHealthState * pHealthState)
{
  return pHealthState->getId();
}

CValueInterface CHealthStateProperty::susceptibility(CHealthState * pHealthState)
{
  return pHealthState->getSusceptibility();
}

CValueInterface CHealthStateProperty::infectivity(CHealthState * pHealthState)
{
  return pHealthState->getInfectivity();
}

bool CHealthStateProperty::setId(CHealthState * pHealthState, const CValueInterface & /* value */, CValueInterface::pOperator /* pOperator */, const CMetadata & /* info */)
{
  CLogger::critical("Invalid operation 'setId' for health state: {}" , pHealthState->getId());
  return false;
}
bool CHealthStateProperty::setSusceptibility(CHealthState * pHealthState, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info)
{
  return COperation::execute< CHealthState, double >(pHealthState, value.toNumber(), pOperator, &CHealthState::setSusceptibility, info);
}

bool CHealthStateProperty::setInfectivity(CHealthState * pHealthState, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info)
{
  return COperation::execute< CHealthState, double >(pHealthState, value.toNumber(), pOperator, &CHealthState::setInfectivity, info);
}

