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

#include <sstream>

#include "dependencies/CDependencyGraph.h"
#include "dependencies/CDependencyNode.h"

// Uncomment this line below to get debug print out.
// #define DEBUG_OUTPUT 1

CDependencyGraph::CDependencyGraph()
  : mObjects2Nodes()
  , mObject2Index()
{}

CDependencyGraph::CDependencyGraph(const CDependencyGraph & src):
  mObjects2Nodes(),
  mObject2Index()
{
  std::map< CDependencyNode *, CDependencyNode * > Src2New;

  NodeMap::const_iterator itSrc = src.mObjects2Nodes.begin();
  NodeMap::const_iterator endSrc = src.mObjects2Nodes.end();

  for (; itSrc != endSrc; ++itSrc)
    {
      CDependencyNode * pNode = new CDependencyNode(*itSrc->second);
      mObjects2Nodes.insert(std::make_pair(itSrc->first, pNode));
      Src2New.insert(std::make_pair(itSrc->second, pNode));
    }

  NodeMap::iterator it = mObjects2Nodes.begin();
  NodeMap::iterator end = mObjects2Nodes.end();

  for (; it != end; ++it)
    {
      it->second->updateEdges(Src2New);
    }
}

CDependencyGraph::~CDependencyGraph()
{
  clear();
}

void CDependencyGraph::clear()
{
  iterator it = mObjects2Nodes.begin();
  iterator end = mObjects2Nodes.end();

  for (; it != end; ++it)
    {
      delete (it->second);
    }

  mObjects2Nodes.clear();
}

CDependencyGraph::iterator CDependencyGraph::addObject(const CComputable * pObject)
{
  iterator found = mObjects2Nodes.find(pObject);

  if (found == mObjects2Nodes.end())
    {
      found = mObjects2Nodes.insert(std::make_pair(pObject, new CDependencyNode(pObject))).first;

      const CComputable::Set & Prerequisites = pObject->getPrerequisites();
      CComputable::Set::const_iterator it = Prerequisites.begin();
      CComputable::Set::const_iterator end = Prerequisites.end();

      for (; it != end; ++it)
        {
          iterator foundPrerequisite = mObjects2Nodes.find(*it);

          if (foundPrerequisite == mObjects2Nodes.end())
            {
              foundPrerequisite = addObject(*it);
            }

          foundPrerequisite->second->addDependent(found->second);
          found->second->addPrerequisite(foundPrerequisite->second);
        }
    }

  return found;
}

void CDependencyGraph::removeObject(const CComputable * pObject)
{
  iterator found = mObjects2Nodes.find(pObject);

  if (found == mObjects2Nodes.end()) return;

  found->second->remove();
  delete found->second;
  mObjects2Nodes.erase(found);
}

void CDependencyGraph::removePrerequisite(const CComputable * pObject, const CComputable * pPrerequisite)
{
  iterator foundObject = mObjects2Nodes.find(pObject);
  iterator foundPrerequisite = mObjects2Nodes.find(pPrerequisite);

  if (foundObject == mObjects2Nodes.end() ||
      foundPrerequisite == mObjects2Nodes.end())
    return;

  foundObject->second->removePrerequisite(foundPrerequisite->second);
  foundPrerequisite->second->removeDependent(foundObject->second);
}

