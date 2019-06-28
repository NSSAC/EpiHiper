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

#ifndef SRC_NETWORK_NETWORK_H_
#define SRC_NETWORK_NETWORK_H_

#include <map>
#include <iostream>
#include "utilities/Annotation.h"

struct json_t;
struct NodeData;
struct EdgeData;
class Trait;

class Network: public Annotation
{
private:
  /**
   * Default constructor
   */
  Network() = delete;

  /**
   * Copy construnctor
   * @param const std::string & networkFile
   */
  Network(const std::string & networkFile);

  /**
   * Destructor
   */
  virtual ~Network();

  bool loadEdge(EdgeData * pEdge, std::istream & is) const;
  void writeEdge(EdgeData * pEdge, std::ostream & os) const;

public:
  static Network * INSTANCE;

  static void init();
  static void release();
  void load();
  void write(const std::string & file, bool binary);

  void partition(std::istream & is);

  void fromJSON(const json_t * json);

  const bool & isValid() const;

  NodeData * lookupNode(const size_t & id) const;

private:
  std::string mFile;
  NodeData * mLocalNodes;
  size_t mFirstLocalNode;
  size_t mBeyondLocalNode;
  size_t mLocalNodesSize;
  std::map< size_t, NodeData > mRemoteNodes;
  EdgeData * mEdges;
  size_t mEdgesSize;
  bool mHasEdgeTrait;
  bool mHasActiveField;
  bool mHasWeightField;
  size_t mTotalNodesSize;
  size_t mTotalEdgesSize;
  size_t mSizeOfPid;
  std::string mAccumulationTime;
  size_t mTimeResolution;
  bool mIsBinary;
  bool mValid;

  json_t * mpJson;
  const Trait * mpActivityTrait;
  const Trait * mpEdgeTrait;

};

#endif /* SRC_NETWORK_NETWORK_H_ */
