// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2024 Rector and Visitors of the University of Virginia 
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

#include <algorithm>
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
#include "actions/CChanges.h"
#include "network/CEdge.h"
#include "network/CNetwork.h"

#include "utilities/CSimConfig.h"
#include "utilities/CDirEntry.h"
#include "network/CNode.h"
#include "CNetwork.h"

// static
void CNetwork::init(const std::string & networkFile)
{
  CChanges::init();
  Context.init();

  Context.Master().loadJsonPreamble(networkFile);
  Context.Master().partition(CCommunicate::TotalProcesses(), false);
}

// static
void CNetwork::clear()
{
  Context.release();
  CChanges::release();
}

// static
int CNetwork::index(const CNode * pNode)
{
  if (Context.Master().isRemoteNode(pNode))
    return -1;

  CNetwork * pLeft = Context.beginThread();
  CNetwork * pRight = Context.endThread() - 1;
  CNetwork * pCurrent = pLeft + (pRight - pLeft) / 2;

  while (pLeft <= pRight)
    {
      if (pCurrent->endNode() <= pNode)
        {
          pLeft = pCurrent + 1;
        }
      else if (pCurrent->beginNode() > pNode)
        {
          pRight = pCurrent - 1;
        }
      else
        {
          return Context.localIndex(pCurrent);
        }

      pCurrent = pLeft + (pRight - pLeft) / 2;
    }

  return -1;
}

// static
int CNetwork::index(const size_t & id)
{
  if (Context.Master().isRemoteNode(id))
    return -1;

  CNetwork * pLeft = Context.beginThread();
  CNetwork * pRight = Context.endThread() - 1;
  CNetwork * pCurrent = pLeft + (pRight - pLeft) / 2;

  while (pLeft <= pRight)
    {
      if ((pCurrent->endNode() - 1)->id < id)
        {
          pLeft = pCurrent + 1;
        }
      else if (pCurrent->beginNode()->id > id)
        {
          pRight = pCurrent - 1;
        }
      else
        {
          return Context.localIndex(pCurrent);
        }

      pCurrent = pLeft + (pRight - pLeft) / 2;
    }

  return -1;
}

// static
double CNetwork::timeResolution()
{
  return Context.Master().mTimeResolution;
}

CNetwork::CNetwork()
  : CAnnotation()
  , mFile()
  , mLocalNodes(NULL)
  , mFirstLocalNode(std::numeric_limits< size_t >::max())
  , mBeyondLocalNode(0)
  , mLocalNodesSize(0)
  , mRemoteNodes()
  , mSourceOnlyNodes()
  , mNodes(NULL)
  , mNodesSize(0)
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
  , mDumpActiveNetwork()
{}

void CNetwork::loadJsonPreamble(const std::string & networkFile)
{
  mFile = networkFile;

  if (mFile.empty())
    {
      CLogger::error("Network file is not specified");
      return;
    }

  mpJson = CSimConfig::loadJsonPreamble(mFile, 0);

  fromJSON(mpJson);
}

// virtual
CNetwork::~CNetwork()
{
  if (!Context.isMaster(this))
    return;

  if (mpJson != NULL)
    {
      json_decref(mpJson);
    }

  // This data is owned by master;
  if (mNodes != NULL)
    {
      delete[] mNodes;
      mNodes = NULL;
    }

  if (mEdges != NULL)
    {
      delete[] mEdges;
      mEdges = NULL;
    }
}

