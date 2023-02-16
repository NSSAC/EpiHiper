// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2023 Rector and Visitors of the University of Virginia 
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

#ifndef SRC_NETWORK_CNETWORK_H_
#define SRC_NETWORK_CNETWORK_H_

#include <set>
#include <map>
#include <iostream>

#include "utilities/CAnnotation.h"
#include "utilities/CCommunicate.h"
#include "utilities/CContext.h"

struct json_t;
class CNode;
class CEdge;
class CTrait;

class CNetwork: public CAnnotation
{
private:
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

  struct dump_active_network
  {
    size_t Nodes;
    size_t Edges;
  };

public:
  static CContext< CNetwork > Context;

  static void init();
  static void release();
  static int index(const CNode * pNode);
  static int index(const size_t & id);
  static bool dumpActiveNetwork();

  /**
   * Default construnctor
   * @param const std::string & networkFile
   */
  CNetwork();

  /**
   * Destructor
   */
  virtual ~CNetwork();

  void loadJsonPreamble(const std::string & networkFile);

  void load();

  void writePreamble(std::ostream & os) const;

  void write(const std::string & file, bool binary);

  void partition(const int & parts, const bool & save, const std::string & outputDirectory = "");

  virtual void fromJSON(const json_t * json) override;

  const bool & isValid() const;

  CNode * lookupNode(const size_t & id, const bool localOnly) const;

  CEdge * lookupEdge(const size_t & targetId, const size_t & sourceId) const;

  CNode * beginNode();

  CNode * endNode();

  CEdge * beginEdge();

  CEdge * endEdge();

  const std::map< size_t, CNode * > & getRemoteNodes() const;
  
  std::map< size_t, CNode * >::const_iterator beginRemoteNodes() const;
  
  std::map< size_t, CNode * >::const_iterator endRemoteNodes() const;
  
  bool isRemoteNode(const CNode * pNode) const;

  bool isRemoteNode(const size_t & id) const;

  int broadcastChanges();

  CCommunicate::ErrorCode receiveNodes(std::istream & is, int sender);

  CCommunicate::ErrorCode receiveEdges(std::istream & is, int sender);

  const size_t & getLocalNodeCount() const;
  const size_t & getGlobalNodeCount() const;
  const size_t & getLocalEdgeCount() const;

  bool haveValidPartition(const int & parts);


  CCommunicate::ErrorCode receiveDump(std::istream & is, int sender);
  
  bool concatenateDump();

private:
  void initExternalEdges();
  void initOutgoingEdges();
  
  std::string mFile;
  CNode * mLocalNodes;
  size_t mFirstLocalNode;
  size_t mBeyondLocalNode;
  size_t mLocalNodesSize;
  CNode * mExternalNodes;
  size_t mExternalNodesSize;
  std::map< size_t, CNode *> mRemoteNodes;
  std::set< size_t > mSourceOnlyNodes;
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

  dump_active_network mDumpActiveNetwork;

};

#endif /* SRC_NETWORK_CNETWORK_H_ */
