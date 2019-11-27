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
#include <sstream>
#include <iostream>
#include <vector>
#include <cstdio>
#include <cstring>
#include <jansson.h>

#include "actions/CActionQueue.h"
#include "traits/CTrait.h"
#include "utilities/CCommunicate.h"
#include "utilities/CStreamBuffer.h"
#include "actions/Changes.h"
#include "network/CEdge.h"
#include "network/CNetwork.h"

#include "utilities/CSimConfig.h"
#include "utilities/CDirEntry.h"
#include "network/CNode.h"

// static
void CNetwork::init()
{
  if (INSTANCE == NULL)
    {
      INSTANCE = new CNetwork(CSimConfig::getContactNetwork());
      INSTANCE->partition(CCommunicate::MPIProcesses, false);
    }
}

// static
void CNetwork::release()
{
  if (INSTANCE != NULL)
    {
      delete INSTANCE;
      INSTANCE = NULL;
    }
}

CNetwork::CNetwork(const std::string & networkFile)
  : CAnnotation()
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
  , mTotalPendingActions(0)
  , mpJson(NULL)
{
  if (mFile.empty())
    {
      std::cerr << "Network file is not specified" << std::endl;
      return;
    }

  mpJson = CSimConfig::loadJson(mFile, 0);

  fromJSON(mpJson);
}

// virtual
CNetwork::~CNetwork()
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

void CNetwork::fromJSON(const json_t * json)
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
      CTrait Trait;
      Trait.fromJSON(pValue);

      if (Trait.isValid())
        {
          CTrait::INSTANCES[Trait.getId()] = Trait;
        }
    }

  mValid &= CTrait::ActivityTrait->isValid();

  pValue = json_object_get(json, "sizeofEdgeTrait");

  if (json_is_integer(pValue))
    {
      Size = json_integer_value(pValue);
    }

  mValid &= (Size == 4 || Size == 0);

  pValue = json_object_get(json, "edgeTraitEncoding");

  if (json_is_object(pValue))
    {
      CTrait Trait;
      Trait.fromJSON(pValue);

      if (Trait.isValid())
        {
          CTrait::INSTANCES[Trait.getId()] = Trait;
          CEdge::HasEdgeTrait = (Size == 4);
        }
    }

  mValid &= Size == 0 || CTrait::EdgeTrait->isValid();

  pValue = json_object_get(json, "hasActiveField");

  if (json_is_boolean(pValue))
    {
      CEdge::HasActiveField = json_boolean_value(pValue);
    }

  pValue = json_object_get(json, "hasWeightField");

  if (json_is_boolean(pValue))
    {
      CEdge::HasWeightField = json_boolean_value(pValue);
    }

  CAnnotation::fromJSON(json);
}

void CNetwork::partition(const int & parts, const bool & save, const std::string & outputDirectory)
{
  if (!mValid)
    {
      return;
    }

  if (haveValidPartition(parts))
    {
      return;
    }

  std::ifstream is(mFile.c_str());
  std::string Line;
  // Skip JSON Header
  std::getline(is, Line);
  // Skip Column Header
  std::getline(is, Line);

  partition(is, parts, save, outputDirectory);

  is.close();
}