void CNetwork::fromJSON(const json_t * json)
{
  /*
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "title": "Schema title",
  "description": "Description of the schema",
  "type": "object",
  "required": [
    "epiHiperSchema",
    "encoding",
    "numberOfNodes",
    "numberOfEdges",
    "sizeofPID",
    "activityEncoding",
    "sizeofActivity",
    "edgeTraitEncoding",
    "sizeofEdgeTrait",
    "accumulationTime",
    "timeResolution",
    "hasActiveField",
    "hasWeightField"
  ],
  "properties": {
    "epiHiperSchema": {
      "type": "string",
      "pattern": "^./networkSchema.json$"
    },
    "encoding": {
      "description": "Encoding used for the network edges",
      "type": "string",
      "enum": [
        "binary",
        "text"
      ]
    },
    "numberOfNodes": {
      "description": "The number of nodes in the network",
      "$ref": "./typeRegistry.json#/definitions/nonNegativeInteger"
    },
    "numberOfEdges": {
      "description": "The number of edges in the network",
      "$ref": "./typeRegistry.json#/definitions/nonNegativeInteger"
    },
    "sizeofPID": {
      "description": "The size of the person Id (PID) in bytes",
      "$ref": "./typeRegistry.json#/definitions/nonNegativeInteger"
    },
    "accumulationTime": {
      "description": "An annotation string describing the accumulation time for the contact",
      "type": "string"
    },
    "timeResolution": {
      "description": "The maximal value of the duration field, i.e., the value to divide the duration by to get it relative to the accumulation time",
      "$ref": "./typeRegistry.json#/definitions/nonNegativeNumber"
    },
    "hasLocationIDField": {
      "description": "Boolean indicating whether the network contains a Location ID (LID) for each edge (Default: false).",
      "type": "boolean"
    },
    "hasActiveField": {
      "description": "Boolean indicating whether the network contains an active flag for each edge",
      "type": "boolean"
    },
    "hasWeightField": {
      "description": "Boolean indicating whether the network contains a weight for each edge",
      "type": "boolean"
    },
    "activityEncoding": {
      "description": "The features and their valid values encoded in the target and source activities",
      "allOf": [
        {"$ref": "./typeRegistry.json#/definitions/trait"},
        {
          "type": "object",
          "properties": {
            "id": {
              "enum": ["activityTrait"]
            }
          }
        }
      ]
    },
    "sizeofActivity": {
      "description": "The size of the target and source activities in bytes",
      "$ref": "./typeRegistry.json#/definitions/nonNegativeInteger"
    },
    "edgeTraitEncoding": {
      "description": "The features and their valid values encoded in the edge trait",
      "allOf": [
        {"$ref": "./typeRegistry.json#/definitions/trait"},
        {
          "type": "object",
          "properties": {
            "id": {
              "enum": ["edgeTrait"]
            }
          }
        }
      ]
    },
    "sizeofEdgeTrait": {
      "description": "The size of the edge trait in bytes",
      "$ref": "./typeRegistry.json#/definitions/nonNegativeInteger"
    },
    "partition": {
      "type": "object",
      "required": [
        "numberOfNodes",
        "numberOfEdges",
        "numberOfParts",
        "firstLocalNode",
        "beyondLocalNode"
      ],
      "properties": {
        "numberOfNodes": {
          "description": "The number of nodes in the network",
          "$ref": "./typeRegistry.json#/definitions/nonNegativeInteger"
        },
        "numberOfEdges": {
          "description": "The number of edges in the network",
          "$ref": "./typeRegistry.json#/definitions/nonNegativeInteger"
        },
        "numberOfParts": {
          "description": "The number of partitions of the network",
          "$ref": "./typeRegistry.json#/definitions/nonNegativeInteger"
        },
        "firstLocalNode": {
          "description": "The number of the first local node",
          "$ref": "./typeRegistry.json#/definitions/nonNegativeInteger"
        },
        "beyondLocalNode": {
          "description": "The number of the first node beyond the local nodes",
          "$ref": "./typeRegistry.json#/definitions/nonNegativeInteger"
        }
      }
    },
    "sourceOnlyNodes": {
      "type": "array",
      "items": {
        "$ref": "./typeRegistry.json#/definitions/nonNegativeInteger"
      }
    }
  }
}
  */

  mValid = false;

  json_t * pValue = json_object_get(json, "encoding");

  if (json_is_string(pValue))
    {
      std::string Encoding = json_string_value(pValue);
      mIsBinary = (Encoding != "text");
    }

  pValue = json_object_get(json, "numberOfNodes");
  mTotalNodesSize = 0;

  if (json_is_integer(pValue))
    {
      mTotalNodesSize = json_integer_value(pValue);
    }

  if (mTotalNodesSize == 0)
    {
      CLogger::error("Network: Invalid or missing 'numberOfNodes'.");
      return;
    }

  pValue = json_object_get(json, "numberOfEdges");
  mTotalEdgesSize = 0;

  if (json_is_integer(pValue))
    {
      mTotalEdgesSize = json_integer_value(pValue);
    }

  if (mTotalEdgesSize == 0)
    {
      CLogger::error("Network: Invalid or missing 'numberOfEdges'.");
      return;
    }

  pValue = json_object_get(json, "sizeofPID");
  mSizeOfPid = 0;

  if (json_is_integer(pValue))
    {
      mSizeOfPid = json_integer_value(pValue);
    }

  if (mSizeOfPid != 4
      && mSizeOfPid != 8)
    {
      CLogger::error("Network: Invalid or missing 'sizeofPID'.");
      return;
    }

  // This is only annotation for know.
  pValue = json_object_get(json, "accumulationTime");

  if (json_is_string(pValue))
    {
      mAccumulationTime = json_string_value(pValue);
    }

  pValue = json_object_get(json, "timeResolution");
  mTimeResolution = 0.0;

  if (json_is_integer(pValue))
    {
      mTimeResolution = json_integer_value(pValue);
    }
  else if (json_is_real(pValue))
    {
      mTimeResolution = json_real_value(pValue);
    }

  if (mTimeResolution == 0.0)
    {
      CLogger::error("Network: Invalid or missing 'timeResolution'.");
      return;
    }

  size_t Size = 0;

  pValue = json_object_get(json, "sizeofActivity");

  if (json_is_integer(pValue))
    {
      Size = json_integer_value(pValue);
    }

  if (Size == 0)
    {
      CLogger::error("Network: Invalid or missing 'sizeofActivity'.");
      return;
    }

  pValue = json_object_get(json, "activityEncoding");

  if (json_is_object(pValue))
    {
      CTrait::loadSingle(pValue);
    }

  if (!CTrait::ActivityTrait->isValid())
    {
      CLogger::error("Network: Invalid or missing 'activityEncoding'.");
      return;
    }

  pValue = json_object_get(json, "hasLocationIDField");

  if (json_is_boolean(pValue))
    {
      CEdge::HasLocationId = json_boolean_value(pValue);
    }
  else
    {
      CEdge::HasLocationId = false;
    }

#ifndef USE_LOCATION_ID
  if (CEdge::HasLocationId)
    CLogger::error("Network: To support location ids please configure EpiHiper with '-DENABLE_LOCATION_ID=ON'.");
#endif

  pValue = json_object_get(json, "sizeofEdgeTrait");
  Size = 1;

  if (json_is_integer(pValue))
    {
      Size = json_integer_value(pValue);
    }

  if (Size != 4 && Size != 0)
    {
      CLogger::error("Network: Invalid or missing 'sizeofEdgeTrait'.");
      return;
    }

  pValue = json_object_get(json, "edgeTraitEncoding");

  if (json_is_object(pValue))
    {
      CTrait::loadSingle(pValue);

      if (CTrait::EdgeTrait->isValid())
        {
          CEdge::HasEdgeTrait = (Size == 4);
        }
    }

  if (Size == 4 && !CTrait::EdgeTrait->isValid())
    {
      CLogger::error("Network: Invalid or missing 'edgeTraitEncoding'.");
      return;
    }

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

  pValue = json_object_get(json, "sourceOnlyNodes");

  if (json_is_array(pValue))
    {
      for (size_t i = 0, imax = json_array_size(pValue); i < imax; ++i)
        mSourceOnlyNodes.insert(json_integer_value(json_array_get(pValue, i)));
    }

  CAnnotation::fromJSON(json);
  mValid = true;
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

  if (!save
      && mTotalEdgesSize >= CSimConfig::getPartitionEdgeLimit())
    {
      CLogger::error("CNetwork: No valid partition ({}) found and partition edge limit ({}) exceded.", parts, CSimConfig::getPartitionEdgeLimit());
      return;
    }

  std::ifstream is(mFile.c_str());
  std::string Line;
  // Skip JSON Header
  std::getline(is, Line);
  // Skip Column Header
  std::getline(is, Line);

  if (parts == 1
      && save
      && !outputDirectory.empty())
    convert(is, outputDirectory);
  else
    partition(is, parts, save, outputDirectory);

  is.close();
}

