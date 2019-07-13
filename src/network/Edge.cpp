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

#include "traits/Trait.h"
#include "network/Edge.h"

// static
bool Edge::HasEdgeTrait(false);

// static
bool Edge::HasActiveField(false);

// static
bool Edge::HasWeightField(false);

EdgeData::EdgeData()
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

// static
void Edge::toBinary(std::ostream & os, const EdgeData * pEdge)
{
  os.write(reinterpret_cast<const char *>(&pEdge->targetId), sizeof(size_t));
  os.write(reinterpret_cast<const char *>(&pEdge->targetActivity), sizeof(TraitData::base));
  os.write(reinterpret_cast<const char *>(&pEdge->sourceId), sizeof(size_t));
  os.write(reinterpret_cast<const char *>(&pEdge->sourceActivity), sizeof(TraitData::base));
  os.write(reinterpret_cast<const char *>(&pEdge->duration), sizeof(double));

  if (Edge::HasEdgeTrait)
    {
      os.write(reinterpret_cast<const char *>(&pEdge->edgeTrait), sizeof(TraitData::base));
    }

  if (Edge::HasActiveField)
    {
      os.write(reinterpret_cast<const char *>(&pEdge->active), sizeof(char));
    }

  if (Edge::HasWeightField)
    {
      os.write(reinterpret_cast<const char *>(&pEdge->weight), sizeof(double));
    }
}

// static
void Edge::fromBinary(std::istream & is, EdgeData * pEdge)
{
  is.read(reinterpret_cast<char *>(&pEdge->targetId), sizeof(size_t));
  is.read(reinterpret_cast<char *>(&pEdge->targetActivity), sizeof(TraitData::base));
  is.read(reinterpret_cast<char *>(&pEdge->sourceId), sizeof(size_t));
  is.read(reinterpret_cast<char *>(&pEdge->sourceActivity), sizeof(TraitData::base));
  is.read(reinterpret_cast<char *>(&pEdge->duration), sizeof(double));

  if (Edge::HasEdgeTrait)
    {
      is.read(reinterpret_cast<char *>(&pEdge->edgeTrait), sizeof(TraitData::base));
    }

  if (Edge::HasActiveField)
    {
      is.read(reinterpret_cast<char *>(&pEdge->active), sizeof(char));
    }

  if (Edge::HasWeightField)
    {
      is.read(reinterpret_cast<char *>(&pEdge->weight), sizeof(double));
    }
}

// static
EdgeData Edge::getDefault()
{
  EdgeData Default;

  Default.targetId = -1;
  Default.targetActivity = Trait::ActivityTrait->getDefault();
  Default.sourceId = -1;
  Default.sourceActivity = Trait::ActivityTrait->getDefault();
  Default.duration = 0.0;
  Default.edgeTrait = Trait::EdgeTrait->getDefault();
  Default.active = true;
  Default.weight = 1.0;
  Default.pTarget = NULL;
  Default.pSource = NULL;

  return Default;
}

Edge::Edge(EdgeData * pData)
  : mpData(pData)
{}

Edge::Edge(const Edge & src)
  : mpData(src.mpData)
{}

Edge::~Edge()
{}

void Edge::toBinary(std::ostream & os) const
{
  toBinary(os, mpData);
}

void Edge::fromBinary(std::istream & is)
{
  fromBinary(is, mpData);
}