bool CDependencyGraph::getUpdateSequence(CComputable::Sequence & updateSequence,
    const CComputable::Set & changedObjects,
    const CComputable::Set & requestedObjects,
    const CComputable::Set & calculatedObjects) const
{
  bool success = true;

  const_iterator found;
  const_iterator notFound = mObjects2Nodes.end();

  std::vector<CComputable*> UpdateSequence;

#ifdef DEBUG_OUTPUT
  std::cout << "Changed:" << std::endl;
#endif // DEBUG_OUTPUT

  if (found != notFound)
    {
      success &= found->second->updateDependentState(changedObjects, true);
#ifdef DEBUG_OUTPUT
      std::cout << *static_cast< const CDataObject * >(mpContainer->getRandomObject()) << std::endl;
#endif // DEBUG_OUTPUT
    }

  CComputable::Set::const_iterator it = changedObjects.begin();
  CComputable::Set::const_iterator end = changedObjects.end();

  // Mark all nodes which are changed or need to be calculated
  for (; it != end && success; ++it)
    {
      // Issue 1170: We need to add elements of the stoichiometry, reduced stoichiometry,
      // and link matrices, i.e., we have data objects which may change
#ifdef DEBUG_OUTPUT
      if ((*it)->getDataObject() != *it)
        {
          std::cout << *static_cast< const CMathObject * >(*it) << std::endl;
        }
      else
        {
          std::cout << *static_cast< const CDataObject * >(*it) << std::endl;
        }

#endif // DEBUG_OUTPUT

      found = mObjects2Nodes.find(*it);

      if (found != notFound)
        {
          success &= found->second->updateDependentState(changedObjects, true);
        }
    }

  if (!success) goto finish;

  // Mark all nodes which have already been calculated and its prerequisites as not changed.
  it = calculatedObjects.begin();
  end = calculatedObjects.end();

#ifdef DEBUG_OUTPUT
  std::cout << "Up To Date:" << std::endl;
#endif // DEBUG_OUTPUT

  // Mark all nodes which are requested and its prerequisites.
  for (; it != end && success; ++it)
    {
#ifdef DEBUG_OUTPUT

      // Issue 1170: We need to add elements of the stoichiometry, reduced stoichiometry,
      // and link matrices, i.e., we have data objects which may change
      if ((*it)->getDataObject() != *it)
        {
          std::cout << *static_cast< const CMathObject * >(*it) << std::endl;
        }
      else
        {
          std::cout << *static_cast< const CDataObject * >(*it) << std::endl;
        }

      std::cout << *static_cast< const CMathObject * >(*it) << std::endl;
#endif // DEBUG_OUTPUT

      found = mObjects2Nodes.find(*it);

      if (found != notFound)
        {
          found->second->setChanged(false);
          success &= found->second->updateCalculatedState(changedObjects, true);
        }
    }

  it = requestedObjects.begin();
  end = requestedObjects.end();

#ifdef DEBUG_OUTPUT
  std::cout << "Requested:" << std::endl;
#endif // DEBUG_OUTPUT

  // Mark all nodes which are requested and its prerequisites.
  for (; it != end && success; ++it)
    {
#ifdef DEBUG_OUTPUT
      std::cout << *it << std::endl;
#endif // DEBUG_OUTPUT

      if (*it == NULL)
        {
          success = false; // we should not have NULL elements here
          break;
        }

#ifdef DEBUG_OUTPUT
      std::cout << *static_cast< const CMathObject * >(*it) << std::endl;
#endif // DEBUG_OUTPUT

      found = mObjects2Nodes.find(*it);

      if (found != notFound)
        {
          found->second->setRequested(true);
          success &= found->second->updatePrerequisiteState(changedObjects, true);
        }
    }

  if (!success) goto finish;

#ifdef DEBUG_OUTPUT
  {
    std::ofstream GetUpdateSequence("GetUpdateSequence.dot");
    exportDOTFormat(GetUpdateSequence, "GetUpdateSequence");
    GetUpdateSequence.close();
  }
#endif // DEBUG_OUTPUT

  it = requestedObjects.begin();
  end = requestedObjects.end();

  for (; it != end; ++it)
    {
      found = mObjects2Nodes.find(*it);

      if (found != notFound)
        {
          success &= found->second->buildUpdateSequence(UpdateSequence, false);
          continue;
        }

      // This is not an error we may have objects which are not part of the dependency tree
      // success = false;
    }

  if (!success) goto finish;

finish:
  const_iterator itCheck = mObjects2Nodes.begin();
  const_iterator endCheck = mObjects2Nodes.end();

  for (; itCheck != endCheck; ++itCheck)
    {
      // Reset the dependency nodes for the next call.
      itCheck->second->reset();
    }

  if (!success)
    {
      UpdateSequence.clear();
    }

  updateSequence = UpdateSequence;

#ifdef DEBUG_OUTPUT
  CComputable::Sequence::const_iterator itSeq = updateSequence.begin();
  CComputable::Sequence::const_iterator endSeq = updateSequence.end();

  std::cout << std::endl <<  "Start" << std::endl;

  for (; itSeq != endSeq; ++itSeq)
    {
      if (dynamic_cast< const CMathObject * >(*itSeq))
        {
          std::cout << *static_cast< const CMathObject * >(*itSeq);
        }
      else
        {
          std::cout << (*itSeq)->getCN() << std::endl;
        }
    }

  std::cout << "End" << std::endl;
#endif // DEBUG_OUTPUT

  return success;
}