void CNetwork::convert(std::istream & is, const std::string & outputDirectory)
{
  // Communicate::Processes = 8;

  size_t Partition[4];

  if (CCommunicate::MPIRank == 0)
    {
      size_t * pPartition = Partition;
      size_t * pNodes = pPartition + 1;
      size_t * pEdges = pPartition + 2;

      size_t Node = std::numeric_limits< size_t >::max();
      size_t CurrentNode = std::numeric_limits< size_t >::max();
      size_t PartitionEdgeCount = 0;

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
              if (CurrentNode > Node && CurrentNode != std::numeric_limits< size_t >::max())
                {
                  CLogger::error("Network target nodes are not sorted.");
                  return;
                }

              ++*pNodes;

              CurrentNode = Node;
            }

          ++*pEdges;

          // Write edge to current node buffer
          ++PartitionEdgeCount;
        }

      if (CLogger::hasErrors())
        {
          return;
        }

      *(pPartition + 3) = Node + 1;

      std::ofstream os;
      openPartition(PartitionIndex, 1, *pNodes, *pPartition, *(pPartition + 3), *pEdges, outputDirectory, os);

      is.clear();                 // clear fail and eof bits
      is.seekg(0, std::ios::beg); // back to the start!

      std::string Line;
      // Skip JSON Header
      std::getline(is, Line);
      // Skip Column Header
      std::getline(is, Line);

      while (is.good() && loadEdge(&Edge, is))
        {
          Edge.toBinary(os);
        }

      os.close();
    }
}