void CNetwork::partition(std::istream & is, const int & parts, const bool & save, const std::string & outputDirectory)
{
  // Communicate::Processes = 8;

  size_t Partition[parts * 3 + 1];

  if (CCommunicate::MPIRank == 0)
    {
      std::ostringstream NodeBuffer;
      std::ostringstream PartitionBuffer;

      size_t * pPartition = Partition;
      size_t * pNodes = pPartition + 1;
      size_t * pEdges = pPartition + 2;

      size_t DesiredPerComputeNode = mTotalEdgesSize / parts + 1;

      size_t Node(-1);
      size_t LastNode(-1);
      size_t CurrentNode(-1);
      size_t PartitionEdgeCount(0);
      size_t LastNodeEdgeCount(0);

      size_t PartitionIndex(1);

      CEdge Edge = CEdge::getDefault();

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

                      // Write partition
                      if (save)
                        {
                          writePartition(PartitionIndex, parts, *pNodes, *pPartition, *(pPartition + 3), *pEdges, PartitionBuffer.str(), outputDirectory);
                          PartitionBuffer.str("");
                        }
                    }
                  else
                    {
                      *(pPartition + 3) = Node;
                      *(pNodes + 3) = 0;
                      *(pEdges + 3) = 0;

                      if (save)
                        {
                          // Write current node buffer to partition buffer
                          PartitionBuffer << NodeBuffer.str();
                          NodeBuffer.str("");

                          // Write partition
                          writePartition(PartitionIndex, parts, *pNodes, *pPartition, *(pPartition + 3), *pEdges, PartitionBuffer.str(), outputDirectory);
                          PartitionBuffer.str("");
                        }
                    }

                  pPartition += 3;
                  pNodes += 3;
                  pEdges += 3;
                  ++PartitionIndex;
                }


              if (save)
                {
                  // Write current node buffer to partition buffer
                  PartitionBuffer << NodeBuffer.str();
                  NodeBuffer.str("");
                }

              ++*pNodes;

              LastNode = CurrentNode;
              LastNodeEdgeCount = PartitionEdgeCount;

              CurrentNode = Node;
            }

          Edge.toBinary(NodeBuffer);

          ++*pEdges;

          // Write edge to current node buffer
          ++PartitionEdgeCount;
        }

      *(pPartition + 3) = Node + 1;

      if (save)
        {
          // Write current node buffer to partition buffer
          PartitionBuffer << NodeBuffer.str();
          NodeBuffer.str("");

          // Write partition
          writePartition(PartitionIndex, parts, *pNodes, *pPartition, *(pPartition + 3), *pEdges, PartitionBuffer.str(), outputDirectory);
          PartitionBuffer.str("");
        }
    }

  CCommunicate::broadcast(Partition, parts * 3 + 1, MPI_UINT64_T, 0);

  size_t * pMine = Partition + 3 * CCommunicate::MPIRank;
  mFirstLocalNode = *pMine++;
  mLocalNodesSize = *pMine++;
  mEdgesSize = *pMine++;
  mBeyondLocalNode = *pMine;
}

