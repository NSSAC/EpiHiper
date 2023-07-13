#include <cstring>
#include <jansson.h>

#include "math/CProgressionProperty.h"
#include "math/CValue.h"
#include "diseaseModel/CProgression.h"
#include "actions/COperation.h"
#include "utilities/CLogger.h"

CProgressionProperty::CProgressionProperty()
  : CValueInterface(Type::boolean, NULL)
  , mProperty(Property::__SIZE)
  , mpProgression(nullptr)
  , mpPropertyOf(nullptr)
  , mpExecute(nullptr)
  , mValid(false)
{}

CProgressionProperty::CProgressionProperty(const CProgressionProperty & src)
  : CValueInterface(src)
  , mProperty(src.mProperty)
  , mpProgression(src.mpProgression)
  , mpPropertyOf(src.mpPropertyOf)
  , mpExecute(src.mpExecute)
  , mValid(src.mValid)
{}

CProgressionProperty::CProgressionProperty(const json_t * json)
  : CValueInterface(Type::boolean, NULL)
  , mProperty(Property::__SIZE)
  , mpProgression(nullptr)
  , mpPropertyOf(nullptr)
  , mpExecute(nullptr)
  , mValid(false)
{
  fromJSON(json);
}

// virtual 
CProgressionProperty::~CProgressionProperty()
{}

// virtual 
void CProgressionProperty::fromJSON(const json_t * json)
{
  mValid = false;
  json_t * pObject = json_object_get(json, "transition");

  if (!json_is_object(pObject))
    {
      return;
    }

  mpProgression = nullptr;
  json_t * pValue = json_object_get(pObject, "idRef");

  if (json_is_string(pValue))
    {
      mpProgression = CModel::GetProgression(json_string_value(pValue));

      if (mpProgression == nullptr)
        {
          CLogger::error("Progression property: Invalid progression '{}'", json_string_value(pValue));
          return;
        }  
    }

  if (mpProgression == nullptr)
    {
      CLogger::error("Progression property: Missing attribute progression.");
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
          mpPropertyOf = &CProgressionProperty::id;
          mpExecute = &CProgressionProperty::setId;
        }
      else if (strcmp(Property, "entryState") == 0)
        {
          mProperty = Property::entryState;
          mType = Type::id;
          mpPropertyOf = &CProgressionProperty::entryState;
          mpExecute = &CProgressionProperty::setEntryState;
        }
      else if (strcmp(Property, "exitState") == 0)
        {
          mProperty = Property::exitState;
          mType = Type::id;
          mpPropertyOf = &CProgressionProperty::exitState;
          mpExecute = &CProgressionProperty::setExitState;
        }
      else if (strcmp(Property, "propensity") == 0)
        {
          mProperty = Property::propensity;
          mType = Type::number;
          mpPropertyOf = &CProgressionProperty::propensity;
          mpExecute = &CProgressionProperty::setPropensity;
        }
      else if (strcmp(Property, "dwellTime") == 0)
        {
          mProperty = Property::dwellTime;
          mType = Type::string;
          mpPropertyOf = &CProgressionProperty::dwellTime;
          mpExecute = &CProgressionProperty::setDwellTime;
        }
      else if (strcmp(Property, "susceptibilityFactorOperation") == 0)
        {
          mProperty = Property::susceptibilityFactorOperation;
          mType = Type::string;
          mpPropertyOf = &CProgressionProperty::susceptibilityFactorOperation;
          mpExecute = &CProgressionProperty::setSusceptibilityFactorOperation;
        }
      else if (strcmp(Property, "infectivityFactorOperation") == 0)
        {
          mProperty = Property::infectivityFactorOperation;
          mType = Type::string;
          mpPropertyOf = &CProgressionProperty::infectivityFactorOperation;
          mpExecute = &CProgressionProperty::setInfectivityFactorOperation;
        }
      else
        {
          CLogger::error("Progression property: Invalid property '{}'", Property);
          return;
        }

      mValid = true;
      return;
    }
}

bool CProgressionProperty::operator != (const CProgressionProperty & rhs) const
{
  return mProperty != rhs.mProperty;
}

bool CProgressionProperty::operator < (const CProgressionProperty & rhs) const
{
    return mProperty < rhs.mProperty;
}

const bool & CProgressionProperty::isValid() const
{
  return mValid;
}

CValueInterface CProgressionProperty::propertyValue() const
{
  return (*mpPropertyOf)(const_cast< CProgression * >(mpProgression));
}

bool CProgressionProperty::execute(const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info)
{
  return (*mpExecute)(mpProgression, value, pOperator, info);
}

bool CProgressionProperty::isReadOnly() const
{
  return mpPropertyOf == &CProgressionProperty::id;
}

CValueInterface CProgressionProperty::id(CProgression * pProgression) 

{
  return pProgression->getId();
}

CValueInterface CProgressionProperty::entryState(CProgression * pProgression)
{
  return pProgression->getEntryState()->getIndex();
}

CValueInterface CProgressionProperty::exitState(CProgression * pProgression)
{
  return pProgression->getExitState()->getIndex();
}

CValueInterface CProgressionProperty::propensity(CProgression * pProgression)
{
  return pProgression->getPropensity();
}

CValueInterface CProgressionProperty::dwellTime(CProgression * pProgression)
{
  return pProgression->getDwellTime();
}

CValueInterface CProgressionProperty::susceptibilityFactorOperation(CProgression * pProgression)
{
  return pProgression->getSusceptibilityFactorOperation();
}

CValueInterface CProgressionProperty::infectivityFactorOperation(CProgression * pProgression)
{
  return pProgression->getInfectivityFactorOperation();
}

bool CProgressionProperty::setId(CProgression * pProgression, const CValueInterface & /* value */, CValueInterface::pOperator /* pOperator */, const CMetadata & /* info */)
{
  CLogger::critical("Invalid operation 'setId' for progression: {}" , pProgression->getId());
  return false;
}

bool CProgressionProperty::setEntryState(CProgression * pProgression, const CValueInterface & /* value */, CValueInterface::pOperator /* pOperator */, const CMetadata & /* info */)
{
  CLogger::critical("Invalid operation 'setEntryState' for progression: {}" , pProgression->getId());
  return false;
}

bool CProgressionProperty::setExitState(CProgression * pProgression, const CValueInterface & /* value */, CValueInterface::pOperator /* pOperator */, const CMetadata & /* info */)
{
  CLogger::critical("Invalid operation 'setExitState' for progression: {}" , pProgression->getId());
  return false;
}

bool CProgressionProperty::setPropensity(CProgression * pProgression, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info)
{
  return COperation::execute< CProgression, double >(pProgression, value.toNumber(), pOperator, &CProgression::setPropensity, info);
}

bool CProgressionProperty::setDwellTime(CProgression * pProgression, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info)
{
  return COperation::execute< CProgression, std::string >(pProgression, value.toString(), pOperator, &CProgression::setDwellTime, info);
}

bool CProgressionProperty::setSusceptibilityFactorOperation(CProgression * pProgression, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info)
{
  return COperation::execute< CProgression, std::string >(pProgression, value.toString(), pOperator, &CProgression::setSusceptibilityFactorOperation, info);
}

bool CProgressionProperty::setInfectivityFactorOperation(CProgression * pProgression, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info)
{
  return COperation::execute< CProgression, std::string >(pProgression, value.toString(), pOperator, &CProgression::setInfectivityFactorOperation, info);
}