void CNetwork::partition(std::istream & is, const int & parts, const bool & save, const std::string & outputDirectory)
{
  // Communicate::Processes = 8;

  size_t Partition[parts * 3 + 1];

  if (CCommunicate::MPIRank == 0)
    {
      std::ostringstream NodeBuffer;

      std::ostringstream PartitionBuffer;

      size_t * pFirst = Partition;
      size_t * pNodes = Partition + 1;
      size_t * pEdges = Partition + 2;

      *pFirst = std::numeric_limits< size_t >::max();
      *pNodes = 0;
      *pEdges = 0;

      double DesiredPerComputeNode = (double) mTotalEdgesSize / parts;

      size_t Node = std::numeric_limits< size_t >::max();
      size_t CurrentNode = std::numeric_limits< size_t >::max();
      size_t PartitionEdgeCount = 0;
      size_t LastNodeEdgeCount = 0;

      size_t PartitionIndex = 1;
      std::set< size_t >::const_iterator itSourceOnlyNode = mSourceOnlyNodes.begin();

      CEdge Edge = CEdge::getDefault();

      while (is.good() && loadEdge(&Edge, is))
        {
          // std::cout << Edge.targetId << ", " << Edge.targetActivity << ", " << Edge.sourceId << ", " << Edge.sourceActivity << ", " << Edge.duration << std::endl;
          if (PartitionEdgeCount == 0)
            {
              *pFirst = std::min(Edge.targetId, *itSourceOnlyNode);

              // Account for source only nodes at the very beginning.
              while (itSourceOnlyNode != mSourceOnlyNodes.end()
                     && *itSourceOnlyNode < Edge.targetId)
                {
                  ++pNodes;
                  ++itSourceOnlyNode;
                }
            }

          Node = Edge.targetId;

          if (CurrentNode != Node)
            {
              if (CurrentNode > Node && CurrentNode != std::numeric_limits< size_t >::max())
                {
                  CLogger::error("Network target nodes are not sorted.");
                  return;
                }

              if (PartitionEdgeCount >= PartitionIndex * DesiredPerComputeNode)
                {
                  // PartitionIndex * DesiredPerComputeNode - LastNodeEdgeCount < PartitionEdgeCount - PartitionIndex * DesiredPerComputeNode
                  if (2 * PartitionIndex * DesiredPerComputeNode< LastNodeEdgeCount + PartitionEdgeCount && * pNodes > 1) // A partition must include a least 1 node
                    {
                      *pNodes -= 1;
                      *pEdges -= PartitionEdgeCount - LastNodeEdgeCount;

                      *(pFirst + 3) = CurrentNode;
                      *(pNodes + 3) = 1;
                      *(pEdges + 3) = PartitionEdgeCount - LastNodeEdgeCount;

                      // Write partition
                      if (save)
                        {
                          writePartition(PartitionIndex, parts, *pNodes, *pFirst, *(pFirst + 3), *pEdges, PartitionBuffer.str(), outputDirectory);
                          PartitionBuffer.str("");
                        }
                    }
                  else
                    {
                      // Account for source only nodes.
                      // Note: this assures that we always append nodes at the end of the partition.
                      while (itSourceOnlyNode != mSourceOnlyNodes.end()
                             && *itSourceOnlyNode < Node)
                        {
                          ++*pNodes;
                          ++itSourceOnlyNode;
                        }

                      *(pFirst + 3) = Node;
                      *(pNodes + 3) = 0;
                      *(pEdges + 3) = 0;

                      if (save)
                        {
                          // Write current node buffer to partition buffer
                          PartitionBuffer << NodeBuffer.str();
                          NodeBuffer.str("");

                          // Write partition
                          writePartition(PartitionIndex, parts, *pNodes, *pFirst, *(pFirst + 3), *pEdges, PartitionBuffer.str(), outputDirectory);
                          PartitionBuffer.str("");
                        }
                    }

                  pFirst += 3;
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

              // Account for source only nodes.
              while (itSourceOnlyNode != mSourceOnlyNodes.end()
                     && *itSourceOnlyNode < Node)
                {
                  ++*pNodes;
                  ++itSourceOnlyNode;
                }

              LastNodeEdgeCount = PartitionEdgeCount;

              CurrentNode = Node;
            }

          // Write edge to current node buffer
          Edge.toBinary(NodeBuffer);

          ++*pEdges;
          ++PartitionEdgeCount;
        }

      if (CLogger::hasErrors())
        {
          return;
        }

      // Account for remaining source only nodes
      while (itSourceOnlyNode != mSourceOnlyNodes.end())
        {
          Node = *itSourceOnlyNode;

          ++*pNodes;
          ++itSourceOnlyNode;
        }

      *(pFirst + 3) = Node + 1;

      // Handling the rare case where we have fewer nodes than parts.
      while (PartitionIndex <= (size_t) parts)
        {
          if (save)
            {
              // Write current node buffer to partition buffer
              PartitionBuffer << NodeBuffer.str();
              NodeBuffer.str("");

              // Write partition
              writePartition(PartitionIndex, parts, *pNodes, *pFirst, *(pFirst + 3), *pEdges, PartitionBuffer.str(), outputDirectory);
              PartitionBuffer.str("");
            }

          ++PartitionIndex;

          // We must not write beyond the Partition buffer.
          if (PartitionIndex < (size_t) parts)
            {
              pFirst += 3;
              pNodes += 3;
              pEdges += 3;

              *pNodes = 0;
              *pEdges = 0;
              *(pFirst + 3) = *pFirst;
            }
        }
    }

  CCommunicate::broadcast(Partition, (parts * 3 + 1) * sizeof(size_t), 0);

  CNetwork * pEnd = Context.endThread();

  for (CNetwork * pIt = Context.beginThread(); pIt != pEnd; ++pIt)
    {
      size_t * pPartInfo = Partition + 3 * Context.globalIndex(pIt);
      pIt->mFirstLocalNode = *pPartInfo++;
      pIt->mLocalNodesSize = *pPartInfo++;
      pIt->mEdgesSize = *pPartInfo++;
      pIt->mBeyondLocalNode = *pPartInfo;
      pIt->mValid = true;
    }

  // CLogger::info() << "CNetwork::partition: " << mFirstLocalNode << ", " << mBeyondLocalNode << ", " << mLocalNodesSize << ", " << mEdgesSize << std::endl;
}

void CNetwork::load()
{
  if (!mValid)
    return;

  if (mFile.empty())
    {
      CLogger::error("Network file is not specified");
      mValid = false; // DONE
      return;
    }

  CNetwork * pEnd = Context.endThread();

#pragma omp parallel
  {
    CNetwork & Active = Context.Active();

    std::ostringstream File;
    File << mFile << "." << Context.globalIndex(&Active);
    Active.mValid = mValid;

    if (Active.mLocalNodesSize == 0
        && CDirEntry::isFile(File.str()))
      {
        Active.mFile = File.str();

        json_t * pJson = CSimConfig::loadJsonPreamble(Active.mFile, 0);
        json_t * pPartition = json_object_get(pJson, "partition");
        json_t * pValue = json_object_get(pPartition, "numberOfNodes");

        if (json_is_integer(pValue))
          {
            Active.mLocalNodesSize = json_integer_value(pValue);
          }
        else
          {
            Active.mValid = false;
          }

        pValue = json_object_get(pPartition, "firstLocalNode");

        if (json_is_integer(pValue))
          {
            Active.mFirstLocalNode = json_integer_value(pValue);
          }
        else
          {
            Active.mValid = false;
          }

        pValue = json_object_get(pPartition, "beyondLocalNode");

        if (json_is_integer(pValue))
          {
            Active.mBeyondLocalNode = json_integer_value(pValue);
          }
        else
          {
            Active.mValid = false;
          }

        pValue = json_object_get(pPartition, "numberOfEdges");

        if (json_is_integer(pValue))
          {
            Active.mEdgesSize = json_integer_value(pValue);
          }
        else
          {
            Active.mValid = false;
          }

        pValue = json_object_get(pJson, "encoding");

        if (json_is_string(pValue))
          {
            std::string Encoding = json_string_value(pValue);
            Active.mIsBinary = (Encoding != "text");
          }
        else
          {
            Active.mValid = false;
          }

        json_decref(pJson);
      }
    else
      {
        Active.mFile = mFile;
      }

    if (Context.isThread(&Active))
#pragma omp critical(load_network_master_data)
      {
        mFirstLocalNode = std::min(mFirstLocalNode, Active.mFirstLocalNode);
        mLocalNodesSize += Active.mLocalNodesSize;
        mBeyondLocalNode = std::max(mBeyondLocalNode, Active.mBeyondLocalNode);
        mEdgesSize += Active.mEdgesSize;
        mValid &= Active.mValid;
      }
  }

  CLogger::info("Network: Allocating edges '{}' ({} bytes).", mEdgesSize, mEdgesSize * sizeof(CEdge));

  try
    {
      mEdges = new CEdge[mEdgesSize];
    }

  catch (...)
    {
      CLogger::error("Network: Allocating edges failed '{}' ({} bytes).", mEdgesSize, mEdgesSize * sizeof(CEdge));

      return;
    }

  CEdge * pEdge = mEdges;

  for (CNetwork * pIt = Context.beginThread(); pIt != pEnd; ++pIt)
    if (Context.isThread(pIt))
      {
        // We need to postpone because the nodes are not yet allocated
        // pIt->mLocalNodes = pNode;
        // pNode += pIt->mLocalNodesSize;
        pIt->mEdges = pEdge;
        pEdge += pIt->mEdgesSize;
        pIt->mTotalNodesSize = mTotalNodesSize;
        pIt->mTotalEdgesSize = mTotalEdgesSize;
        pIt->mSizeOfPid = mSizeOfPid;
        pIt->mAccumulationTime = mAccumulationTime;
        pIt->mTimeResolution = mTimeResolution;
        pIt->mpJson = json_incref(mpJson);
      }

#pragma omp parallel
  {
    CNetwork & Active = Context.Active();
    std::ostringstream File;
    std::ifstream is;

    is.open(Active.mFile.c_str());

    if (is.fail())
      {
        CLogger::error("Network file: '" + Active.mFile + "' cannot be opened.");
        Active.mValid = false; // DONE
      }

    std::string Line;

    // Skip JSON Header
    std::getline(is, Line);
    // Skip Column Header
    std::getline(is, Line);

    std::set< size_t >::const_iterator itSourceOnlyNode;

    if (Context.globalIndex(&Active) == 0)
      itSourceOnlyNode = mSourceOnlyNodes.begin();
    else
      itSourceOnlyNode = mSourceOnlyNodes.lower_bound(Active.mFirstLocalNode);

    std::set< size_t >::const_iterator endSourceOnlyNode = mSourceOnlyNodes.upper_bound(Active.mBeyondLocalNode);

    Active.mLocalNodeIds.resize(Active.mLocalNodesSize);
    std::vector< size_t >::iterator itLocalNodeId = Active.mLocalNodeIds.begin();
    size_t Node = std::numeric_limits< size_t >::max();

    CEdge * pEdge = Active.mEdges;
    CEdge * pEdgeEnd = pEdge + Active.mEdgesSize;
    CEdge DefaultEdge = CEdge::getDefault();

    while (is.good() && pEdge < pEdgeEnd)
      {
        *pEdge = DefaultEdge;

        if (!Active.loadEdge(pEdge, is))
          {
            CLogger::error("Network file: '{}' invalid edge ({}).", mFile, pEdge - Active.beginEdge());

            Active.mValid = false; // DONE
            break;
          }

        if (pEdge->targetId < Active.mFirstLocalNode)
          continue;

        if (Node != pEdge->targetId)
          {
            while (itSourceOnlyNode != endSourceOnlyNode
                   && *itSourceOnlyNode < pEdge->targetId)
              {
                *itLocalNodeId = *itSourceOnlyNode;
                ++itSourceOnlyNode;
                ++itLocalNodeId;
              }

            Node = pEdge->targetId;
            *itLocalNodeId = Node;
            ++itLocalNodeId;
          }

        if (pEdge->sourceId < Active.mFirstLocalNode || Active.mBeyondLocalNode <= pEdge->sourceId)
          {
            Active.mRemoteNodes[pEdge->sourceId] = NULL;

            if (CCommunicate::LocalProcesses() > 1)
              if (pEdge->sourceId < mFirstLocalNode || mBeyondLocalNode <= pEdge->sourceId)
#pragma omp critical(load_network_master_remote_nodes)
                mRemoteNodes[pEdge->sourceId] = NULL;
          }

        ++pEdge;
      }

    if (Active.mValid)
      {
        while (itSourceOnlyNode != endSourceOnlyNode)
          {
            Node = *itSourceOnlyNode;
            *itLocalNodeId = Node;
            ++itSourceOnlyNode;
            ++itLocalNodeId;
          }

        assert(itLocalNodeId == Active.mLocalNodeIds.end());
        assert(itSourceOnlyNode == endSourceOnlyNode);
        assert(Active.mBeyondLocalNode > Node
               || Node == std::numeric_limits< size_t >::max());
      }

#pragma omp atomic
    mValid &= Active.mValid;
    is.close();
  }

  if (!mValid)
    return;

  initNodes();

#pragma omp parallel
  {
    CNetwork & Active = Context.Active();

    CNode * pNode = Active.beginNode();
    CNode * pNodeEnd = Active.endNode();
    CEdge * pEdge = Active.beginEdge();
    CEdge * pEdgeEnd = Active.endEdge();

    pNode->Edges = pEdge;

    while (pEdge != pEdgeEnd && pNode != pNodeEnd)
      {
        // This loop accounts source only nodes
        while (pNode->id != pEdge->targetId)
          {
            pNode->EdgesSize = pEdge - pNode->Edges;
            pNode++;
            pNode->Edges = pEdge;
          }

        pEdge->pTarget = pNode;
        pEdge->pSource = Active.lookupNode(pEdge->sourceId, false);

        Active.mOutgoingEdges[pEdge->pSource].push_back(pEdge);
        ++pEdge;
      }

    assert(pEdge == pEdgeEnd);

    while (true)
      {
        pNode->EdgesSize = pEdge - pNode->Edges;
        pNode++;

        if (pNode == pNodeEnd)
          break;

        pNode->Edges = pEdge;
      }
  }

  initOutgoingEdges();
}

void CNetwork::initNodes()
{
  // Now we can determine how many nodes we must allocate.
  mNodesSize = mLocalNodesSize + mRemoteNodes.size();

  CLogger::info("Network: Allocating nodes '{}' ({} bytes).", mNodesSize, mNodesSize * sizeof(CNode));

  try
    {
      mNodes = new CNode[mNodesSize];
    }

  catch (...)
    {
      CLogger::error Error("Network: Allocating nodes failed '{}' ({} bytes).", mNodesSize, mNodesSize * sizeof(CNode));

      return;
    }

  mLocalNodes = mNodes;

  CNode DefaultNode = CNode::getDefault();
  CNode * pNode = mNodes;

  // Initialize Remote Nodes which are part of mNodes and thus properly sorted by Id
#pragma omp parallel
  {
    std::map< size_t, CNode * >::iterator it = mRemoteNodes.begin();
    std::map< size_t, CNode * >::iterator end = mRemoteNodes.end();

#pragma omp single
    {
      for (; it != end && it->first < mFirstLocalNode; ++it, ++pNode)
#pragma omp task
        {
          *pNode = DefaultNode;
          pNode->id = it->first;
          it->second = pNode;
        }

      mLocalNodes = pNode;
      pNode += mLocalNodesSize;

      for (; it != end; ++it, ++pNode)
#pragma omp task
        {
          *pNode = DefaultNode;
          pNode->id = it->first;
          it->second = pNode;
        }
    }
  }

  pNode = mLocalNodes;

  CNetwork * pEnd = Context.endThread();
  for (CNetwork * pIt = Context.beginThread(); pIt != pEnd; ++pIt)
    if (Context.isThread(pIt))
      {
        pIt->mLocalNodes = pNode;
        pNode += pIt->mLocalNodesSize;
      }

#pragma omp parallel
  {
    CNetwork & Active = Context.Active();
    CNode * pNode = Active.beginNode();

    for (const size_t & Id : Active.mLocalNodeIds)
      {
        *pNode = DefaultNode;
        pNode->id = Id;
        ++pNode;
      }

    Active.mLocalNodeIds.clear();

    if (Context.isThread(&Active))
      {
        std::map< size_t, CNode * >::iterator it = Active.mRemoteNodes.begin();
        std::map< size_t, CNode * >::iterator end = Active.mRemoteNodes.end();

        for (; it != end; ++it)
          {
            it->second = Active.lookupNode(it->first, false);
          }
      }
  }
}

void CNetwork::initOutgoingEdges()
{
#pragma omp parallel
  {
    CNetwork & Active = Context.Active();
    std::map< CNode *, std::vector< CEdge * > >::const_iterator it = Active.mOutgoingEdges.begin();
    std::map< CNode *, std::vector< CEdge * > >::const_iterator end = Active.mOutgoingEdges.end();

    for (; it != end; ++it)
      {
        CNode::sOutgoingEdges & OutgoingEdges = it->first->OutgoingEdges.Active();

        OutgoingEdges.Size = it->second.size();
        OutgoingEdges.pEdges = new CEdge *[OutgoingEdges.Size];

        std::vector< CEdge * >::const_iterator itEdge = it->second.begin();
        std::vector< CEdge * >::const_iterator endEdge = it->second.end();
        CEdge ** ppEdge = OutgoingEdges.pEdges;

        for (; itEdge != endEdge; ++itEdge, ++ppEdge)
          *ppEdge = *itEdge;
      }

    Active.mOutgoingEdges.clear();
  }
}

void CNetwork::writePreamble(std::ostream & os) const
{
  os << CSimConfig::jsonToString(mpJson) << std::endl;

  os << "targetPID,targetActivity,sourcePID,sourceActivity,duration";

  if (CEdge::HasLocationId)
    os << ",LID";

  if (CEdge::HasEdgeTrait)
    os << ",edgeTrait";

  if (CEdge::HasActiveField)
    os << ",active";

  if (CEdge::HasWeightField)
    os << ",weight";

  os << std::endl;
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

  writePreamble(os);

  CEdge * pEdge = beginEdge();
  CEdge * pEdgeEnd = endEdge();

  for (; pEdge != pEdgeEnd; ++pEdge)
    {
      writeEdge(pEdge, os);
    }

  os.close();
}

bool CNetwork::openPartition(const size_t & partition,
                             const size_t & numberOfParts,
                             const size_t & numberOfNodes,
                             const size_t & firstLocalNode,
                             const size_t & beyondLocalNode,
                             const size_t & numberOfEdges,
                             const std::string & outputDirectory,
                             std::ofstream & os)
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
  json_object_set_new(pValue, "numberOfParts", json_integer(numberOfParts));
  json_object_set_new(pValue, "numberOfNodes", json_integer(numberOfNodes));
  json_object_set_new(pValue, "firstLocalNode", json_integer(firstLocalNode));
  json_object_set_new(pValue, "beyondLocalNode", json_integer(beyondLocalNode));
  json_object_set_new(pValue, "numberOfEdges", json_integer(numberOfEdges));

  std::ostringstream File;
  File << FileName << "." << partition - 1;

  os.open(File.str().c_str());

  if (os.fail())
    {
      std::cout << File.str() << std::endl;
      return false;
    }

  os << CSimConfig::jsonToString(pJson) << std::endl;

  if (CEdge::HasLocationId)
    os << "targetPID,targetActivity,sourcePID,sourceActivity,duration,LID,edgeTrait,active,weight";
  else
    os << "targetPID,targetActivity,sourcePID,sourceActivity,duration,edgeTrait,active,weight";

  os << std::endl;

  json_decref(pJson);

  return true;
}

