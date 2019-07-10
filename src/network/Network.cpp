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

#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdio>
#include <jansson.h>

#include "traits/Trait.h"
#include "SimConfig.h"

#include "Network.h"

#include "../utilities/Communicate.h"
#include "Node.h"
#include "Edge.h"

// static
Network * Network::INSTANCE(NULL);

// static
void Network::init()
{
  if (INSTANCE == NULL)
    {
      INSTANCE = new Network(SimConfig::getContactNetwork());
    }
}

// static
void Network::release()
{
  if (INSTANCE != NULL)
    {
      delete INSTANCE;
      INSTANCE = NULL;
    }
}

Network::Network(const std::string & networkFile)
  : Annotation()
  , mFile(networkFile)
  , mLocalNodes(NULL)
  , mFirstLocalNode(0)
  , mBeyondLocalNode(0)
  , mLocalNodesSize(0)
  , mRemoteNodes()
  , mEdges(NULL)
  , mEdgesSize(0)
  , mTotalNodesSize(0)
  , mTotalEdgesSize(0)
  , mSizeOfPid(0)
  , mAccumulationTime()
  , mTimeResolution(0)
  , mIsBinary(false)
  , mValid(false)
  , mpJson(NULL)
{
  if (mFile.empty())
    {
      std::cerr << "Network file is not specified" << std::endl;
      return;
    }

  std::ifstream is(mFile.c_str());

  if (is.fail())
    {
      std::cerr << "Network file: '" << mFile << "' cannot be opened." << std::endl;
      return;
    }

  std::string Line;
  std::getline(is, Line);

  json_error_t error;

  mpJson = json_loads(Line.c_str(), 0, &error);

  if (mpJson == NULL)
    {
      std::cerr << "Network file: '" << mFile << "' error on line " << error.line << ": " << error.text << std::endl;;
    }

  fromJSON(mpJson);

  if (mValid)
    {
      // Skip Column Header
      std::getline(is, Line);

      partition(is);
    }

  is.close();
}

// virtual
Network::~Network()
{
  if (mLocalNodes != NULL)
    {
      delete [] mLocalNodes;
      mLocalNodes = NULL;
    }

  if (mEdges != NULL)
    {
      delete [] mEdges;
      mEdges = NULL;
    }

  if (mpJson != NULL)
    {
      json_decref(mpJson);
    }
}

void Network::fromJSON(const json_t * json)
{
  mValid = true;

  json_t * pValue = json_object_get(json, "encoding");

  if (json_is_string(pValue))
    {
      std::string Encoding = json_string_value(pValue);
      mIsBinary = (Encoding != "text");
    }

  pValue = json_object_get(json, "numberOfNodes");

  if (json_is_integer(pValue))
    {
      mTotalNodesSize = json_integer_value(pValue);
    }

  mValid &= (mTotalNodesSize > 0);

  pValue = json_object_get(json, "numberOfEdges");

  if (json_is_integer(pValue))
    {
      mTotalEdgesSize = json_integer_value(pValue);
    }

  mValid &= (mTotalEdgesSize > 0);

  pValue = json_object_get(json, "sizeofPID");

  if (json_is_integer(pValue))
    {
      mSizeOfPid = json_integer_value(pValue);
    }

  mValid &= (mSizeOfPid == 4 || mSizeOfPid == 8);

  pValue = json_object_get(json, "accumulationTime");

  if (json_is_string(pValue))
    {
      mAccumulationTime = json_string_value(pValue);
    }

  pValue = json_object_get(json, "timeResolution");

  if (json_is_integer(pValue))
    {
      mTimeResolution = json_integer_value(pValue);
      mValid &= (mTimeResolution > 0);
    }

  size_t Size = 0;

  pValue = json_object_get(json, "sizeofActivity");

  if (json_is_integer(pValue))
    {
      Size = json_integer_value(pValue);
    }

  mValid &= (Size == 4);

  pValue = json_object_get(json, "activityEncoding");

  if (json_is_object(pValue))
    {
      Trait Trait;
      Trait.fromJSON(pValue);

      if (Trait.isValid())
        {
          Trait::INSTANCES[Trait.getId()] = Trait;
        }
    }

  mValid &= Trait::ActivityTrait->isValid();

  pValue = json_object_get(json, "sizeofEdgeTrait");

  if (json_is_integer(pValue))
    {
      Size = json_integer_value(pValue);
    }

  mValid &= (Size == 4 || Size == 0);

  pValue = json_object_get(json, "edgeTraitEncoding");

  if (json_is_object(pValue))
    {
      Trait Trait;
      Trait.fromJSON(pValue);

      if (Trait.isValid())
        {
          Trait::INSTANCES[Trait.getId()] = Trait;
          Edge::HasEdgeTrait = (Size == 4);
        }
    }

  mValid &= Size == 0 || Trait::EdgeTrait->isValid();

  pValue = json_object_get(json, "hasActiveField");

  if (json_is_boolean(pValue))
    {
      Edge::HasActiveField = json_boolean_value(pValue);
    }

  pValue = json_object_get(json, "hasWeightField");

  if (json_is_boolean(pValue))
    {
      Edge::HasWeightField = json_boolean_value(pValue);
    }

  Annotation::fromJSON(json);
}

