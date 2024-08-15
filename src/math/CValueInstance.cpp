// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2023 - 2024 Rector and Visitors of the University of Virginia 
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
#include "math/CValueInstance.h"
#include "math/CObservable.h"
#include "math/CSizeOf.h"
#include "variables/CVariable.h"
#include "variables/CVariableList.h"
#include "utilities/CSimConfig.h"
#include "diseaseModel/CFactorOperation.h"
#include "diseaseModel/CDistribution.h"

// static
bool CValueInstance::compatible(const CValueInstance & lhs, const CValueInstance & rhs)
{
  return CValueInterface::compatible(lhs.interfaceType(), rhs.interfaceType());
}

CValueInstance::CValueInstance()
  : mType()
  , mWritable(false)
  , mValid(false)
  , mpPrerequisites(NULL)
  , mpValue(NULL)
  , mpValueList(NULL)
  , mpObservable(NULL)
  , mpNodeProperty(NULL)
  , mpEdgeProperty(NULL)
  , mpHealthStateProperty(NULL)
  , mpTransmissionsProperty(NULL)
  , mpProgressionProperty(NULL)
  , mpVariable(NULL)
  , mpSizeOf(NULL)
{}

CValueInstance::CValueInstance(const CValueInstance & src)
  : mType(src.mType)
  , mWritable(src.mWritable)
  , mValid(src.mValid)
  , mpPrerequisites(src.mpPrerequisites)
  , mpValue(src.mpValue)
  , mpValueList(src.mpValueList)
  , mpObservable(src.mpObservable)
  , mpNodeProperty(src.mpNodeProperty)
  , mpEdgeProperty(src.mpEdgeProperty)
  , mpHealthStateProperty(src.mpHealthStateProperty)
  , mpTransmissionsProperty(src.mpTransmissionsProperty)
  , mpProgressionProperty(src.mpProgressionProperty)
  , mpVariable(src.mpVariable)
  , mpSizeOf(src.mpSizeOf)
{}

CValueInstance::~CValueInstance()
{}

void CValueInstance::fromJSON(const json_t * json, bool writable)
{
  mValid = false;

  if (!json_is_object(json))
    {
      CLogger::error("Value Instance: Invalid.");
      return;
    }

  CLogger::pushLevel(CLogger::LogLevel::off);

  mpNodeProperty = std::shared_ptr< CNodeProperty >(new CNodeProperty(json));

  if (mpNodeProperty->isValid())
    {
      mType = ValueType::NodeProperty;
      mWritable = !mpNodeProperty->isReadOnly();
      mValid = writable ? mWritable : true;
      goto final;
    }

  mpNodeProperty.reset();

  mpEdgeProperty = std::shared_ptr< CEdgeProperty >(new CEdgeProperty(json));

  if (mpEdgeProperty->isValid())
    {
      mType = ValueType::EdgeProperty;
      mWritable = !mpEdgeProperty->isReadOnly();
      mValid = writable ? mWritable : true;
      goto final;
    }

  mpEdgeProperty.reset();

  mpHealthStateProperty = std::shared_ptr< CHealthStateProperty >(new CHealthStateProperty(json));

  if (mpHealthStateProperty->isValid())
    {
      mType = ValueType::HealthStateProperty;
      mWritable = !mpHealthStateProperty->isReadOnly();
      mValid = writable ? mWritable : true;
      goto final;
    }

  mpHealthStateProperty.reset();

  mpTransmissionsProperty = std::shared_ptr< CTransmissionProperty >(new CTransmissionProperty(json));

  if (mpTransmissionsProperty->isValid())
    {
      mType = ValueType::TransmissionProperty;
      mWritable = !mpTransmissionsProperty->isReadOnly();
      mValid = writable ? mWritable : true;
      goto final;
    }

  mpTransmissionsProperty.reset();

  mpProgressionProperty = std::shared_ptr< CProgressionProperty >(new CProgressionProperty(json));

  if (mpProgressionProperty->isValid())
    {
      mType = ValueType::ProgressionProperty;
      mWritable = !mpProgressionProperty->isReadOnly();
      mValid = writable ? mWritable : true;
      goto final;
    }

  mpProgressionProperty.reset();

  mpVariable = &CVariableList::INSTANCE[json];

  if (mpVariable->isValid())
    {
      mType = ValueType::Variable;
      mpPrerequisites = mpVariable;
      mWritable = true;
      mValid = true;
      goto final;
    }

  mpVariable = NULL;

  if (!writable)
    {
      mpValue = std::shared_ptr< CValue >(new CValue(json));

      if (mpValue->isValid())
        {
          mType = ValueType::Value;
          mWritable = false;
          mValid = true;
          goto final;
        }

      mpValue.reset();

      mpValueList = std::shared_ptr< CValueList >(new CValueList(json));

      if (mpValueList->isValid())
        {
          mType = ValueType::ValueList;
          mWritable = false;
          mValid = true;
          goto final;
        }

      mpValueList.reset();

      mpObservable = CObservable::get(json);

      if (mpObservable
          && mpObservable->isValid())
        {
          mType = ValueType::Observable;
          mpPrerequisites = mpObservable;
          mWritable = false;
          mValid = true;
          goto final;
        }

      mpObservable = NULL;

      mpSizeOf = CSizeOf::FromJSON(json);

      if (mpSizeOf
          && mpSizeOf->isValid())
        {
          mType = ValueType::SizeOf;
          mpPrerequisites = mpSizeOf.get();
          mWritable = false;
          mValid = true;
          goto final;
        }

      mpSizeOf.reset();

      // We may have factor operations or dwell time;
      CDistribution Distribution;
      Distribution.fromJSON(json);

      if (Distribution.isValid())
        {
          mpValue =  std::shared_ptr< CValue >(new CValue(Distribution.getJson()));
          mType = ValueType::Value;
          mWritable = false;
          mValid = true;
          goto final;
        }

      CFactorOperation FactorOperation;
      FactorOperation.fromJSON(json);

      if (FactorOperation.isValid())
        {
          mpValue =  std::shared_ptr< CValue >(new CValue(FactorOperation.getJson()));
          mType = ValueType::Value;
          mWritable = false;
          mValid = true;
          goto final;
        }
    }

final:
  CLogger::popLevel();

  if (!mValid)
    {
      CLogger::error("CValueInstance::fromJSON: Invalid. {}", CSimConfig::jsonToString(json));
    }
}