void CNetwork::writePartition(const size_t & partition,
                              const size_t & numberOfParts,
                              const size_t & numberOfNodes,
                              const size_t & firstLocalNode,
                              const size_t & beyondLocalNode,
                              const size_t & numberOfEdges,
                              const std::string & edges,
                              const std::string & outputDirectory)
{
  std::ofstream os;

  openPartition(partition,
                numberOfParts,
                numberOfNodes,
                firstLocalNode,
                beyondLocalNode,
                numberOfEdges,
                outputDirectory,
                os);

  os << edges << std::endl;

  os.close();
}

bool CNetwork::haveValidPartition(const int & parts)
{
  bool haveValidPartition = (parts > 0);

#pragma omp parallel for reduction(& \
                                   : haveValidPartition)
  for (int i = 0; i < parts; ++i)
    {
      if (!haveValidPartition)
        continue;

      bool valid = true;
      std::ostringstream File;
      File << mFile << "." << i;

      std::ifstream is;

      if (CDirEntry::isFile(File.str()))
        {
          json_t * pJson = CSimConfig::loadJsonPreamble(File.str(), 0);

          valid &= pJson != NULL;

          json_t * pPartition = json_object_get(pJson, "partition");

          valid &= pPartition != NULL;

          json_t * pValue = json_object_get(pPartition, "numberOfParts");

          valid &= (json_is_integer(pValue) && json_integer_value(pValue) == parts);

          pValue = json_object_get(pJson, "encoding");

          valid &= (json_is_string(pValue) && strcmp(json_string_value(pValue), "binary") == 0);

          json_decref(pJson);
        }
      else
        {
          valid = false;
        }

      haveValidPartition &= valid;
    }

  return haveValidPartition;
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

