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

  /**
   * Copy construnctor
   * @param const std::string & networkFile
   */
  CNetwork(const std::string & networkFile);

  /**
   * Destructor
   */
  virtual ~CNetwork();

  bool loadEdge(CEdge * pEdge, std::istream & is) const;
  void writeEdge(CEdge * pEdge, std::ostream & os) const;

public:
  static CNetwork * INSTANCE;

  static void init();
  static void release();
  void load();
  void write(const std::string & file, bool binary);

  void partition(std::istream & is);

  void fromJSON(const json_t * json);

  const bool & isValid() const;

  CNode * lookupNode(const size_t & id) const;

  CEdge * lookupEdge(const size_t & targetId, const size_t & sourceId) const;

  CNode * beginNode();

  CNode * endNode();

  int broadcastChanges();

  CCommunicate::ErrorCode receiveNodes(std::istream & is, int sender);

  CCommunicate::ErrorCode receiveEdges(std::istream & is, int sender);

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
