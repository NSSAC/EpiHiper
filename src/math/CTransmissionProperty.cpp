#include <cstring>
#include <jansson.h>

#include "math/CTransmissionProperty.h"
#include "math/CValue.h"
#include "diseaseModel/CTransmission.h"
#include "actions/COperation.h"
#include "utilities/CLogger.h"

CTransmissionProperty::CTransmissionProperty()
  : CValueInterface(Type::boolean, NULL)
  , mProperty(Property::__SIZE)
  , mpTransmission(nullptr)
  , mpPropertyOf(nullptr)
  , mpExecute(nullptr)
  , mValid(false)
{}

CTransmissionProperty::CTransmissionProperty(const CTransmissionProperty & src)
  : CValueInterface(src)
  , mProperty(src.mProperty)
  , mpTransmission(src.mpTransmission)
  , mpPropertyOf(src.mpPropertyOf)
  , mpExecute(src.mpExecute)
  , mValid(src.mValid)
{}

CTransmissionProperty::CTransmissionProperty(const json_t * json)
  : CValueInterface(Type::boolean, NULL)
  , mProperty(Property::__SIZE)
  , mpTransmission(nullptr)
  , mpPropertyOf(nullptr)
  , mpExecute(nullptr)
  , mValid(false)
{
  fromJSON(json);
}

// virtual 
CTransmissionProperty::~CTransmissionProperty()
{}

// virtual 
void CTransmissionProperty::fromJSON(const json_t * json)
{
  mValid = false;
  json_t * pObject = json_object_get(json, "transmission");

  if (!json_is_object(pObject))
    {
      return;
    }

  mpTransmission = nullptr;
  json_t * pValue = json_object_get(pObject, "idRef");

  if (json_is_string(pValue))
    {
      mpTransmission = CModel::GetTransmission(json_string_value(pValue));

      if (mpTransmission == nullptr)
        {
          CLogger::error("Transmission property: Invalid transmission '{}'", json_string_value(pValue));
          return;
        }  
    }

  if (mpTransmission == nullptr)
    {
      CLogger::error("Transmission property: Missing attribute transmission.");
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
          mpPropertyOf = &CTransmissionProperty::id;
          mpExecute = &CTransmissionProperty::setId;
        }
      else if (strcmp(Property, "entryState") == 0)
        {
          mProperty = Property::entryState;
          mType = Type::id;
          mpPropertyOf = &CTransmissionProperty::entryState;
          mpExecute = &CTransmissionProperty::setEntryState;
        }
      else if (strcmp(Property, "exitState") == 0)
        {
          mProperty = Property::exitState;
          mType = Type::id;
          mpPropertyOf = &CTransmissionProperty::exitState;
          mpExecute = &CTransmissionProperty::setExitState;
        }
      else if (strcmp(Property, "contactState") == 0)
        {
          mProperty = Property::contactState;
          mType = Type::id;
          mpPropertyOf = &CTransmissionProperty::contactState;
          mpExecute = &CTransmissionProperty::setContactState;
        }
      else if (strcmp(Property, "transmissibility") == 0)
        {
          mProperty = Property::transmissibility;
          mType = Type::number;
          mpPropertyOf = &CTransmissionProperty::transmissibility;
          mpExecute = &CTransmissionProperty::setTransmissibility;
        }
      else if (strcmp(Property, "susceptibilityFactorOperation") == 0)
        {
          mProperty = Property::susceptibilityFactorOperation;
          mType = Type::string;
          mpPropertyOf = &CTransmissionProperty::susceptibilityFactorOperation;
          mpExecute = &CTransmissionProperty::setSusceptibilityFactorOperation;
        }
      else if (strcmp(Property, "infectivityFactorOperation") == 0)
        {
          mProperty = Property::infectivityFactorOperation;
          mType = Type::string;
          mpPropertyOf = &CTransmissionProperty::infectivityFactorOperation;
          mpExecute = &CTransmissionProperty::setInfectivityFactorOperation;
        }
      else
        {
          CLogger::error("Transmission property: Invalid property '{}'", Property);
          return;
        }

      mValid = true;
      return;
    }
}

