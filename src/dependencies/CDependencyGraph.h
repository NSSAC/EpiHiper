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

#ifndef SRC_DEPENDENCIES_CDEPENDENCYGRAPH_H_
#define SRC_DEPENDENCIES_CDEPENDENCYGRAPH_H_

#include <map>
#include <set>
#include <vector>

#include "dependencies/CComputable.h"

class CDependencyNode;

class CDependencyGraph
{
public:
  typedef std::map< const CComputable *, CDependencyNode * > NodeMap;
  typedef NodeMap::iterator iterator;
  typedef NodeMap::const_iterator const_iterator;

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
   * @param const CComputable * pObject
   * @return std::map< const CComputable *, CMathDependencyNode * >::iterator itObject
   */
  iterator addObject(const CComputable * pObject);

  /**
   * Remove an object and all its prerequisites to the tree
   * @param const CComputable * pObject
   */
  void removeObject(const CComputable * pObject);

  /**
   * Remove a prerequisite of an object
   * @param const CComputable * pObject
   * @param const CComputable * pPrerequisite
   */
  void removePrerequisite(const CComputable * pObject, const CComputable * pPrerequisite);

  /**
   * Construct a update sequence for the given context. Please note the calculated objects
   * must be calculated based on the same changed values and context.
   * @param CComputable::Sequence & updateSequence
   * @param const CComputable::Set & changedObjects
   * @param const CComputable::Set & requestedObjects
   * @param const CComputable::Set & calculatedObjects (default: none)
   * @return bool success
   */
  bool getUpdateSequence(CComputable::Sequence & updateSequence,
                         const CComputable::Set & changedObjects,
                         const CComputable::Set & requestedObjects,
                         const CComputable::Set & calculatedObjects = CComputable::Set()) const;

  /**
   * Check whether the given object depends on the changed object in given context
   * @param const CComputable * pObject
   * @param const CComputable * pChangedObject
   * @return bool dependsOn
   */
  bool dependsOn(const CComputable * pObject,
                 const CComputable * pChangedObject) const;

  /**
   * Check whether the given object depends on any of the changed objects in given context
   * @param const CComputable * pObject
   * @param const CComputable::Set & changedObjects
   * @return bool dependsOn
   */
  bool dependsOn(const CComputable * pObject,
                 const CComputable::Set & changedObjects) const;

  /**
   * Check whether the given object depends on the changed object in given context
   * @param const CComputable * pChangedObject
   * @return bool dependsOn
   */
  bool hasCircularDependencies(const CComputable * pObject,
                               const CComputable * pChangedObject) const;

  /**
   * Append all objects which directly depend on the given changed objects
   * @param const CComputable::Set & changedObjects
   * @param CComputable::Set & dependentObjects
   * @return bool objectsAppendee
   */
  bool appendDirectDependents(const CComputable::Set & changedObjects,
                              CComputable::Set & dependentObjects) const;

  /**
   * Append all objects which depend on the given changed objects
   * @param const CComputable::Set & changedObjects
   * @param CComputable::Set & dependentObjects
   * @return bool objectsAppendee
   */
  bool appendAllDependents(const CComputable::Set & changedObjects,
                           CComputable::Set & dependentObjects,
                           const CComputable::Set & ignoredObjects = CComputable::Set()) const;

  void exportDOTFormat(std::ostream & os, const std::string & name) const;

private:
  std::string getDOTNodeId(const CComputable * pObject) const;

  // Attributes
  NodeMap mObjects2Nodes;

  mutable std::map< const CComputable *, size_t > mObject2Index;
};

#endif /* SRC_DEPENDENCIES_CDEPENDENCYGRAPH_H_ */