bool CDependencyGraph::dependsOn(const CComputable * pObject,
                                     const CComputable * pChangedObject) const
{
  CComputable::Sequence UpdateSequence;
  CComputable::Set ChangedObjects;

  if (pChangedObject != NULL)
    {
      ChangedObjects.insert(pChangedObject);
    }

  CComputable::Set RequestedObjects;

  if (pObject != NULL)
    {
      RequestedObjects.insert(pObject);
    }

  getUpdateSequence(UpdateSequence, ChangedObjects, RequestedObjects);

  return !UpdateSequence.empty();
}

bool CDependencyGraph::dependsOn(const CComputable * pObject,
                                     const CComputable::Set & changedObjects) const
{
  CComputable::Sequence UpdateSequence;
  CComputable::Set RequestedObjects;

  if (pObject != NULL)
    {
      RequestedObjects.insert(pObject);
    }

  getUpdateSequence(UpdateSequence, changedObjects, RequestedObjects);

  return !UpdateSequence.empty();
}

bool CDependencyGraph::hasCircularDependencies(const CComputable * pObject,
    const CComputable * pChangedObject) const
{
  CComputable::Sequence UpdateSequence;
  CComputable::Set ChangedObjects;

  if (pChangedObject != NULL)
    {
      ChangedObjects.insert(pChangedObject);
    }

  CComputable::Set RequestedObjects;

  if (pObject != NULL)
    {
      RequestedObjects.insert(pObject);
    }

  bool hasCircularDependencies = !getUpdateSequence(UpdateSequence, ChangedObjects, RequestedObjects);

  return hasCircularDependencies;
}

bool CDependencyGraph::appendDirectDependents(const CComputable::Set & changedObjects,
    CComputable::Set & dependentObjects) const
{
  dependentObjects.erase(NULL);
  size_t Size = dependentObjects.size();

  CComputable::Set::const_iterator it = changedObjects.begin();
  CComputable::Set::const_iterator end = changedObjects.end();

  for (; it != end; ++it)
    {
      NodeMap::const_iterator found = mObjects2Nodes.find(*it);

      if (found != mObjects2Nodes.end())
        {
          std::vector< CDependencyNode * >::const_iterator itNode = found->second->getDependents().begin();
          std::vector< CDependencyNode * >::const_iterator endNode = found->second->getDependents().end();

          for (; itNode != endNode; ++itNode)
            {
              dependentObjects.insert((*itNode)->getObject());
            }
        }
    }

  dependentObjects.erase(NULL);

  return dependentObjects.size() > Size;
}