void Network::partition(std::istream & is)
{
  // Communicate::Processes = 8;

  size_t Partition[Communicate::Processes * 3 + 1];

  if (Communicate::Rank == 0)
    {
      size_t * pPartition = Partition;
      size_t * pNodes = pPartition + 1;
      size_t * pEdges = pPartition + 2;

      size_t DesiredPerComputeNode = mTotalEdgesSize / Communicate::Processes + 1;

      size_t Node(-1);
      size_t LastNode(-1);
      size_t CurrentNode(-1);
      size_t PartitionEdgeCount(0);
      size_t LastNodeEdgeCount(0);

      size_t PartitionIndex(1);

      EdgeData Edge = Edge::getDefault();

      while (is.good() && loadEdge(&Edge, is))
        {
          // std::cout << Edge.targetId << ", " << Edge.targetActivity << ", " << Edge.sourceId << ", " << Edge.sourceActivity << ", " << Edge.duration << std::endl;
          Node = Edge.targetId;

          if (PartitionEdgeCount == 0)
            {
              *pPartition = Node;
              *pNodes = 0;
              *pEdges = 0;
            }

          if (CurrentNode != Node)
            {
              if (PartitionEdgeCount >= PartitionIndex * DesiredPerComputeNode)
                {
                  if (2 * PartitionIndex * DesiredPerComputeNode <  LastNodeEdgeCount + PartitionEdgeCount)
                    {
                      *pNodes -= 1;
                      *pEdges -= PartitionEdgeCount - LastNodeEdgeCount;

                      *(pPartition + 3) = CurrentNode;
                      *(pNodes + 3) = 1;
                      *(pEdges + 3) = PartitionEdgeCount - LastNodeEdgeCount;
                    }
                  else
                    {
                      *(pPartition + 3) = Node;
                      *(pNodes + 3) = 0;
                      *(pEdges + 3) = 0;
                    }

                  pPartition += 3;
                  pNodes += 3;
                  pEdges += 3;
                  ++PartitionIndex;
                }

              ++*pNodes;

              LastNode = CurrentNode;
              LastNodeEdgeCount = PartitionEdgeCount;

              CurrentNode = Node;
            }

          ++*pEdges;
          ++PartitionEdgeCount;
        }

      *(pPartition + 3)= Node + 1;
    }

  Communicate::broadcast(Partition, Communicate::Processes * 3 + 1, MPI_UINT64_T, 0);

  size_t * pMine = Partition + 3 * Communicate::Rank;
  mFirstLocalNode = *pMine++;
  mLocalNodesSize = *pMine++;
  mEdgesSize = *pMine++;
  mBeyondLocalNode = *pMine;

  mLocalNodes = new NodeData[mLocalNodesSize];
  mEdges = new EdgeData[mEdgesSize];
}

