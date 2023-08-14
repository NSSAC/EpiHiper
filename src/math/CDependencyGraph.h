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

#ifndef SRC_MATH_CDEPENDENCYGRAPH_H_
#define SRC_MATH_CDEPENDENCYGRAPH_H_

#include <map>
#include <set>
#include <vector>

#include "math/CComputable.h"

#define USE_PROCESS_GROUPS 1

class CDependencyNode;

class CDependencyGraph
{
public:
  typedef std::map< const CComputable *, CDependencyNode * > NodeMap;
  typedef NodeMap::iterator iterator;
  typedef NodeMap::const_iterator const_iterator;

#ifdef USE_PROCESS_GROUPS
  typedef std::vector< CComputable::Sequence > UpdateOrder;

#else
  typedef CComputable::Sequence UpdateOrder;
#endif

  static void buildGraph();

  static bool applyUpdateOrder();

  static bool applyComputeOnceOrder();

  static bool applyUpdateOrder(UpdateOrder & updateOrder);

  static bool getUpdateOrder(UpdateOrder & updateOrder,
                             const CComputableSet & requestedComputables);

  static bool applyComputableSequence(CComputable::Sequence & updateSequence);

  // Operations
  /**
   * Constructor
   */
  CDependencyGraph();

  /**
   * Copy constructor
   */
  CDependencyGraph(const CDependencyGraph & src);

  /**
   * Destructor
   */
  ~CDependencyGraph();

  /**
   * Clear the whole dependency tree
   */
  void clear();

  /**
   * Add an object and all its prerequisites to the tree
   * @param const CComputable * pComputable
   * @return std::map< const CComputable *, CMathDependencyNode * >::iterator itComputable
   */
  iterator addComputable(const CComputable * pComputable);

  /**
   * Remove an object and all its prerequisites to the tree
   * @param const CComputable * pComputable
   */
  void removeComputable(const CComputable * pComputable);

  /**
   * Remove a prerequisite of an object
   * @param const CComputable * pComputable
   * @param const CComputable * pPrerequisite
   */
  void addPrerequisite(const CComputable * pComputable, const CComputable * pPrerequisite);

  /**
   * Remove a prerequisite of an object
   * @param const CComputable * pComputable
   * @param const CComputable * pPrerequisite
   */
  void removePrerequisite(const CComputable * pComputable, const CComputable * pPrerequisite);

  /**
   * Construct a update sequence for the given context. Please note the calculated objects
   * must be calculated based on the same changed values and context.
   * @param CComputable::Sequence & updateSequence
   * @param const CComputableSet & changedComputables
   * @param const CComputableSet & requestedComputables
   * @param const CComputableSet & calculatedComputables (default: none)
   * @return bool success
   */
  bool getUpdateSequence(CComputable::Sequence & updateSequence,
                         const CComputableSet & changedComputables,
                         const CComputableSet & requestedComputables,
                         const CComputableSet & calculatedComputables = CComputableSet()) const;

  /**
   * Construct a vector of process groups. Please note the calculated objects
   * must be calculated based on the same changed values and context.
   * @param std::vector < CComputable::Sequence > & processGroups
   * @param const CComputableSet & changedComputables
   * @param const CComputableSet & requestedComputables
   * @param const CComputableSet & calculatedComputables (default: none)
   * @return bool success
   */
  bool getProcessGroups(std::vector < CComputable::Sequence > & processGroups,
                        const CComputableSet & changedComputables,
                        const CComputableSet & requestedComputables,
                        const CComputableSet & calculatedComputables = CComputableSet()) const;

  /**
   * Check whether the given object depends on the changed object in given context
   * @param const CComputable * pComputable
   * @param const CComputable * pChangedComputable
   * @return bool dependsOn
   */
  bool dependsOn(const CComputable * pComputable,
                 const CComputable * pChangedComputable) const;

  /**
   * Check whether the given object depends on any of the changed objects in given context
   * @param const CComputable * pComputable
   * @param const CComputableSet & changedComputables
   * @return bool dependsOn
   */
  bool dependsOn(const CComputable * pComputable,
                 const CComputableSet & changedComputables) const;

  /**
   * Check whether the given object depends on the changed object in given context
   * @param const CComputable * pChangedComputable
   * @return bool dependsOn
   */
  bool hasCircularDependencies(const CComputable * pComputable,
                               const CComputable * pChangedComputable) const;

  /**
   * Append all objects which directly depend on the given changed objects
   * @param const CComputableSet & changedComputables
   * @param CComputableSet & dependentComputables
   * @return bool objectsAppendee
   */
  bool appendDirectDependents(const CComputableSet & changedComputables,
                              CComputableSet & dependentComputables) const;

  /**
   * Append all objects which depend on the given changed objects
   * @param const CComputableSet & changedComputables
   * @param CComputableSet & dependentComputables
   * @return bool objectsAppendee
   */
  bool appendAllDependents(const CComputableSet & changedComputables,
                           CComputableSet & dependentComputables,
                           const CComputableSet & ignoredComputables = CComputableSet()) const;

  void exportDOTFormat(std::ostream & os, const std::string & name) const;

private:
  static CDependencyGraph INSTANCE;
  static UpdateOrder CommonUpdateOrder;
  static CComputableSet UpToDate;
  static CComputable::Sequence ComputeOnceSequence;

  std::string getDOTNodeId(const CComputable * pComputable) const;

  // Attributes
  NodeMap mComputables2Nodes;

  mutable std::map< const CComputable *, size_t > mComputable2Index;
};

#endif /* SRC_MATH_CDEPENDENCYGRAPH_H_ */
