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

#include "math/CDependencyGraph.h"

#include <sstream>

#include "math/CDependencyNode.h"

// Uncomment this line below to get debug print out.
// #define DEBUG_OUTPUT 1

CDependencyGraph::CDependencyGraph()
  : mComputables2Nodes()
  , mComputable2Index()
{}

CDependencyGraph::CDependencyGraph(const CDependencyGraph & src):
  mComputables2Nodes(),
  mComputable2Index()
{
  std::map< CDependencyNode *, CDependencyNode * > Src2New;

  NodeMap::const_iterator itSrc = src.mComputables2Nodes.begin();
  NodeMap::const_iterator endSrc = src.mComputables2Nodes.end();

  for (; itSrc != endSrc; ++itSrc)
    {
      CDependencyNode * pNode = new CDependencyNode(*itSrc->second);
      mComputables2Nodes.insert(std::make_pair(itSrc->first, pNode));
      Src2New.insert(std::make_pair(itSrc->second, pNode));
    }

  NodeMap::iterator it = mComputables2Nodes.begin();
  NodeMap::iterator end = mComputables2Nodes.end();

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
  iterator it = mComputables2Nodes.begin();
  iterator end = mComputables2Nodes.end();

  for (; it != end; ++it)
    {
      delete (it->second);
    }

  mComputables2Nodes.clear();
}

CDependencyGraph::iterator CDependencyGraph::addComputable(const CComputable * pComputable)
{
  iterator found = mComputables2Nodes.find(pComputable);

  if (found == mComputables2Nodes.end())
    {
      found = mComputables2Nodes.insert(std::make_pair(pComputable, new CDependencyNode(pComputable))).first;

      const CComputable::Set & Prerequisites = pComputable->getPrerequisites();
      CComputable::Set::const_iterator it = Prerequisites.begin();
      CComputable::Set::const_iterator end = Prerequisites.end();

      for (; it != end; ++it)
        {
          iterator foundPrerequisite = mComputables2Nodes.find(*it);

          if (foundPrerequisite == mComputables2Nodes.end())
            {
              foundPrerequisite = addComputable(*it);
            }

          foundPrerequisite->second->addDependent(found->second);
          found->second->addPrerequisite(foundPrerequisite->second);
        }
    }

  return found;
}

void CDependencyGraph::removeComputable(const CComputable * pComputable)
{
  iterator found = mComputables2Nodes.find(pComputable);

  if (found == mComputables2Nodes.end()) return;

  found->second->remove();
  delete found->second;
  mComputables2Nodes.erase(found);
}

void CDependencyGraph::removePrerequisite(const CComputable * pComputable, const CComputable * pPrerequisite)
{
  iterator foundComputable = mComputables2Nodes.find(pComputable);
  iterator foundPrerequisite = mComputables2Nodes.find(pPrerequisite);

  if (foundComputable == mComputables2Nodes.end() ||
      foundPrerequisite == mComputables2Nodes.end())
    return;

  foundComputable->second->removePrerequisite(foundPrerequisite->second);
  foundPrerequisite->second->removeDependent(foundComputable->second);
}

