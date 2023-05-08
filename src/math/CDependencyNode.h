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

#ifndef SRC_MATH_CDEPENDENCYNODE_H_
#define SRC_MATH_CDEPENDENCYNODE_H_

#include <set>
#include <vector>
#include <map>

#include "math/CComputable.h"

class CDependencyNode
{
  // Operations
private:
  CDependencyNode(void);

public:
  /**
   * Specific constructor
   * @param const CComputable * pComputable
   */
  CDependencyNode(const CComputable * pComputable);

  /**
   * Copy constructor
   */
  CDependencyNode(const CDependencyNode & src);

  /**
   * Destructor
   */
  ~CDependencyNode(void);

  /**
   * Retrieve a pointer to the object the node is representing
   * @return   const CComputable * pComputable
   */
  const CComputable * getComputable() const;

  /**
   * Add a prerequisite
   * @param CMathDependencyNode * pNode
   */
  void addPrerequisite(CDependencyNode * pNode);

  /**
   * Remove a prerequisite
   * @param CMathDependencyNode * pNode
   */
  void removePrerequisite(CDependencyNode * pNode);

  /**
   * Retrieve the prerequisites
   * @return std::vector< CMathDependencyNode * > prerequisites
   */
  std::vector< CDependencyNode * > & getPrerequisites();

  /**
   * Add a dependent
   * @param CMathDependencyNode * pNode
   */
  void addDependent(CDependencyNode * pNode);

  /**
   * Remove a dependent
   * @param CMathDependencyNode * pNode
   */
  void removeDependent(CDependencyNode * pNode);

  /**
   * Retrieve the dependents
   * @return std::vector< CMathDependencyNode * > dependents
   */
  std::vector< CDependencyNode * > & getDependents();

  /**
   * Update the state of all dependents (and dependents thereof) to changed,
   * @param const CComputableSet & changedComputables
   * @param bool ignoreCircularDependecies
   * @return bool success
   */
  bool updateDependentState(const CComputableSet & changedComputables,
                            bool ignoreCircularDependecies);

  /**
   * Update the state of all prerequisites (and prerequisites thereof) to requested.
   * @param const CComputableSet & changedComputables
   * @param bool ignoreCircularDependecies
   * @return bool success
   */
  bool updatePrerequisiteState(const CComputableSet & changedComputables,
                               bool ignoreCircularDependecies);

  /**
   * Update the state of all prerequisites (and prerequisites thereof) to calculate.
   * @param const CComputableSet & changedComputables
   * @param bool ignoreCircularDependecies
   * @return bool success
   */
  bool updateCalculatedState(const CComputableSet & changedComputables,
                             bool ignoreCircularDependecies);

  /**
   * Update the state of all dependents (and dependents thereof) to changed,
   * @param const CComputableSet & changedComputables
   * @param bool ignoreCircularDependecies
   * @return bool success
   */
  bool updateIgnoredState(const CComputableSet & changedComputables,
                          bool ignoreCircularDependecies);

  /**
   * Build the sequence of objects which need to be updated to calculate the object value.
   * @param std::vector < CComputable * > & updateSequence
   * @param bool ignoreCircularDependecies
   * @return bool success
   */
  bool buildUpdateSequence(std::vector < CComputable * > & updateSequence,
                           bool ignoreCircularDependecies);

  /**
   * Build the sequence of objects which need to be updated to calculate the object value.
   * @param std::vector < std::vector < CComputable * > > & processGroups
   * @param bool ignoreCircularDependecies
   * @return bool success
   */
  bool buildProcessGroups(std::vector < std::vector < CComputable * > > & processGroups,
                          bool ignoreCircularDependecies);

  /**
   * Set whether the current node has changed its value
   * @param const bool & changed
   */
  void setChanged(const bool & changed);

  /**
   * Check whether the current nodes value is changed
   * @return const bool & isChanged
   */
  const bool & isChanged() const;

  /**
   * Set whether the current node's value is requested
   * @param const bool & requested
   */
  void setRequested(const bool & requested);

  /**
   * Check whether the current node's value is requested
   * @param const bool & isRequested
   */
  const bool & isRequested() const;

  /**
   * Reset the flags requested and changed
   */
  void reset();

  void remove();

  void updateEdges(const std::map< CDependencyNode *, CDependencyNode * > & map);

  void updateMaxChildGroupIndex(const int & childIndex);

  // Attributes
private:
  bool createMessage(bool ignoreCircularDependecies);

  const CComputable * mpComputable;
  std::vector< CDependencyNode * > mPrerequisites;
  std::vector< CDependencyNode * > mDependents;
  bool mChanged;
  bool mRequested;

  /**
   * The index of the process group in which the CComputable associated with the node can be computed.
   * 
   */
  int mMaxChildGroupIndex;
};

#endif /* SRC_MATH_CDEPENDENCYNODE_H_ */
