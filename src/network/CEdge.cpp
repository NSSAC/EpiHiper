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

/*
 * Edge.cpp
 *
 *  Created on: Jun 26, 2019
 *      Author: shoops
 */

#include "network/CEdge.h"
#include "traits/CTrait.h"

// static
bool CEdge::HasEdgeTrait(false);

// static
bool CEdge::HasActiveField(false);

// static
bool CEdge::HasWeightField(false);

// static
CEdge CEdge::getDefault()
{
  CEdge Default;

  Default.targetId = -1;
  Default.targetActivity = CTrait::ActivityTrait->getDefault();
  Default.sourceId = -1;
  Default.sourceActivity = CTrait::ActivityTrait->getDefault();
  Default.duration = 0.0;
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
  , edgeTrait()
  , active(true)
  , weight(1.0)
  , pTarget(NULL)
  , pSource(NULL)
{}

CEdge::CEdge(const CEdge & src)
  : targetId(src.targetId)
  , targetActivity(src.targetActivity)
  , sourceId(src.sourceId)
  , sourceActivity(src.sourceActivity)
  , duration(src.duration)
  , edgeTrait(src.edgeTrait)
  , active(src.active)
  , weight(src.weight)
  , pTarget(src.pTarget)
  , pSource(src.pSource)
{}

CEdge::~CEdge()
{}

void CEdge::toBinary(std::ostream & os) const
{
  os.write(reinterpret_cast<const char *>(&targetId), sizeof(size_t));
  os.write(reinterpret_cast<const char *>(&targetActivity), sizeof(CTraitData::base));
  os.write(reinterpret_cast<const char *>(&sourceId), sizeof(size_t));
  os.write(reinterpret_cast<const char *>(&sourceActivity), sizeof(CTraitData::base));
  os.write(reinterpret_cast<const char *>(&duration), sizeof(double));

  if (CEdge::HasEdgeTrait)
    {
      os.write(reinterpret_cast<const char *>(&edgeTrait), sizeof(CTraitData::base));
    }

  if (CEdge::HasActiveField)
    {
      os.write(reinterpret_cast<const char *>(&active), sizeof(char));
    }

  if (CEdge::HasWeightField)
    {
      os.write(reinterpret_cast<const char *>(&weight), sizeof(double));
    }
}

void CEdge::fromBinary(std::istream & is)
{
  is.read(reinterpret_cast<char *>(&targetId), sizeof(size_t));
  is.read(reinterpret_cast<char *>(&targetActivity), sizeof(CTraitData::base));
  is.read(reinterpret_cast<char *>(&sourceId), sizeof(size_t));
  is.read(reinterpret_cast<char *>(&sourceActivity), sizeof(CTraitData::base));
  is.read(reinterpret_cast<char *>(&duration), sizeof(double));

  if (CEdge::HasEdgeTrait)
    {
      is.read(reinterpret_cast<char *>(&edgeTrait), sizeof(CTraitData::base));
    }

  if (CEdge::HasActiveField)
    {
      is.read(reinterpret_cast<char *>(&active), sizeof(char));
    }

  if (CEdge::HasWeightField)
    {
      is.read(reinterpret_cast<char *>(&weight), sizeof(double));
    }
}
