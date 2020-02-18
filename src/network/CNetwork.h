// BEGIN: Copyright 
// Copyright (C) 2019 - 2020 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#ifndef SRC_NETWORK_CNETWORK_H_
#define SRC_NETWORK_CNETWORK_H_

#include <map>
#include <iostream>

#include "utilities/CAnnotation.h"
#include "utilities/CCommunicate.h"

struct json_t;
class CNode;
class CEdge;
class CTrait;

class CNetwork: public CAnnotation
{
private:
  /**
   * Default constructor
   */
  CNetwork() = delete;

  bool loadEdge(CEdge * pEdge, std::istream & is) const;
  void writeEdge(CEdge * pEdge, std::ostream & os) const;
  void partition(std::istream & is, const int & parts, const bool & save, const std::string & outputDirectory);
  void convert(std::istream & is, const std::string & outputDirectory);
  bool openPartition(const size_t & partition,
                     const size_t & partCount,
                     const size_t & nodeCount,
                     const size_t & firstLocalNode,
                     const size_t & beyondLocalNode,
                     const size_t & edgeCount,
                     const std::string & outputDirectory,
                     std::ofstream & os);

  void writePartition(const size_t & partition,
                      const size_t & partCount,
                      const size_t & nodeCount,
                      const size_t & firstLocalNode,
                      const size_t & beyondLocalNode,
                      const size_t & edgeCount,
                      const std::string & edges,
                      const std::string & outputDirectory);


public:
  static CNetwork * INSTANCE;

  static void init();
  static void release();

  /**
   * Copy construnctor
   * @param const std::string & networkFile
   */
  CNetwork(const std::string & networkFile);

  /**
   * Destructor
   */
  virtual ~CNetwork();

  void load();
  void write(const std::string & file, bool binary);

  void partition(const int & parts, const bool & save, const std::string & outputDirectory = "");

  virtual void fromJSON(const json_t * json);

  const bool & isValid() const;

  CNode * lookupNode(const size_t & id, const bool localOnly) const;

  CEdge * lookupEdge(const size_t & targetId, const size_t & sourceId) const;

  CNode * beginNode();

  CNode * endNode();

  CEdge * beginEdge();

  CEdge * endEdge();

  std::map< size_t, CNode >::const_iterator beginRemoteNodes() const;
  
  std::map< size_t, CNode >::const_iterator endRemoteNodes() const;
  
  bool isRemoteNode(const CNode * pNode) const;

  int broadcastChanges();

  CCommunicate::ErrorCode receiveNodes(std::istream & is, int sender);

  CCommunicate::ErrorCode receiveEdges(std::istream & is, int sender);

  const size_t & getLocalNodeCount() const;
  const size_t & getGlobalNodeCount() const;

  bool haveValidPartition(const int & parts);

private:
  std::string mFile;
  CNode * mLocalNodes;
  size_t mFirstLocalNode;
  size_t mBeyondLocalNode;
  size_t mLocalNodesSize;
  std::map< size_t, CNode > mRemoteNodes;
  CEdge * mEdges;
  size_t mEdgesSize;
  size_t mTotalNodesSize;
  size_t mTotalEdgesSize;
  size_t mSizeOfPid;
  std::string mAccumulationTime;
  size_t mTimeResolution;
  bool mIsBinary;
  bool mValid;
  size_t mTotalPendingActions;
  json_t * mpJson;
};

#endif /* SRC_NETWORK_CNETWORK_H_ */