bool CDependencyGraph::getUpdateSequence(CComputable::Sequence & updateSequence,
    const CComputable::Set & changedComputables,
    const CComputable::Set & requestedComputables,
    const CComputable::Set & calculatedComputables) const
{
  bool success = true;

  const_iterator found;
  const_iterator notFound = mComputables2Nodes.end();

  std::vector<CComputable*> UpdateSequence;

#ifdef DEBUG_OUTPUT
  std::cout << "Changed:" << std::endl;
#endif // DEBUG_OUTPUT

  if (found != notFound)
    {
      success &= found->second->updateDependentState(changedComputables, true);
#ifdef DEBUG_OUTPUT
      std::cout << *static_cast< const CDataComputable * >(mpContainer->getRandomComputable()) << std::endl;
#endif // DEBUG_OUTPUT
    }

  CComputable::Set::const_iterator it = changedComputables.begin();
  CComputable::Set::const_iterator end = changedComputables.end();

  // Mark all nodes which are changed or need to be calculated
  for (; it != end && success; ++it)
    {
      // Issue 1170: We need to add elements of the stoichiometry, reduced stoichiometry,
      // and link matrices, i.e., we have data objects which may change
#ifdef DEBUG_OUTPUT
      if ((*it)->getDataComputable() != *it)
        {
          std::cout << *static_cast< const CMathComputable * >(*it) << std::endl;
        }
      else
        {
          std::cout << *static_cast< const CDataComputable * >(*it) << std::endl;
        }

#endif // DEBUG_OUTPUT

      found = mComputables2Nodes.find(*it);

      if (found != notFound)
        {
          success &= found->second->updateDependentState(changedComputables, true);
        }
    }

  if (!success) goto finish;

  // Mark all nodes which have already been calculated and its prerequisites as not changed.
  it = calculatedComputables.begin();
  end = calculatedComputables.end();

#ifdef DEBUG_OUTPUT
  std::cout << "Up To Date:" << std::endl;
#endif // DEBUG_OUTPUT

  // Mark all nodes which are requested and its prerequisites.
  for (; it != end && success; ++it)
    {
#ifdef DEBUG_OUTPUT

      // Issue 1170: We need to add elements of the stoichiometry, reduced stoichiometry,
      // and link matrices, i.e., we have data objects which may change
      if ((*it)->getDataComputable() != *it)
        {
          std::cout << *static_cast< const CMathComputable * >(*it) << std::endl;
        }
      else
        {
          std::cout << *static_cast< const CDataComputable * >(*it) << std::endl;
        }

      std::cout << *static_cast< const CMathComputable * >(*it) << std::endl;
#endif // DEBUG_OUTPUT

      found = mComputables2Nodes.find(*it);

      if (found != notFound)
        {
          found->second->setChanged(false);
          success &= found->second->updateCalculatedState(changedComputables, true);
        }
    }

  it = requestedComputables.begin();
  end = requestedComputables.end();

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
      std::cout << *static_cast< const CMathComputable * >(*it) << std::endl;
#endif // DEBUG_OUTPUT

      found = mComputables2Nodes.find(*it);

      if (found != notFound)
        {
          found->second->setRequested(true);
          success &= found->second->updatePrerequisiteState(changedComputables, true);
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

  it = requestedComputables.begin();
  end = requestedComputables.end();

  for (; it != end; ++it)
    {
      found = mComputables2Nodes.find(*it);

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
  const_iterator itCheck = mComputables2Nodes.begin();
  const_iterator endCheck = mComputables2Nodes.end();

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
      if (dynamic_cast< const CMathComputable * >(*itSeq))
        {
          std::cout << *static_cast< const CMathComputable * >(*itSeq);
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

bool CDependencyGraph::dependsOn(const CComputable * pComputable,
                                     const CComputable * pChangedComputable) const
{
  CComputable::Sequence UpdateSequence;
  CComputable::Set ChangedComputables;

  if (pChangedComputable != NULL)
    {
      ChangedComputables.insert(pChangedComputable);
    }

  CComputable::Set RequestedComputables;

  if (pComputable != NULL)
    {
      RequestedComputables.insert(pComputable);
    }

  getUpdateSequence(UpdateSequence, ChangedComputables, RequestedComputables);

  return !UpdateSequence.empty();
}

bool CDependencyGraph::dependsOn(const CComputable * pComputable,
                                     const CComputable::Set & changedComputables) const
{
  CComputable::Sequence UpdateSequence;
  CComputable::Set RequestedComputables;

  if (pComputable != NULL)
    {
      RequestedComputables.insert(pComputable);
    }

  getUpdateSequence(UpdateSequence, changedComputables, RequestedComputables);

  return !UpdateSequence.empty();
}

bool CDependencyGraph::hasCircularDependencies(const CComputable * pComputable,
    const CComputable * pChangedComputable) const
{
  CComputable::Sequence UpdateSequence;
  CComputable::Set ChangedComputables;

  if (pChangedComputable != NULL)
    {
      ChangedComputables.insert(pChangedComputable);
    }

  CComputable::Set RequestedComputables;

  if (pComputable != NULL)
    {
      RequestedComputables.insert(pComputable);
    }

  bool hasCircularDependencies = !getUpdateSequence(UpdateSequence, ChangedComputables, RequestedComputables);

  return hasCircularDependencies;
}

bool CDependencyGraph::appendDirectDependents(const CComputable::Set & changedComputables,
    CComputable::Set & dependentComputables) const
{
  dependentComputables.erase(NULL);
  size_t Size = dependentComputables.size();

  CComputable::Set::const_iterator it = changedComputables.begin();
  CComputable::Set::const_iterator end = changedComputables.end();

  for (; it != end; ++it)
    {
      NodeMap::const_iterator found = mComputables2Nodes.find(*it);

      if (found != mComputables2Nodes.end())
        {
          std::vector< CDependencyNode * >::const_iterator itNode = found->second->getDependents().begin();
          std::vector< CDependencyNode * >::const_iterator endNode = found->second->getDependents().end();

          for (; itNode != endNode; ++itNode)
            {
              dependentComputables.insert((*itNode)->getComputable());
            }
        }
    }

  dependentComputables.erase(NULL);

  return dependentComputables.size() > Size;
}

bool CDependencyGraph::appendAllDependents(const CComputable::Set & changedComputables,
    CComputable::Set & dependentComputables,
    const CComputable::Set & ignoredComputables) const
{
  bool success = true;

  dependentComputables.erase(NULL);
  size_t Size = dependentComputables.size();

  const_iterator found;
  const_iterator notFound = mComputables2Nodes.end();

  std::vector<CComputable*> UpdateSequence;

  CComputable::Set::const_iterator it = changedComputables.begin();
  CComputable::Set::const_iterator end = changedComputables.end();

#ifdef DEBUG_OUTPUT
  std::cout << "Changed:" << std::endl;
#endif // DEBUG_OUTPUT

  // Mark all nodes which are changed or need to be calculated
  for (; it != end && success; ++it)
    {
      // Issue 1170: We need to add elements of the stoichiometry, reduced stoichiometry,
      // and link matrices, i.e., we have data objects which may change
#ifdef DEBUG_OUTPUT
      if ((*it)->getDataComputable() != *it)
        {
          std::cout << *static_cast< const CMathComputable * >(*it) << std::endl;
        }
      else
        {
          std::cout << *static_cast< const CDataComputable * >(*it) << std::endl;
        }

#endif // DEBUG_OUTPUT

      found = mComputables2Nodes.find(*it);

      if (found != notFound)
        {
          success &= found->second->updateDependentState(changedComputables, true);
        }
    }

  // Mark all nodes which are ignored and thus break dependencies.
  it = ignoredComputables.begin();
  end = ignoredComputables.end();

#ifdef DEBUG_OUTPUT
  std::cout << "Ignored:" << std::endl;
#endif // DEBUG_OUTPUT

  // Mark all nodes which are requested and its prerequisites.
  for (; it != end && success; ++it)
    {
#ifdef DEBUG_OUTPUT

      // Issue 1170: We need to add elements of the stoichiometry, reduced stoichiometry,
      // and link matrices, i.e., we have data objects which may change
      if ((*it)->getDataComputable() != *it)
        {
          std::cout << *static_cast< const CMathComputable * >(*it) << std::endl;
        }
      else
        {
          std::cout << *static_cast< const CDataComputable * >(*it) << std::endl;
        }

#endif // DEBUG_OUTPUT

      found = mComputables2Nodes.find(*it);

      if (found != notFound)
        {
          success &= found->second->updateIgnoredState(changedComputables, true);
        }
    }

#ifdef DEBUG_OUTPUT
  {
    std::ofstream AppendAllDependents("AppendAllDependents.dot");
    exportDOTFormat(AppendAllDependents, "AppendAllDependents");
    AppendAllDependents.close();
  }
#endif // DEBUG_OUTPUT

  const_iterator itCheck = mComputables2Nodes.begin();
  const_iterator endCheck = mComputables2Nodes.end();

  for (; itCheck != endCheck; ++itCheck)
    {
      if (itCheck->second->isChanged())
        {
          dependentComputables.insert(itCheck->first);
        }

      // Reset the dependency nodes for the next call.
      itCheck->second->reset();
    }

  dependentComputables.erase(NULL);

  return dependentComputables.size() > Size;
}

void CDependencyGraph::exportDOTFormat(std::ostream & os, const std::string & name) const
{
  os << "digraph " << name << " {" << std::endl;
  os << "rankdir=LR;" << std::endl;

  mComputable2Index.clear();

  const_iterator it = mComputables2Nodes.begin();
  const_iterator end = mComputables2Nodes.end();

  for (; it != end; ++it)
    {
      const CComputable * pComputable = it->second->getComputable();

      const std::vector< CDependencyNode * > & Dependents = it->second->getDependents();
      std::vector< CDependencyNode * >::const_iterator itDep = Dependents.begin();
      std::vector< CDependencyNode * >::const_iterator endDep = Dependents.end();

      for (; itDep != endDep; ++itDep)
        {
          os << "\"";
          os << getDOTNodeId(pComputable);
          os << ((it->second->isChanged()) ? "\\nC" : "\\no");
          os << ((it->second->isRequested()) ? "R" : "o");
          os << "\"";
          os << " -> ";
          os << "\"";
          os << getDOTNodeId((*itDep)->getComputable());
          os << (((*itDep)->isChanged()) ? "\\nC" : "\\no");
          os << (((*itDep)->isRequested()) ? "R" : "o");
          os << "\"";
          os << ";" << std::endl;
        }
    }

  os << "}" << std::endl;
}

// static
std::string CDependencyGraph::getDOTNodeId(const CComputable * pComputable) const
{

}