const std::map< size_t, CNode * > & CNetwork::getRemoteNodes() const
{
  return mRemoteNodes;
}

std::map< size_t, CNode * >::const_iterator CNetwork::beginRemoteNodes() const
{
  return mRemoteNodes.begin();
}

std::map< size_t, CNode * >::const_iterator CNetwork::endRemoteNodes() const
{
  return mRemoteNodes.end();
}

bool CNetwork::isRemoteNode(const CNode * pNode) const
{
  return pNode < mLocalNodes || mLocalNodes + mLocalNodesSize <= pNode;
}

bool CNetwork::isRemoteNode(const size_t & id) const
{
  return id < mLocalNodes->id || (mLocalNodes + mLocalNodesSize - 1)->id < id;
}

CNode * CNetwork::lookupNode(const size_t & id, const bool localOnly) const
{
  if (id < mFirstLocalNode || mBeyondLocalNode <= id)
    {
      if (!localOnly)
        {
          if (Context.isThread(this))
            return Context.Master().lookupNode(id, false);

          std::map< size_t, CNode * >::const_iterator found = mRemoteNodes.find(id);

          if (found != mRemoteNodes.end())
            return const_cast< CNode * >(found->second);
        }

      return NULL;
    }

  CNode * pLeft = mLocalNodes;
  CNode * pRight = mLocalNodes + mLocalNodesSize - 1;
  CNode * pCurrent = pLeft + (pRight - pLeft) / 2;

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

      pCurrent = pLeft + (pRight - pLeft) / 2;
    }

  return NULL;
}

