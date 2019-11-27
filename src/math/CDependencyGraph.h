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

#ifndef SRC_MATH_CDEPENDENCYGRAPH_H_
#define SRC_MATH_CDEPENDENCYGRAPH_H_

#include <map>
#include <set>
#include <vector>

#include "math/CComputable.h"

class CDependencyNode;

class CDependencyGraph
{
public:
  typedef std::map< const CComputable *, CDependencyNode * > NodeMap;
  typedef NodeMap::iterator iterator;
  typedef NodeMap::const_iterator const_iterator;


  static void buildGraph();

  static void applyUpdateSequence();

  static void addRequested(CComputable * pComputable);

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
   * @param const CComputable::Set & changedComputables
   * @param const CComputable::Set & requestedComputables
   * @param const CComputable::Set & calculatedComputables (default: none)
   * @return bool success
   */
  bool getUpdateSequence(CComputable::Sequence & updateSequence,
                         const CComputable::Set & changedComputables,
                         const CComputable::Set & requestedComputables,
                         const CComputable::Set & calculatedComputables = CComputable::Set()) const;

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
   * @param const CComputable::Set & changedComputables
   * @return bool dependsOn
   */
  bool dependsOn(const CComputable * pComputable,
                 const CComputable::Set & changedComputables) const;

  /**
   * Check whether the given object depends on the changed object in given context
   * @param const CComputable * pChangedComputable
   * @return bool dependsOn
   */
  bool hasCircularDependencies(const CComputable * pComputable,
                               const CComputable * pChangedComputable) const;

  /**
   * Append all objects which directly depend on the given changed objects
   * @param const CComputable::Set & changedComputables
   * @param CComputable::Set & dependentComputables
   * @return bool objectsAppendee
   */
  bool appendDirectDependents(const CComputable::Set & changedComputables,
                              CComputable::Set & dependentComputables) const;

  /**
   * Append all objects which depend on the given changed objects
   * @param const CComputable::Set & changedComputables
   * @param CComputable::Set & dependentComputables
   * @return bool objectsAppendee
   */
  bool appendAllDependents(const CComputable::Set & changedComputables,
                           CComputable::Set & dependentComputables,
                           const CComputable::Set & ignoredComputables = CComputable::Set()) const;

  void exportDOTFormat(std::ostream & os, const std::string & name) const;

private:
  static CDependencyGraph INSTANCE;
  static CComputable::Sequence UPDATE_SEQUENCE;
  static CComputable::Set REQUESTED;


  std::string getDOTNodeId(const CComputable * pComputable) const;

  // Attributes
  NodeMap mComputables2Nodes;

  mutable std::map< const CComputable *, size_t > mComputable2Index;
};

#endif /* SRC_MATH_CDEPENDENCYGRAPH_H_ */