void CNetwork::load()
{
  if (!mValid) return;

  if (mFile.empty())
    {
      std::cerr << "Network file is not specified" << std::endl;
      mValid = false;
      return;
    }

  std::ostringstream File;
  File << mFile << "." << CCommunicate::MPIRank;
  bool havePartition = CDirEntry::isFile(File.str());

  std::ifstream is;

  if (havePartition)
    {
      json_t * pJson = CSimConfig::loadJson(File.str(), 0);
      json_t * pPartition = json_object_get(pJson, "partition");
      json_t * pValue = json_object_get(pPartition, "numberOfNodes");

      if (json_is_integer(pValue))
        {
          mLocalNodesSize = json_integer_value(pValue);
        }

      pValue = json_object_get(pPartition, "firstLocalNode");

      if (json_is_integer(pValue))
        {
          mFirstLocalNode = json_integer_value(pValue);
        }

      pValue = json_object_get(pPartition, "beyondLocalNode");

      if (json_is_integer(pValue))
        {
          mBeyondLocalNode = json_integer_value(pValue);
        }

      pValue = json_object_get(pPartition, "numberOfEdges");

      if (json_is_integer(pValue))
        {
          mEdgesSize = json_integer_value(pValue);
        }

      pValue = json_object_get(pJson, "encoding");

      if (json_is_string(pValue))
        {
          std::string Encoding = json_string_value(pValue);
          mIsBinary = (Encoding != "text");
        }

      json_decref(pJson);
    }
  else
    {
      File.str(mFile);
    }

  // std::cout << Communicate::Rank << ": " << mFirstLocalNode << ", " << mBeyondLocalNode << ", " << mLocalNodesSize << ", " << mEdgesSize << std::endl;
  mLocalNodes = new CNode[mLocalNodesSize];
  mEdges = new CEdge[mEdgesSize];


  is.open(File.str().c_str());

  if (is.fail())
    {
      std::cerr << "Network file: '" << File.str() << "' cannot be opened." << std::endl;
      mValid = false;
      return;
    }

  std::string Line;

  // Skip JSON Header
  std::getline(is, Line);
  // Skip Column Header
  std::getline(is, Line);

  CNode * pNode = mLocalNodes;
  CNode * pNodeEnd = pNode + mLocalNodesSize;
  CNode DefaultNode = CNode::getDefault();
  *pNode = DefaultNode;

  CEdge * pEdge = mEdges;
  CEdge * pEdgeEnd = pEdge + mEdgesSize;
  CEdge DefaultEdge = CEdge::getDefault();

  bool FirstTime = true;

  while (is.good() && pNode < pNodeEnd && pEdge < pEdgeEnd)
    {
      *pEdge = DefaultEdge;

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
          mFirstLocalNode = pNode->id;

          FirstTime = false;
        }

      if (pNode->id != pEdge->targetId)
        {
          pNode->EdgesSize = pEdge - pNode->Edges;

          ++pNode;

          if (pNode < pNodeEnd)
            {
              *pNode = DefaultNode;
              pNode->id = pEdge->targetId;
              pNode->Edges = pEdge;
            }
        }

      pEdge->pTarget = pNode;

      if (pEdge->sourceId < mFirstLocalNode || mBeyondLocalNode <= pEdge->sourceId)
        {
          CNode Remote = DefaultNode;
          Remote.id = (pEdge->sourceId);

          pEdge->pSource = &mRemoteNodes.insert(std::make_pair(pEdge->sourceId, Remote)).first->second;
        }

      ++pEdge;
    }

  pNode->EdgesSize = pEdge - pNode->Edges;
  mBeyondLocalNode = pNode->id + 1;

  is.close();

  if (mValid)
    {
      for (pEdge = mEdges; pEdge != pEdgeEnd; ++pEdge)
        if (pEdge->pSource == NULL)
          {
            pEdge->pSource = lookupNode(pEdge->sourceId, false);

            if (pEdge->pSource== NULL)
              {
                std::cerr << "Network file: '" << mFile << "' " "Source not found " << pEdge << ", " << pEdge->targetId << ", " << pEdge->sourceId << std::endl;
                mValid = false;
              }
          }
    }
}

void CNetwork::write(const std::string & file, bool binary)
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

  if (CEdge::HasEdgeTrait) os << ",edgeTrait";

  if (CEdge::HasActiveField) os << ",active";

  if (CEdge::HasWeightField) os << ",weight";

  os << std::endl;

  CEdge * pEdge = beginEdge();
  CEdge * pEdgeEnd = endEdge();

  for (; pEdge != pEdgeEnd; ++pEdge)
    {
      writeEdge(pEdge, os);
    }

  os.close();
}

void CNetwork::writePartition(const size_t & partition,
                              const size_t & partCount,
                              const size_t & nodeCount,
                              const size_t & firstLocalNode,
                              const size_t & beyondLocalNode,
                              const size_t & edgeCount,
                              const std::string & edges,
                              const std::string & outputDirectory)
{
  std::string FileName = mFile;

  if (!outputDirectory.empty())
    {
      FileName = CDirEntry::fileName(mFile);
      CDirEntry::makePathAbsolute(FileName, outputDirectory);
    }

  json_t * pJson = json_deep_copy(mpJson);
  json_t * pValue = json_object_get(pJson, "encoding");

  if (json_is_string(pValue))
    {
      json_string_set(pValue, "binary");
    }

  json_object_set_new(pJson, "partition", json_object());

  pValue = json_object_get(pJson, "partition");
  json_object_set_new(pValue, "numberOfParts", json_integer(partCount));
  json_object_set_new(pValue, "numberOfNodes", json_integer(nodeCount));
  json_object_set_new(pValue, "firstLocalNode", json_integer(firstLocalNode));
  json_object_set_new(pValue, "beyondLocalNode", json_integer(beyondLocalNode));
  json_object_set_new(pValue, "numberOfEdges", json_integer(edgeCount));

  std::ostringstream File;
  File << FileName << "." << partition - 1;

  std::ofstream os(File.str().c_str());

  if (os.fail())
    {
      std::cout << File.str() << std::endl;
    }
  os << json_dumps(pJson, JSON_COMPACT | JSON_INDENT(0)) << std::endl;
  os << "targetPID,targetActivity,sourcePID,sourceActivity,duration,edgeTrait,active,weight";

  os << std::endl;

  os << edges << std::endl;
}

