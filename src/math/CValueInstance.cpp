#include <jansson.h>

#include "utilities/CLogger.h"
#include "math/CValueInstance.h"
#include "math/CObservable.h"
#include "math/CSizeOf.h"
#include "variables/CVariable.h"
#include "variables/CVariableList.h"
#include "utilities/CSimConfig.h"

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

  CLogger::pushLevel(spdlog::level::off);

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
          mType = ValueType::Sizeof;
          mpPrerequisites = mpSizeOf.get();
          mWritable = false;
          mValid = true;
          goto final;
        }

      mpSizeOf.reset();
    }

final:
  CLogger::popLevel();

  if (!mValid)
    {
      CLogger::error() << "CValueInstance::fromJSON: Invalid. " << CSimConfig::jsonToString(json);
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

    case ValueType::Variable:
      // We need to make sure that the value of a global variable is up to date and we have the correct context.
      return mpVariable->toValue();
      break;

    case ValueType::Sizeof:
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

    case ValueType::Variable:
      if (mpVariable)
        return mpVariable->getType();

      break;

    case ValueType::Sizeof:
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
    
