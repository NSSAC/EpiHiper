// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
// All rights reserved 
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

#include "network/CEdge.h"
#include "traits/CTrait.h"
#include "utilities/CMetadata.h"
#include "utilities/CLogger.h"

// static
CEdge CEdge::getDefault()
{
  CEdge Default;

  Default.targetId = -1;
  Default.targetActivity = CTrait::ActivityTrait->getDefault();
  Default.sourceId = -1;
  Default.sourceActivity = CTrait::ActivityTrait->getDefault();
  Default.duration = 0.0;
#ifdef USE_LOCATION_ID
  Default.locationId = -1;
#endif
  Default.edgeTrait = CTrait::EdgeTrait->getDefault();
  Default.active = true;
  Default.weight = 1.0;
  Default.pTarget = NULL;
  Default.pSource = NULL;

  return Default;
}

CEdge::CEdge()
  : targetId(-1)
  , targetActivity()
  , sourceId(-1)
  , sourceActivity()
  , duration(0.0)
#ifdef USE_LOCATION_ID
  , locationId(-1)
#endif
  , edgeTrait()
  , active(true)
  , weight(1.0)
  , pTarget(NULL)
  , pSource(NULL)
{}

CEdge::~CEdge()
{}

void CEdge::toBinary(std::ostream & os) const
{
#ifdef USE_LOCATION_ID
  if (CEdge::HasLocationId)
    os.write(reinterpret_cast< const char * >(&targetId), 64);
  else
    {
      os.write(reinterpret_cast< const char * >(&targetId), 40);
      os.write(reinterpret_cast< const char * >(&edgeTrait), 16);
    }
#else
  os.write(reinterpret_cast< const char * >(&targetId), 56);
#endif
  /*
  os.write(reinterpret_cast<const char *>(&targetId), sizeof(size_t));
  os.write(reinterpret_cast<const char *>(&targetActivity), sizeof(CTraitData::base));
  os.write(reinterpret_cast<const char *>(&sourceId), sizeof(size_t));
  os.write(reinterpret_cast<const char *>(&sourceActivity), sizeof(CTraitData::base));
  os.write(reinterpret_cast<const char *>(&duration), sizeof(double));
  os.write(reinterpret_cast<const char *>(&locationId), sizeof(size_t));
  os.write(reinterpret_cast<const char *>(&edgeTrait), sizeof(CTraitData::base));
  os.write(reinterpret_cast<const char *>(&active), sizeof(char));
  os.write(reinterpret_cast<const char *>(&weight), sizeof(double));
  */
}

void CEdge::fromBinary(std::istream & is)
{
#ifdef USE_LOCATION_ID
  if (CEdge::HasLocationId)
    is.read(reinterpret_cast< char * >(&targetId), 64);
  else
    {
      is.read(reinterpret_cast< char * >(&targetId), 40);
      is.read(reinterpret_cast< char * >(&edgeTrait), 16);
    }
#else
  is.read(reinterpret_cast< char * >(&targetId), 56);
#endif

  /*
  is.read(reinterpret_cast<char *>(&targetId), sizeof(size_t));
  is.read(reinterpret_cast<char *>(&targetActivity), sizeof(CTraitData::base));
  is.read(reinterpret_cast<char *>(&sourceId), sizeof(size_t));
  is.read(reinterpret_cast<char *>(&sourceActivity), sizeof(CTraitData::base));
  is.read(reinterpret_cast<char *>(&duration), sizeof(double));
  is.read(reinterpret_cast<char *>(&locationId), sizeof(size_t));
  is.read(reinterpret_cast<char *>(&edgeTrait), sizeof(CTraitData::base));
  is.read(reinterpret_cast<char *>(&active), sizeof(char));
  is.read(reinterpret_cast<char *>(&weight), sizeof(double));
  */
}

bool CEdge::setTargetActivity(CTraitData::value value, CValueInterface::pOperator pOperator, const CMetadata & metadata)
{
  CLogger::trace() << "CEdge [ActionDefinition:"
                   << (metadata.contains("CActionDefinition") ? metadata.getInt("CActionDefinition") : -1)
                   << "]: Edge ("
                   << targetId
                   << ", "
                   << sourceId
                   << ") targetActivity "
                   << CValueInterface::operatorToString(pOperator)
                   << " "
                   << CTrait::ActivityTrait->toString(value);
  CTraitData::setValue(targetActivity, value);
  return true;
}

bool CEdge::setSourceActivity(CTraitData::value value, CValueInterface::pOperator pOperator, const CMetadata & metadata)
{
  CLogger::trace() << "CEdge [ActionDefinition:"
                   << (metadata.contains("CActionDefinition") ? metadata.getInt("CActionDefinition") : -1)
                   << "]: Edge ("
                   << targetId
                   << ", "
                   << sourceId
                   << ") sourceActivity "
                   << CValueInterface::operatorToString(pOperator)
                   << " "
                   << CTrait::ActivityTrait->toString(value);
  CTraitData::setValue(sourceActivity, value);
  return true;
}

bool CEdge::setEdgeTrait(CTraitData::value value, CValueInterface::pOperator pOperator, const CMetadata & metadata)
{
  CLogger::trace() << "CEdge [ActionDefinition:"
                   << (metadata.contains("CActionDefinition") ? metadata.getInt("CActionDefinition") : -1)
                   << "]: Edge ("
                   << targetId
                   << ", "
                   << sourceId
                   << ") edgeTrait "
                   << CValueInterface::operatorToString(pOperator)
                   << " "
                   << CTrait::EdgeTrait->toString(value);
  CTraitData::setValue(edgeTrait, value);
  return true;
}

bool CEdge::setActive(bool value, CValueInterface::pOperator pOperator, const CMetadata & metadata)
{
  CLogger::trace() << "CEdge [ActionDefinition:"
                   << (metadata.contains("CActionDefinition") ? metadata.getInt("CActionDefinition") : -1)
                   << "]: Edge ("
                   << targetId
                   << ", "
                   << sourceId
                   << ") active "
                   << CValueInterface::operatorToString(pOperator)
                   << " "
                   << (value ? "true" : "false");
  active = value;
  return true;
}

bool CEdge::setWeight(double value, CValueInterface::pOperator pOperator, const CMetadata & metadata)
{
  CLogger::trace() << "CEdge [ActionDefinition:"
                   << (metadata.contains("CActionDefinition") ? metadata.getInt("CActionDefinition") : -1)
                   << "]: Edge ("
                   << targetId
                   << ", "
                   << sourceId
                   << ") weight "
                   << CValueInterface::operatorToString(pOperator)
                   << " "
                   << value;
  (*pOperator)(weight, value);

  return true;
}