bool CNetwork::haveValidPartition(const int & parts)
{
  bool valid = parts > 0;

  for (int i = 0; i < parts && valid; ++i)
    {
      std::ostringstream File;
      File << mFile << "." << i;

      std::ifstream is;

      if (CDirEntry::isFile(File.str()))
        {
          json_t * pJson = CSimConfig::loadJson(File.str(), 0);

          valid &= pJson != NULL;

          json_t * pPartition = json_object_get(pJson, "partition");

          valid &= pPartition != NULL;

          json_t * pValue = json_object_get(pPartition, "numberOfParts");

          valid &= (json_is_integer(pValue) &&
                json_integer_value(pValue) == parts);

          pValue = json_object_get(pJson, "encoding");

          valid &= (json_is_string(pValue) &&
              strcmp(json_string_value(pValue), "binary") == 0);

          json_decref(pJson);
        }
      else
        {
          valid = false;
        }
    }

  return valid;
}

CNode * CNetwork::beginNode()
{
  return mLocalNodes;
}

CNode * CNetwork::endNode()
{
  return mLocalNodes + mLocalNodesSize;
}

CEdge * CNetwork::beginEdge()
{
  return mEdges;
}

CEdge * CNetwork::endEdge()
{
  return mEdges + mEdgesSize;
}

bool CNetwork::isRemoteNode(const CNode * pNode) const
{
  return pNode < mLocalNodes || mLocalNodes + mLocalNodesSize <= pNode;
}

CNode * CNetwork::lookupNode(const size_t & id, const bool localOnly) const
{
  if (id < mFirstLocalNode || mBeyondLocalNode <= id)
    {
      if (!localOnly)
        {
          std::map< size_t, CNode >::const_iterator found = mRemoteNodes.find(id);

          if (found != mRemoteNodes.end())
            {
              return const_cast< CNode *>(&found->second);
            }
        }

      return NULL;
    }

  CNode * pLeft = mLocalNodes;
  CNode * pRight = mLocalNodes + mLocalNodesSize - 1;
  CNode * pCurrent = pCurrent = pLeft + (pRight - pLeft)/2;

  while (pLeft <= pRight)
    {
      if (pCurrent->id < id)
        {
          pLeft = pCurrent + 1;
        }
      else if (pCurrent->id > id)
        {
          pRight = pCurrent - 1;
        }
      else
        {
          return pCurrent;
        }

      pCurrent = pLeft + (pRight - pLeft)/2;
    }

  return NULL;
}

CEdge * CNetwork::lookupEdge(const size_t & targetId, const size_t & sourceId) const
{
  // We only have edges for local target nodes
  if (targetId < mFirstLocalNode || mBeyondLocalNode <= targetId) return NULL;

  CNode * pTargetNode = lookupNode(targetId, true);

  // Handle invalid requests
  if (pTargetNode == NULL) return NULL;

  CEdge * pLeft = pTargetNode->Edges;
  CEdge * pRight = pLeft + pTargetNode->EdgesSize;
  CEdge * pCurrent = pLeft + (pRight - pLeft)/2;

  while (pLeft <= pRight)
    {
      if (pCurrent->sourceId < sourceId)
        {
          pLeft = pCurrent + 1;
        }
      else if (pCurrent->sourceId > sourceId)
        {
          pRight = pCurrent - 1;
        }
      else
        {
          return pCurrent;
        }

      pCurrent = pLeft + (pRight - pLeft)/2;
    }

  return NULL;
}