CEdge * CNetwork::lookupEdge(const size_t & targetId, const size_t & sourceId) const
{

  // We only have edges for local target nodes
  if (targetId < mFirstLocalNode || mBeyondLocalNode <= targetId)
    {
      if (Context.isThread(this))
        return Context.Master().lookupEdge(targetId, sourceId);
      else
        return NULL;
    }

  CNode * pTargetNode = lookupNode(targetId, true);

  // Handle invalid requests
  if (pTargetNode == NULL)
    return NULL;

  // TODO CRITICAL This is broken since the edges are not sorted by source node Id
  // furthermore the targetNodeId, sourceNodeId is not a unique index
  CEdge * pLeft = pTargetNode->Edges;
  CEdge * pRight = pLeft + pTargetNode->EdgesSize;
  CEdge * pCurrent = pLeft + (pRight - pLeft) / 2;

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

      pCurrent = pLeft + (pRight - pLeft) / 2;
    }

  return NULL;
}

bool CNetwork::loadEdge(CEdge * pEdge, std::istream & is) const
{
  bool success = true;

  if (mIsBinary)
    {
      pEdge->fromBinary(is);

      success = is.good() && (mFirstLocalNode == 0 || (mFirstLocalNode <= pEdge->targetId && pEdge->targetId < mBeyondLocalNode));
      if (!success)
        CLogger::error("CEdge: Invalid binary edge encoding.");
    }
  else
    {
      char Line[1024];
      size_t LineSize = 1024;

      std::istream::pos_type p = is.tellg();

      is.getline(Line, LineSize);

      if (is.fail())
        {
          return false;
        }

      if (is.tellg() - p >= (int) LineSize)
        {
          CLogger::error("Edge line size exceeded.");
          return false;
        }

      const char * ptr = Line;
      int Read;

      static char targetActivity[128];
      static char sourceActivity[128];
      static char edgeTrait[128];

      if (5 != sscanf(ptr, "%zu,%[^,],%zu,%[^,],%lf%n", &pEdge->targetId, targetActivity, &pEdge->sourceId, sourceActivity, &pEdge->duration, &Read))
        {
          success = false;
        }

      success &= CTrait::ActivityTrait->fromString(targetActivity, pEdge->targetActivity);
      success &= CTrait::ActivityTrait->fromString(sourceActivity, pEdge->sourceActivity);

      ptr += Read;

#ifdef USE_LOCATION_ID
      if (CEdge::HasLocationId)
        {
          if (1 != sscanf(ptr, ",%zu%n", &pEdge->locationId, &Read))
            {
              success = false;
            }

          ptr += Read;
        }
#endif

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
        success = (*ptr == 0 || *ptr == '\r');

      if (!success)
        CLogger::error("CEdge: Invalid edge encoding '{}'.", Line);
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

#ifdef USE_LOCATION_ID
      if (CEdge::HasLocationId)
        {
          os << "," << pEdge->locationId;
        }
#endif

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
  CCommunicate::Send SendNodes(&CChanges::sendNodesRequested);
  CCommunicate::ClassMemberReceive< CNetwork > ReceiveNodes(this, &CNetwork::receiveNodes);
  CCommunicate::roundRobin(&SendNodes, &ReceiveNodes);

  CChanges::reset();

  return (int) CCommunicate::ErrorCode::Success;
}

CCommunicate::ErrorCode CNetwork::receiveNodes(std::istream & is, int sender)
{
  size_t BufferSize = is.rdbuf()->in_avail() / 56;
  size_t Count = 0;

#pragma omp parallel for shared(is) reduction(+ \
                                              : Count)
  for (size_t i = 0; i < BufferSize; ++i)
    {
      CNode Node;

#pragma omp critical(receive_changes)
      Node.fromBinary(is);

      CNode * pNode = lookupNode(Node.id, false);

      // CLogger::debug() << "CNetwork::receiveNodes: Receiving node: '" << Node.id << "' (" << pNode << ").";

      // TODO CSetCollector for non local nodes
      if (pNode != NULL)
        {
          Count++;
          ENABLE_TRACE(CLogger::trace("CChanges: updating node '{}'.", pNode->id););

          pNode->susceptibilityFactor = Node.susceptibilityFactor;
          pNode->susceptibility = Node.susceptibility;
          pNode->infectivityFactor = Node.infectivityFactor;
          pNode->infectivity = Node.infectivity;
          pNode->nodeTrait = Node.nodeTrait;
          pNode->setHealthState(Node.getHealthState());
        }
    }

  CLogger::debug("CChanges::receiveNodes: Receiving '{}' nodes from: '{}'.", Count, sender);

  return CCommunicate::ErrorCode::Success;
}