bool CDependencyGraph::appendAllDependents(const CComputable::Set & changedObjects,
    CComputable::Set & dependentObjects,
    const CComputable::Set & ignoredObjects) const
{
  bool success = true;

  dependentObjects.erase(NULL);
  size_t Size = dependentObjects.size();

  const_iterator found;
  const_iterator notFound = mObjects2Nodes.end();

  std::vector<CComputable*> UpdateSequence;

  CComputable::Set::const_iterator it = changedObjects.begin();
  CComputable::Set::const_iterator end = changedObjects.end();

#ifdef DEBUG_OUTPUT
  std::cout << "Changed:" << std::endl;
#endif // DEBUG_OUTPUT

  // Mark all nodes which are changed or need to be calculated
  for (; it != end && success; ++it)
    {
      // Issue 1170: We need to add elements of the stoichiometry, reduced stoichiometry,
      // and link matrices, i.e., we have data objects which may change
#ifdef DEBUG_OUTPUT
      if ((*it)->getDataObject() != *it)
        {
          std::cout << *static_cast< const CMathObject * >(*it) << std::endl;
        }
      else
        {
          std::cout << *static_cast< const CDataObject * >(*it) << std::endl;
        }

#endif // DEBUG_OUTPUT

      found = mObjects2Nodes.find(*it);

      if (found != notFound)
        {
          success &= found->second->updateDependentState(changedObjects, true);
        }
    }

  // Mark all nodes which are ignored and thus break dependencies.
  it = ignoredObjects.begin();
  end = ignoredObjects.end();

#ifdef DEBUG_OUTPUT
  std::cout << "Ignored:" << std::endl;
#endif // DEBUG_OUTPUT

  // Mark all nodes which are requested and its prerequisites.
  for (; it != end && success; ++it)
    {
#ifdef DEBUG_OUTPUT

      // Issue 1170: We need to add elements of the stoichiometry, reduced stoichiometry,
      // and link matrices, i.e., we have data objects which may change
      if ((*it)->getDataObject() != *it)
        {
          std::cout << *static_cast< const CMathObject * >(*it) << std::endl;
        }
      else
        {
          std::cout << *static_cast< const CDataObject * >(*it) << std::endl;
        }

#endif // DEBUG_OUTPUT

      found = mObjects2Nodes.find(*it);

      if (found != notFound)
        {
          success &= found->second->updateIgnoredState(changedObjects, true);
        }
    }

#ifdef DEBUG_OUTPUT
  {
    std::ofstream AppendAllDependents("AppendAllDependents.dot");
    exportDOTFormat(AppendAllDependents, "AppendAllDependents");
    AppendAllDependents.close();
  }
#endif // DEBUG_OUTPUT

  const_iterator itCheck = mObjects2Nodes.begin();
  const_iterator endCheck = mObjects2Nodes.end();

  for (; itCheck != endCheck; ++itCheck)
    {
      if (itCheck->second->isChanged())
        {
          dependentObjects.insert(itCheck->first);
        }

      // Reset the dependency nodes for the next call.
      itCheck->second->reset();
    }

  dependentObjects.erase(NULL);

  return dependentObjects.size() > Size;
}

void CDependencyGraph::exportDOTFormat(std::ostream & os, const std::string & name) const
{
  os << "digraph " << name << " {" << std::endl;
  os << "rankdir=LR;" << std::endl;

  mObject2Index.clear();

  const_iterator it = mObjects2Nodes.begin();
  const_iterator end = mObjects2Nodes.end();

  for (; it != end; ++it)
    {
      const CComputable * pObject = it->second->getObject();

      const std::vector< CDependencyNode * > & Dependents = it->second->getDependents();
      std::vector< CDependencyNode * >::const_iterator itDep = Dependents.begin();
      std::vector< CDependencyNode * >::const_iterator endDep = Dependents.end();

      for (; itDep != endDep; ++itDep)
        {
          os << "\"";
          os << getDOTNodeId(pObject);
          os << ((it->second->isChanged()) ? "\\nC" : "\\no");
          os << ((it->second->isRequested()) ? "R" : "o");
          os << "\"";
          os << " -> ";
          os << "\"";
          os << getDOTNodeId((*itDep)->getObject());
          os << (((*itDep)->isChanged()) ? "\\nC" : "\\no");
          os << (((*itDep)->isRequested()) ? "R" : "o");
          os << "\"";
          os << ";" << std::endl;
        }
    }

  os << "}" << std::endl;
}

// static
std::string CDependencyGraph::getDOTNodeId(const CComputable * pObject) const
{

}