bool CNetwork::loadEdge(CEdge * pEdge, std::istream & is) const
{
  bool success = true;

  if (mIsBinary)
    {
      pEdge->fromBinary(is);

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

      CTrait::ActivityTrait->fromString(targetActivity, pEdge->targetActivity);
      CTrait::ActivityTrait->fromString(sourceActivity, pEdge->sourceActivity);
      ptr += Read;

      if (CEdge::HasEdgeTrait)
        {
          if (1 != sscanf(ptr, ",%[^,]%n", edgeTrait, &Read))
            {
              success = false;
            }

          CTrait::EdgeTrait->fromString(edgeTrait, pEdge->edgeTrait);
          ptr += Read;
        }

      if (CEdge::HasActiveField)
        {
          char Active;

          if (1 != sscanf(ptr, ",%c%n", &Active, &Read))
            {
              success = false;
            }

          pEdge->active = (Active == '1');
          ptr += Read;
        }

      if (CEdge::HasWeightField)
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

void CNetwork::writeEdge(CEdge * pEdge, std::ostream & os) const
{
  if (mIsBinary)
    {
      pEdge->toBinary(os);
    }
  else
    {
      os << pEdge->targetId;
      os << "," << CTrait::ActivityTrait->toString(pEdge->targetActivity);
      os << "," << pEdge->sourceId;
      os << "," << CTrait::ActivityTrait->toString(pEdge->sourceActivity);
      os << "," << pEdge->duration;

      if (CEdge::HasEdgeTrait)
        {
          os << "," << CTrait::EdgeTrait->toString(pEdge->edgeTrait);
        }

      if (CEdge::HasActiveField)
        {
          os << "," << pEdge->active;
        }

      if (CEdge::HasWeightField)
        {
          os << "," << pEdge->weight;
        }

      os << std::endl;
    }
}


const bool & CNetwork::isValid() const
{
  return mValid;
}

int CNetwork::broadcastChanges()
{
  std::string Buffer = Changes::getNodes().str();

  // std::cout << Communicate::Rank << ": ActionQueue::broadcastChanges (Nodes)" << std::endl;
  CCommunicate::ClassMemberReceive< CNetwork > ReceiveNode(CNetwork::INSTANCE, &CNetwork::receiveNodes);
  CCommunicate::broadcast(Buffer.c_str(), Buffer.length(), &ReceiveNode);

  Buffer = Changes::getEdges().str();

  // std::cout << Communicate::Rank << ": ActionQueue::broadcastChanges (Edges)" << std::endl;
  CCommunicate::ClassMemberReceive< CNetwork > ReceiveEdge(CNetwork::INSTANCE, &CNetwork::receiveEdges);
  CCommunicate::broadcast(Buffer.c_str(), Buffer.length(), &ReceiveEdge);

  Changes::clear();

  return (int) CCommunicate::ErrorCode::Success;
}

CCommunicate::ErrorCode CNetwork::receiveNodes(std::istream & is, int sender)
{
  while (true)
    {
      CNode Node;
      Node.CNode::fromBinary(is);

      if (is.fail())
        {
          break;
        }

      CNode * pNode = lookupNode(Node.id, false);

      if (pNode != NULL)
        {
          pNode->setHealthState(Node.getHealthState());
          pNode->susceptibilityFactor = Node.susceptibilityFactor;
          pNode->susceptibility = Node.susceptibility;
          pNode->infectivityFactor = Node.infectivityFactor;
          pNode->infectivity = Node.infectivity;
          pNode->nodeTrait = Node.nodeTrait;
        }
    }

  return CCommunicate::ErrorCode::Success;
}

CCommunicate::ErrorCode CNetwork::receiveEdges(std::istream & is, int sender)
{
  while (true)
    {
      CEdge Edge;
      Edge.fromBinary(is);

      if (is.fail())
        {
          break;
        }

      CEdge * pEdge = lookupEdge(Edge.targetId, Edge.sourceId);

      if (pEdge != NULL)
        {
          pEdge->targetActivity = Edge.targetActivity;
          pEdge->sourceActivity = Edge.sourceActivity;
          pEdge->duration = Edge.duration;
          pEdge->edgeTrait = Edge.edgeTrait;
          pEdge->active = Edge.active;
          pEdge->weight = Edge.weight;
        }
    }

  return CCommunicate::ErrorCode::Success;
}

const size_t & CNetwork::getTotalNodes() const
{
  return mTotalNodesSize;
}