void Network::load()
{
  if (!mValid) return;

  if (mFile.empty())
    {
      std::cerr << "Network file is not specified" << std::endl;
      mValid = false;
      return;
    }

  std::ifstream is(mFile.c_str());

  if (is.fail())
    {
      std::cerr << "Network file: '" << mFile << "' cannot be opened." << std::endl;
      mValid = false;
      return;
    }

  std::string Line;

  // Skip JSON Prolog;
  std::getline(is, Line);

  // Skip Column Header;
  std::getline(is, Line);

  NodeData * pNode = mLocalNodes;
  NodeData * pNodeEnd = pNode + mLocalNodesSize;
  NodeData DefaultNode = Node::getDefault();
  *pNode = DefaultNode;

  EdgeData * pEdge = mEdges;
  EdgeData * pEdgeEnd = pEdge + mEdgesSize;
  EdgeData DefaultEdge = Edge::getDefault();
  *pEdge = DefaultEdge;

  bool FirstTime = true;

  while (is.good() && pNode < pNodeEnd)
    {
      if (!loadEdge(pEdge, is))
        {
          std::cerr << "Network file: '" << mFile << "' invalid edge (" << pEdge - mEdges << ")." << std::endl;
          mValid = false;
          break;
        }

      if (pEdge->targetId < mFirstLocalNode) continue;

      if (FirstTime)
        {
          pNode->id = pEdge->targetId;
          pNode->Edges = pEdge;
          FirstTime = false;
        }

      if (pNode->id != pEdge->targetId)
        {
          pNode->EdgesSize = pEdge - pNode->Edges;

          ++pNode;
          *pNode = DefaultNode;
          pNode->id = pEdge->targetId;
          pNode->Edges = pEdge;
        }

      pEdge->pTarget = pNode;

      if (pEdge->sourceId < this->mFirstLocalNode || this->mBeyondLocalNode <= pEdge->sourceId)
        {
          NodeData Remote = DefaultNode;
          Remote.id = (pEdge->sourceId);

          pEdge->pSource = &mRemoteNodes.insert(std::make_pair(pEdge->sourceId, Remote)).first->second;
        }

      ++pEdge;

      if (pEdge == pEdgeEnd) break;

      *pEdge = DefaultEdge;
    }

  is.close();

  if (mValid)
    {
      pNode->EdgesSize = pEdge - pNode->Edges;

      for (pEdge = mEdges; pEdge != pEdgeEnd; ++pEdge)
        if (pEdge->pSource == NULL)
          {
            pEdge->pSource = lookupNode(pEdge->sourceId);
          }
    }
}

void Network::write(const std::string & file, bool binary)
{
  mIsBinary = binary;

  json_t * pValue = json_object_get(mpJson, "encoding");

  if (json_is_string(pValue))
    {
      json_string_set(pValue, mIsBinary ? "binary" : "text");
    }

  std::ofstream os(file.c_str());
  os << json_dumps(mpJson, JSON_COMPACT | JSON_INDENT(0)) << std::endl;
  os << "targetPID,targetActivity,sourcePID,sourceActivity,duration";

  if (Edge::HasEdgeTrait) os << ",edgeTrait";

  if (Edge::HasActiveField) os << ",active";

  if (Edge::HasWeightField) os << ",weight";

  os << std::endl;

  EdgeData * pEdge = mEdges;
  EdgeData * pEdgeEnd = pEdge + mEdgesSize;

  for (; pEdge != pEdgeEnd; ++pEdge)
    {
      writeEdge(pEdge, os);
    }

  os.close();
}

NodeData * Network::beginNode()
{
  return mLocalNodes;
}

NodeData * Network::endNode()
{
  return mLocalNodes + mLocalNodesSize;
}

NodeData * Network::lookupNode(const size_t & id) const
{
  static double stretch = 0.0;

  if (stretch == 0.0)
    {
      stretch = ((double) mLocalNodesSize)/(mBeyondLocalNode - mFirstLocalNode);
    }

  if (id < mFirstLocalNode || mBeyondLocalNode <= id)
    {
      std::map< size_t, NodeData >::const_iterator found = mRemoteNodes.find(id);

      if (found != mRemoteNodes.end())
        {
          return const_cast< NodeData *>(&found->second);
        }

      return NULL;
    }

  NodeData * pLeft = mLocalNodes;
  NodeData * pRight = mLocalNodes + mLocalNodesSize - 1;
  NodeData * pCurrent = mLocalNodes + std::min< size_t >(std::round(stretch * (id - mFirstLocalNode)), mLocalNodesSize - 1);

  while (pCurrent->id != id)
    {
      // Handle invalid requests
      if (pRight - pLeft < 2) return NULL;

      if (pCurrent->id < id)
        {
          pLeft = pCurrent;
          pCurrent = pLeft + (id - pLeft->id);

          // Assure that the interval shrinks
          if(pRight <= pCurrent)
            pCurrent = pLeft + (pRight - pLeft)/2;
        }
      else
        {
          pRight = pCurrent;
          pCurrent = pRight - (pRight->id - id);

          // Assure that the interval shrinks
          if(pCurrent <= pLeft)
            pCurrent = pLeft + (pRight - pLeft)/2;
        }
    }

  return pCurrent;
}

