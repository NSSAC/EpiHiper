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

#include <string>
#include <jansson.h>

#include "Distribution.h"

Distribution::Distribution()
  : mType(Type::__NONE)
  , mValues()
  , mArguments()
  , mValid(false)
{}

Distribution::Distribution(const Distribution & src)
  : mType(src.mType)
  , mValues(src.mValues)
  , mArguments(src.mArguments)
  , mValid(src.mValid)
{}

// virtual
Distribution::~Distribution()
{}

void Distribution::fromJSON(const json_t * json)
{
  /*
    {
      "required": ["fixed"]
    },
    {
      "required": ["discrete"]
    },
    {
      "required": ["uniform"]
    },
    {
      "required": ["normal"]
    }
  */
  mValid = true;

  json_t * pValue = json_object_get(json, "fixed");

  if (json_is_real(pValue))
    {
       mType = Type::fixed;
       mArguments.resize(1);
       mArguments[0] = json_real_value(pValue);

       mValid &= (mArguments[0] >= 0);
    }

  pValue = json_object_get(json, "discrete");

  if (json_is_array(pValue))
    {
       mType = Type::discrete;
       mValues.resize(json_array_size(pValue));
       std::vector< std::pair < double, double > >::iterator it = mValues.begin();
       std::vector< std::pair < double, double > >::iterator end = mValues.end();

       for (size_t i = 0; it != end; ++it, ++i)
         {
           json_t * pObject = json_array_get(pValue, i);

           if (json_is_object(pObject))
             {
               json_t * pReal = json_object_get(pObject, "probability");

               if (json_is_real(pReal))
                 {
                   it->first = json_real_value(pReal);
                   mValid &= (it->first >= 0);
                 }
               else
                 {
                   mValid = false;
                 }

              pReal = json_object_get(pObject, "value");

              if (json_is_real(pReal))
                {
                  it->second = json_real_value(pReal);
                  mValid &= (it->first >= 0);
                }
              else
                {
                  mValid = false;
                }
             }
         }
    }

  pValue = json_object_get(json, "uniform");

  if (json_is_array(pValue))
    {
       mType = Type::uniform;
       mArguments.resize(json_array_size(pValue));
       std::vector< double >::iterator it = mArguments.begin();
       std::vector< double >::iterator end = mArguments.end();

       for (size_t i = 0; it != end; ++it, ++i)
         {
           json_t * pReal = json_array_get(pValue, i);

           if (json_is_real(pReal))
             {
               *it = json_real_value(pReal);
               mValid &= (*it >= 0);
             }
           else
             {
               mValid = false;
             }
         }
    }
  else if (json_is_object(pValue))
    {
       mType = Type::uniform;
       mArguments.resize(2);

       json_t * pReal = json_object_get(pValue, "min");

       if (json_is_real(pReal))
         {
           mArguments[0] = json_real_value(pReal);
           mValid &= (mArguments[0] >= 0);
         }
       else
         {
           mValid = false;
         }

      pReal = json_object_get(pValue, "max");

      if (json_is_real(pReal))
        {
          mArguments[1] = json_real_value(pReal);
          mValid &= (mArguments[1] >= 0);
        }
      else
        {
          mValid = false;
        }
     }

  pValue = json_object_get(json, "normal");

  if (json_is_object(pValue))
    {
       mType = Type::normal;
       mArguments.resize(2);

       json_t * pReal = json_object_get(pValue, "mean");

       if (json_is_real(pReal))
         {
           mArguments[0] = json_real_value(pReal);
           mValid &= (mArguments[0] >= 0);
         }
       else
         {
           mValid = false;
         }

      pReal = json_object_get(pValue, "standardDeviation");

      if (json_is_real(pReal))
        {
          mArguments[1] = json_real_value(pReal);
          mValid &= (mArguments[1] >= 0);
        }
      else
        {
          mValid = false;
        }
    }

  mValid &= (mType != Type::__NONE);

}

const bool & Distribution::isValid() const
{
  return mValid;
}