const size_t & CNetwork::getLocalNodeCount() const
{
  return mLocalNodesSize;
}

const size_t & CNetwork::getGlobalNodeCount() const
{
  return mTotalNodesSize;
}

const size_t & CNetwork::getLocalEdgeCount() const
{
  return mEdgesSize;
}

// static
bool CNetwork::dumpActiveNetwork()
{
  const CSimConfig::dump_active_network & dumpActiveNetwork = CSimConfig::getDumpActiveNetwork();

  // Is network dump required?
  if (dumpActiveNetwork.threshold < 0.0)
    return true;

  const CTick & CurrentTick = CActionQueue::getCurrentTick();

  // Is the current tick within the interval [startTick, endTick]
  if (CurrentTick < dumpActiveNetwork.startTick || dumpActiveNetwork.endTick < CurrentTick)
    return true;

  // Is the current tick matching the tick increments.
  // Note: dumpActiveNetwork.startTick will always be dumped but dumpActiveNetwork.endTick may be not.
  if ((CurrentTick - dumpActiveNetwork.startTick) % dumpActiveNetwork.tickIncrement != 0)
    return true;

    // We need to dump the network
    // Each thread will write its own part and the master thread will create JSON preample and combine the individual files.

#pragma omp parallel
  {
    CNetwork & Active = Context.Active();

    std::ostringstream File;
    File << dumpActiveNetwork.output << "." << Context.globalIndex(&Active);

    std::ofstream os;
    os.open(File.str().c_str());

    // Preserve the existing format
    bool IsBinary = Active.mIsBinary;
    Active.mIsBinary = (dumpActiveNetwork.encoding == "binary");

    size_t CurrentNode = std::numeric_limits< size_t >::max();
    Active.mDumpActiveNetwork.Nodes = 0;
    Active.mDumpActiveNetwork.Edges = 0;

    CEdge * pEdge = Active.beginEdge();
    CEdge * pEndEdge = Active.endEdge();

    for (; pEdge != pEndEdge; ++pEdge)
      if (pEdge->active
          && pEdge->weight >= dumpActiveNetwork.threshold)
        {
          if (CurrentNode != pEdge->targetId)
            {
              CurrentNode = pEdge->targetId;
              ++Active.mDumpActiveNetwork.Nodes;
            }

          Active.writeEdge(pEdge, os);
          ++Active.mDumpActiveNetwork.Edges;
        }

    Active.mIsBinary = IsBinary;

    os.close();
  }

  if (CCommunicate::LocalProcesses() > 1)
    {
      CNetwork & Master = Context.Master();

      Master.mDumpActiveNetwork.Nodes = 0;
      Master.mDumpActiveNetwork.Edges = 0;

      CNetwork * pIt = Context.beginThread();
      CNetwork * pEnd = Context.endThread();

      for (; pIt != pEnd; ++pIt)
        {
          Master.mDumpActiveNetwork.Nodes += pIt->mDumpActiveNetwork.Nodes;
          Master.mDumpActiveNetwork.Edges += pIt->mDumpActiveNetwork.Edges;
        }
    }

  // Construct the complete network based on the parts
  if (CCommunicate::MPIProcesses > 1)
    {
      dump_active_network DumpActiveNetwork = Context.Master().mDumpActiveNetwork;

      CCommunicate::ClassMemberReceive< CNetwork > Receive(&Context.Master(), &CNetwork::receiveDump);
      CCommunicate::master(0, &DumpActiveNetwork, sizeof(dump_active_network), sizeof(dump_active_network), &Receive);
    }

  if (CCommunicate::MPIRank != 0)
    return true;

  return Context.Master().concatenateDump();
}

CCommunicate::ErrorCode CNetwork::receiveDump(std::istream & is, int sender)
{
  if (CCommunicate::MPIRank == 0
      && sender != CCommunicate::MPIRank)
    {
      dump_active_network DumpActiveNetwork;
      is.read(reinterpret_cast< char * >(&DumpActiveNetwork), sizeof(dump_active_network));

      mDumpActiveNetwork.Nodes += DumpActiveNetwork.Nodes;
      mDumpActiveNetwork.Edges += DumpActiveNetwork.Edges;
    }

  return CCommunicate::ErrorCode::Success;
}

bool CNetwork::concatenateDump()
{
  const CSimConfig::dump_active_network & dumpActiveNetwork = CSimConfig::getDumpActiveNetwork();

  json_t * pValue = json_object_get(mpJson, "encoding");

  if (json_is_string(pValue))
    {
      json_string_set(pValue, dumpActiveNetwork.encoding.c_str());
    }

  pValue = json_object_get(mpJson, "numberOfNodes");

  if (json_is_integer(pValue))
    {
      json_integer_set(pValue, mDumpActiveNetwork.Nodes);
    }

  pValue = json_object_get(mpJson, "numberOfEdges");

  if (json_is_integer(pValue))
    {
      json_integer_set(pValue, mDumpActiveNetwork.Edges);
    }

  pValue = json_object_get(mpJson, "partition");

  if (json_is_object(pValue))
    {
      json_object_del(mpJson, "partition");
    }

  std::ostringstream File;
  File << dumpActiveNetwork.output << "[" << CActionQueue::getCurrentTick() << "]";

  std::ofstream os;
  os.open(File.str().c_str());

  // write JSON preamble
  writePreamble(os);

  std::ifstream is;
  int p = 0;
  int pMax = CCommunicate::TotalProcesses();

  for (; p < pMax; ++p)
    {
      File.str("");
      File << dumpActiveNetwork.output << "." << p;
      is.open(File.str().c_str());
      os << is.rdbuf();
      is.close();

      CDirEntry::remove(File.str());
    }

  os.close();

  return true;
}
