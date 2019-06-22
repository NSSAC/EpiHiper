#include <jansson.h>

#include "Enum.h"

Enum::Enum()
  : Annotation()
  , mId()
  , mMask()
  , mValid(false)
{}

Enum::Enum(const Enum & src)
  : Annotation(src)
  , mId(src.mId)
  , mMask(src.mMask)
  , mValid(src.mValid)
{}


Enum::~Enum()
{}

void Enum::fromJSON(const json_t * json)
{
  json_t * pValue = json_object_get(json, "id");

  if (json_is_string(pValue))
    {
      mId = json_string_value(pValue);
      mAnnId = mId;
    }

  mValid = !mId.empty();

  Annotation::fromJSON(json);
}

const std::string & Enum::getId() const
{
  return mId;
}

const bool & Enum::isValid() const
{
  return mValid;
}

void Enum::setMask(const TraitData & mask)
{
  mMask = mask;
}

const TraitData & Enum::getMask() const
{
  return mMask;
}