EdgeData * Network::lookupEdge(const size_t & targetId, const size_t & sourceId) const
{
  // We only have edges for local target nodes
  if (targetId < mFirstLocalNode || mBeyondLocalNode <= targetId) return NULL;

  NodeData * pTargetNode = lookupNode(targetId);

  // Handle invalid requests
  if (pTargetNode == NULL) return NULL;

  EdgeData * pLeft = pTargetNode->Edges;
  EdgeData * pRight = pLeft + pTargetNode->EdgesSize;
  EdgeData * pCurrent = pLeft + (pRight - pLeft)/2;

  while (pCurrent->sourceId != sourceId)
    {
      // Handle invalid requests
      if (pRight - pLeft < 2) return NULL;

      if (pCurrent->sourceId < sourceId)
        {
          pLeft = pCurrent + 1;
        }
      else
        {
          pRight = pCurrent;
        }

      pCurrent = pLeft + (pRight - pLeft)/2;
    }

  return pCurrent;
}

bool Network::loadEdge(EdgeData * pEdge, std::istream & is) const
{
  bool success = true;

  if (mIsBinary)
    {
      Edge::fromBinary(is, pEdge);

      success = is.good() &&
              (mFirstLocalNode == 0 ||
               (mFirstLocalNode <= pEdge->targetId && pEdge->targetId < mBeyondLocalNode));
    }
  else
    {
      std::string Line;
      std::getline(is, Line);

      const char * ptr = Line.c_str();
      int Read;

      static char targetActivity[128];
      static char sourceActivity[128];
      static char edgeTrait[128];

      if (5 != sscanf(ptr, "%zu,%[^,],%zu,%[^,],%lf%n", &pEdge->targetId, targetActivity, &pEdge->sourceId, sourceActivity, &pEdge->duration, &Read))
        {
          success = false;
        }

      Trait::ActivityTrait->fromString(targetActivity, pEdge->targetActivity);
      Trait::ActivityTrait->fromString(sourceActivity, pEdge->sourceActivity);
      ptr += Read;

      if (Edge::HasEdgeTrait)
        {
          if (1 != sscanf(ptr, ",%[^,]%n", edgeTrait, &Read))
            {
              success = false;
            }

          Trait::EdgeTrait->fromString(edgeTrait, pEdge->edgeTrait);
          ptr += Read;
        }

      if (Edge::HasActiveField)
        {
          char Active;

          if (1 != sscanf(ptr, ",%c%n", &Active, &Read))
            {
              success = false;
            }

          pEdge->active = (Active == '1');
          ptr += Read;
        }

      if (Edge::HasWeightField)
        {
          if (1 != sscanf(ptr, ",%lf%n", &pEdge->weight, &Read))
            {
              success = false;
            }

          ptr += Read;
        }

      if (success)
        success = (*ptr == 0);
    }

  return success;
}

void Network::writeEdge(EdgeData * pEdge, std::ostream & os) const
{
  if (mIsBinary)
    {
      Edge::toBinary(os, pEdge);
    }
  else
    {
      os << pEdge->targetId;
      os << "," << Trait::ActivityTrait->toString(pEdge->targetActivity);
      os << "," << pEdge->sourceId;
      os << "," << Trait::ActivityTrait->toString(pEdge->sourceActivity);
      os << "," << pEdge->duration;

      if (Edge::HasEdgeTrait)
        {
          os << "," << Trait::EdgeTrait->toString(pEdge->edgeTrait);
        }

      if (Edge::HasActiveField)
        {
          os << "," << pEdge->active;
        }

      if (Edge::HasWeightField)
        {
          os << "," << pEdge->weight;
        }

      os << std::endl;
    }
}


const bool & Network::isValid() const
{
  return mValid;
}

