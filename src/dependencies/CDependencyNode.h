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

#ifndef SRC_DEPENDENCIES_CDEPENDENCYNODE_H_
#define SRC_DEPENDENCIES_CDEPENDENCYNODE_H_

#include <set>
#include <vector>
#include <map>

#include "dependencies/CComputable.h"

class CDependencyNode
{
  // Operations
private:
  CDependencyNode(void);

public:
  /**
   * Specific constructor
   * @param const CComputable * pObject
   */
  CDependencyNode(const CComputable * pObject);

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
   * @return   const CComputable * pObject
   */
  const CComputable * getObject() const;

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
   * @param const CComputable::Set & changedObjects
   * @param bool ignoreCircularDependecies
   * @return bool success
   */
  bool updateDependentState(const CComputable::Set & changedObjects,
                            bool ignoreCircularDependecies);

  /**
   * Update the state of all prerequisites (and prerequisites thereof) to requested.
   * @param const CComputable::Set & changedObjects
   * @param bool ignoreCircularDependecies
   * @return bool success
   */
  bool updatePrerequisiteState(const CComputable::Set & changedObjects,
                               bool ignoreCircularDependecies);

  /**
   * Update the state of all prerequisites (and prerequisites thereof) to calculate.
   * @param const CComputable::Set & changedObjects
   * @param bool ignoreCircularDependecies
   * @return bool success
   */
  bool updateCalculatedState(const CComputable::Set & changedObjects,
                             bool ignoreCircularDependecies);

  /**
   * Update the state of all dependents (and dependents thereof) to changed,
   * @param const CComputable::Set & changedObjects
   * @param bool ignoreCircularDependecies
   * @return bool success
   */
  bool updateIgnoredState(const CComputable::Set & changedObjects,
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

  // Attributes
private:
  bool createMessage(bool ignoreCircularDependecies);

  const CComputable * mpObject;
  std::vector< CDependencyNode * > mPrerequisites;
  std::vector< CDependencyNode * > mDependents;
  bool mChanged;
  bool mRequested;
};

#endif /* SRC_DEPENDENCIES_CDEPENDENCYNODE_H_ */