CValueInterface CValueInstance::value(const CNode * pNode) const
{
  if (mpNodeProperty)
    return mpNodeProperty->propertyOf(pNode);

  return value();
}

CValueInterface CValueInstance::value(const CEdge * pEdge) const
{
  if (mpEdgeProperty)
    return mpEdgeProperty->propertyOf(pEdge);

  return value();
}

CValueInterface CValueInstance::value() const
{
  static CValueInterface Invalid(CValueInterface::Type::__SIZE, NULL);

  switch (mType)
    {
    case ValueType::Value:
      return *mpValue;
      break;

    case ValueType::Observable:
      return *mpObservable;
      break;

    case ValueType::HealthStateProperty:
      if (mpHealthStateProperty)
        return mpHealthStateProperty->propertyValue();

      break;

    case ValueType::TransmissionProperty:
      if (mpTransmissionsProperty)
        return mpTransmissionsProperty->propertyValue();

      break;

    case ValueType::ProgressionProperty:
      if (mpProgressionProperty)
        return mpProgressionProperty->propertyValue();

      break;
    case ValueType::Variable:
      // We need to make sure that the value of a global variable is up to date and we have the correct context.
      return mpVariable->toValue();
      break;

    case ValueType::SizeOf:
      return *mpSizeOf;
      break;

    default:
      break;
    }

  // pValueList is only available for Within and NotWithin which does not use this path

  return Invalid;
}

CValueList & CValueInstance::valueList() const
{
  static CValueList Invalid(CValueInterface::Type::__SIZE);

  if (mpValueList)
    return *mpValueList;

  return Invalid;
}


CValueInterface::Type CValueInstance::interfaceType() const
{
  switch (mType)
    {
    case ValueType::Value:
      if (mpValue)
        return mpValue->getType();

      break;

    case ValueType::ValueList:
      if (mpValueList)
        return mpValueList->getType();

      break;

    case ValueType::Observable:
      if (mpObservable)
        return mpObservable->getType();

      break;

    case ValueType::NodeProperty:
      if (mpNodeProperty)
        return mpNodeProperty->getType();

      break;

    case ValueType::EdgeProperty:
      if (mpEdgeProperty)
        return mpEdgeProperty->getType();

      break;

    case ValueType::HealthStateProperty:
      if (mpHealthStateProperty)
        return mpHealthStateProperty->getType();

      break;

    case ValueType::TransmissionProperty:
      if (mpTransmissionsProperty)
        return mpTransmissionsProperty->getType();

      break;

    case ValueType::ProgressionProperty:
      if (mpProgressionProperty)
        return mpProgressionProperty->getType();

      break;

    case ValueType::Variable:
      if (mpVariable)
        return mpVariable->getType();

      break;

    case ValueType::SizeOf:
      if (mpSizeOf)
        return mpSizeOf->getType();

      break;

    case ValueType::__SIZE:
      break;
    }

  return CValueInterface::Type::__SIZE;
}

bool CValueInstance::inherit() const
{
  return mpNodeProperty || mpEdgeProperty;
}

const bool & CValueInstance::isValid() const
{
  return mValid;
}

const CValueInstance::ValueType & CValueInstance::getType() const
{
  return mType;
}

CComputable * CValueInstance::getPrerequisite() const
{
  return mpPrerequisites;
}
    
CEdgeProperty * CValueInstance::edgeProperty() const
{
  return mpEdgeProperty.get();
}

CNodeProperty * CValueInstance::nodeProperty() const
{
  return mpNodeProperty.get();
}

CHealthStateProperty * CValueInstance::healthStateProperty() const
{
  return mpHealthStateProperty.get();
}

CTransmissionProperty * CValueInstance::transmissionProperty() const
{
  return mpTransmissionsProperty.get();
}

CProgressionProperty * CValueInstance::progressionProperty() const
{
  return mpProgressionProperty.get();
}

CVariable * CValueInstance::variable() const
{
  return mpVariable;
}