bool CTransmissionProperty::operator != (const CTransmissionProperty & rhs) const
{
  return mProperty != rhs.mProperty;
}

bool CTransmissionProperty::operator < (const CTransmissionProperty & rhs) const
{
    return mProperty < rhs.mProperty;
}

const bool & CTransmissionProperty::isValid() const
{
  return mValid;
}

CValueInterface CTransmissionProperty::propertyValue() const
{
  return (*mpPropertyOf)(const_cast< CTransmission * >(mpTransmission));
}

bool CTransmissionProperty::execute(const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info)
{
  return (*mpExecute)(mpTransmission, value, pOperator, info);
}

bool CTransmissionProperty::isReadOnly() const
{
  return mpPropertyOf == &CTransmissionProperty::id;
}

CValueInterface CTransmissionProperty::id(CTransmission * pTransmission)
{
  return pTransmission->getId();
}

CValueInterface CTransmissionProperty::entryState(CTransmission * pTransmission)
{
  return pTransmission->getEntryState()->getIndex();
}

CValueInterface CTransmissionProperty::exitState(CTransmission * pTransmission)
{
  return pTransmission->getExitState()->getIndex();
}

CValueInterface CTransmissionProperty::contactState(CTransmission * pTransmission)
{
  return pTransmission->getContactState()->getIndex();
}

CValueInterface CTransmissionProperty::transmissibility(CTransmission * pTransmission)
{
  return pTransmission->getTransmissibility();
}

CValueInterface CTransmissionProperty::susceptibilityFactorOperation(CTransmission * pTransmission)
{
  return pTransmission->getSusceptibilityFactorOperation();
}

CValueInterface CTransmissionProperty::infectivityFactorOperation(CTransmission * pTransmission)
{
  return pTransmission->getInfectivityFactorOperation();
}

bool CTransmissionProperty::setId(CTransmission * pTransmission, const CValueInterface & /* value */, CValueInterface::pOperator /* pOperator */, const CMetadata & /* info */)
{
  CLogger::critical("Invalid operation 'setId' for transmission: {}" , pTransmission->getId());
  return false;
}

bool CTransmissionProperty::setEntryState(CTransmission * pTransmission, const CValueInterface & /* value */, CValueInterface::pOperator /* pOperator */, const CMetadata & /* info */)
{
  CLogger::critical("Invalid operation 'setEntryState' for transmission: {}" , pTransmission->getId());
  return false;
}

bool CTransmissionProperty::setExitState(CTransmission * pTransmission, const CValueInterface & /* value */, CValueInterface::pOperator /* pOperator */, const CMetadata & /* info */)
{
  CLogger::critical("Invalid operation 'setExitState' for transmission: {}" , pTransmission->getId());
  return false;
}

bool CTransmissionProperty::setContactState(CTransmission * pTransmission, const CValueInterface & /* value */, CValueInterface::pOperator /* pOperator */, const CMetadata & /* info */)
{
  CLogger::critical("Invalid operation 'setContactState' for transmission: {}" , pTransmission->getId());
  return false;
}

bool CTransmissionProperty::setTransmissibility(CTransmission * pTransmission, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info)
{
  return COperation::execute< CTransmission, double >(pTransmission, value.toNumber(), pOperator, &CTransmission::setTransmissibility, info);
}


bool CTransmissionProperty::setSusceptibilityFactorOperation(CTransmission * pTransmission, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info)
{
  return COperation::execute< CTransmission, std::string >(pTransmission, value.toString(), pOperator, &CTransmission::setSusceptibilityFactorOperation, info);
}

bool CTransmissionProperty::setInfectivityFactorOperation(CTransmission * pTransmission, const CValueInterface & value, CValueInterface::pOperator pOperator, const CMetadata & info)
{
  return COperation::execute< CTransmission, std::string >(pTransmission, value.toString(), pOperator, &CTransmission::setInfectivityFactorOperation, info);
}
