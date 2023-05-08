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

#include <sstream>

#include "math/CDependencyNode.h"
#include "math/CDependencyGraph.h"
#include "actions/CActionQueue.h"
#include "actions/CConditionDefinition.h"
#include "utilities/CLogger.h"
#include "variables/CVariableList.h"

// Uncomment this line below to get debug print out.
// #define DEBUG_OUTPUT 1

// static
void CDependencyGraph::buildGraph()
{
  INSTANCE.clear();
  ComputeOnceSequence.clear();

  // We need to be smarter and not compute all.
  // Target sets should only be computed if the intervention fires.
  // Everything else which is used in a trigger or action condition needs to be computed

  CComputableSet Changed;
  
  CComputableSet::const_iterator it = CComputable::Instances.begin();
  CComputableSet::const_iterator end = CComputable::Instances.end();

  for (; it != end; ++it)
    {
      CComputable * pComputable = const_cast< CComputable * >(it->second);

      INSTANCE.addComputable(pComputable);

      if (pComputable->getPrerequisites().empty())
        {
          ComputeOnceSequence.push_back(pComputable);
          Changed.insert(pComputable);
        }
      else if (pComputable->isStatic())
        {
          CConditionDefinition::RequiredComputables.insert(pComputable);
        }
    }

  // CConditionDefinition::RequiredComputables already includes all computables for trigger and action conditions.
  INSTANCE.getUpdateSequence(UpdateSequence, Changed, CConditionDefinition::RequiredComputables);
  INSTANCE.getProcessGroups(ProcessGroups, Changed, CConditionDefinition::RequiredComputables);

  UpToDate.clear();

  CComputable::Sequence::iterator itSeq = UpdateSequence.begin();
  CComputable::Sequence::iterator endSeq = UpdateSequence.end();

  for (; itSeq != endSeq; ++itSeq)
    UpToDate.insert(*itSeq);
}

// static
bool CDependencyGraph::applyUpdateSequence()
{
  return applyUpdateSequence(UpdateSequence);
}

// static
bool CDependencyGraph::applyComputeOnceSequence()
{
  return applyUpdateSequence(ComputeOnceSequence);
}

// static
bool CDependencyGraph::applyUpdateSequence(CComputable::Sequence & updateSequence)
{
  bool success = true;
  CComputable::Sequence::iterator it = updateSequence.begin();
  CComputable::Sequence::iterator end = updateSequence.end();

  for (; it != end && success; ++it)
    success &= (*it)->compute();

  return success;
}

// static
bool CDependencyGraph::getUpdateSequence(CComputable::Sequence & updateSequence,
                                         const CComputableSet & requestedComputables)
{
  CComputableSet & Changed = CVariableList::INSTANCE.changedVariables().Master();
  Changed.insert(&CActionQueue::getCurrentTick());

  return INSTANCE.getUpdateSequence(updateSequence, Changed, requestedComputables, UpToDate);
}

// static
bool CDependencyGraph::applyProcessGroups()
{
  return applyProcessGroups(ProcessGroups);
}

// static
bool CDependencyGraph::applyProcessGroups(std::vector< CComputable::Sequence > & processGroups)
{
  bool success = true;
  std::vector< CComputable::Sequence >::iterator it = processGroups.begin();
  std::vector< CComputable::Sequence >::iterator end = processGroups.end();

  for (; it != end && success; ++it)
    success &= applyUpdateSequence(*it);

  return success;
}

// static
bool CDependencyGraph::getProcessGroups(std::vector< CComputable::Sequence > & processGroups,
                                        const CComputableSet & requestedComputables)
{
  CComputableSet & Changed = CVariableList::INSTANCE.changedVariables().Master();
  Changed.insert(&CActionQueue::getCurrentTick());

  return INSTANCE.getProcessGroups(processGroups, Changed, requestedComputables, UpToDate);
}

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

      const CComputableSet & Prerequisites = pComputable->getPrerequisites();
      CComputableSet::const_iterator it = Prerequisites.begin();
      CComputableSet::const_iterator end = Prerequisites.end();

      for (; it != end; ++it)
        {
          iterator foundPrerequisite = mComputables2Nodes.find(it->second);

          if (foundPrerequisite == mComputables2Nodes.end())
            {
              foundPrerequisite = addComputable(it->second);
            }

          foundPrerequisite->second->addDependent(found->second);
          found->second->addPrerequisite(foundPrerequisite->second);
        }
      
      // This can only be done after all prerequisites are added;
      const_cast< CComputable * >(pComputable)->determineIsStatic();
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

void CDependencyGraph::addPrerequisite(const CComputable * pComputable, const CComputable * pPrerequisite)
{
  iterator found = mComputables2Nodes.find(pComputable);

  if (found == mComputables2Nodes.end())
    {
      addComputable(pComputable);
      return;
    }

  iterator foundPrerequisite = mComputables2Nodes.find(pPrerequisite);

  if (foundPrerequisite == mComputables2Nodes.end())
    {
      foundPrerequisite = addComputable(pPrerequisite);
    }

  foundPrerequisite->second->addDependent(found->second);
  found->second->addPrerequisite(foundPrerequisite->second);
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
    const CComputableSet & changedComputables,
    const CComputableSet & requestedComputables,
    const CComputableSet & calculatedComputables) const
{
  bool success = true;

  const_iterator found;
  const_iterator notFound = mComputables2Nodes.end();

  std::vector<CComputable*> UpdateSequence;

#ifdef DEBUG_OUTPUT
  std::cout << "Changed:" << std::endl;
#endif // DEBUG_OUTPUT

  CComputableSet::const_iterator it = changedComputables.begin();
  CComputableSet::const_iterator end = changedComputables.end();

  // Mark all nodes which are changed or need to be calculated
  for (; it != end && success; ++it)
    {
      // Issue 1170: We need to add elements of the stoichiometry, reduced stoichiometry,
      // and link matrices, i.e., we have data objects which may change
#ifdef DEBUG_OUTPUT
      if (it->second->getDataComputable() != it->second)
        {
          std::cout << *static_cast< const CMathComputable * >(it->second) << std::endl;
        }
      else
        {
          std::cout << *static_cast< const CDataComputable * >(it->second) << std::endl;
        }

#endif // DEBUG_OUTPUT

      found = mComputables2Nodes.find(it->second);

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
      if (it->second->getDataComputable() != it->second)
        {
          std::cout << *static_cast< const CMathComputable * >(it->second) << std::endl;
        }
      else
        {
          std::cout << *static_cast< const CDataComputable * >(it->second) << std::endl;
        }

      std::cout << *static_cast< const CMathComputable * >(it->second) << std::endl;
#endif // DEBUG_OUTPUT

      found = mComputables2Nodes.find(it->second);

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
      std::cout << it->second << std::endl;
#endif // DEBUG_OUTPUT

      if (it->second == NULL)
        {
          success = false; // we should not have NULL elements here
          break;
        }

#ifdef DEBUG_OUTPUT
      std::cout << *static_cast< const CMathComputable * >(it->second) << std::endl;
#endif // DEBUG_OUTPUT

      found = mComputables2Nodes.find(it->second);

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
      found = mComputables2Nodes.find(it->second);

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

bool CDependencyGraph::getProcessGroups(std::vector< CComputable::Sequence > & processGroups,
                                        const CComputableSet & changedComputables,
                                        const CComputableSet & requestedComputables,
                                        const CComputableSet & calculatedComputables) const
{
  bool success = true;

  const_iterator found;
  const_iterator notFound = mComputables2Nodes.end();

  std::vector< CComputable::Sequence > ProcessGroups;

#ifdef DEBUG_OUTPUT
  std::cout << "Changed:" << std::endl;
#endif // DEBUG_OUTPUT

  CComputableSet::const_iterator it = changedComputables.begin();
  CComputableSet::const_iterator end = changedComputables.end();

  // Mark all nodes which are changed or need to be calculated
  for (; it != end && success; ++it)
    {
      // Issue 1170: We need to add elements of the stoichiometry, reduced stoichiometry,
      // and link matrices, i.e., we have data objects which may change
#ifdef DEBUG_OUTPUT
      if (it->second->getDataComputable() != it->second)
        {
          std::cout << *static_cast< const CMathComputable * >(it->second) << std::endl;
        }
      else
        {
          std::cout << *static_cast< const CDataComputable * >(it->second) << std::endl;
        }

#endif // DEBUG_OUTPUT

      found = mComputables2Nodes.find(it->second);

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
      if (it->second->getDataComputable() != it->second)
        {
          std::cout << *static_cast< const CMathComputable * >(it->second) << std::endl;
        }
      else
        {
          std::cout << *static_cast< const CDataComputable * >(it->second) << std::endl;
        }

      std::cout << *static_cast< const CMathComputable * >(it->second) << std::endl;
#endif // DEBUG_OUTPUT

      found = mComputables2Nodes.find(it->second);

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
      std::cout << it->second << std::endl;
#endif // DEBUG_OUTPUT

      if (it->second == NULL)
        {
          success = false; // we should not have NULL elements here
          break;
        }

#ifdef DEBUG_OUTPUT
      std::cout << *static_cast< const CMathComputable * >(it->second) << std::endl;
#endif // DEBUG_OUTPUT

      found = mComputables2Nodes.find(it->second);

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
      found = mComputables2Nodes.find(it->second);

      if (found != notFound)
        {
          success &= found->second->buildProcessGroups(ProcessGroups, false);
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

  processGroups = ProcessGroups;

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
  CComputableSet ChangedComputables;

  if (pChangedComputable != NULL)
    {
      ChangedComputables.insert(pChangedComputable);
    }

  CComputableSet RequestedComputables;

  if (pComputable != NULL)
    {
      RequestedComputables.insert(pComputable);
    }

  getUpdateSequence(UpdateSequence, ChangedComputables, RequestedComputables);

  return !UpdateSequence.empty();
}

bool CDependencyGraph::dependsOn(const CComputable * pComputable,
                                     const CComputableSet & changedComputables) const
{
  CComputable::Sequence UpdateSequence;
  CComputableSet RequestedComputables;

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
  CComputableSet ChangedComputables;

  if (pChangedComputable != NULL)
    {
      ChangedComputables.insert(pChangedComputable);
    }

  CComputableSet RequestedComputables;

  if (pComputable != NULL)
    {
      RequestedComputables.insert(pComputable);
    }

  bool hasCircularDependencies = !getUpdateSequence(UpdateSequence, ChangedComputables, RequestedComputables);

  return hasCircularDependencies;
}

bool CDependencyGraph::appendDirectDependents(const CComputableSet & changedComputables,
    CComputableSet & dependentComputables) const
{
  dependentComputables.erase(NULL);
  size_t Size = dependentComputables.size();

  CComputableSet::const_iterator it = changedComputables.begin();
  CComputableSet::const_iterator end = changedComputables.end();

  for (; it != end; ++it)
    {
      NodeMap::const_iterator found = mComputables2Nodes.find(it->second);

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

bool CDependencyGraph::appendAllDependents(const CComputableSet & changedComputables,
    CComputableSet & dependentComputables,
    const CComputableSet & ignoredComputables) const
{
  bool success = true;

  dependentComputables.erase(NULL);
  size_t Size = dependentComputables.size();

  const_iterator found;
  const_iterator notFound = mComputables2Nodes.end();

  std::vector<CComputable*> UpdateSequence;

  CComputableSet::const_iterator it = changedComputables.begin();
  CComputableSet::const_iterator end = changedComputables.end();

#ifdef DEBUG_OUTPUT
  std::cout << "Changed:" << std::endl;
#endif // DEBUG_OUTPUT

  // Mark all nodes which are changed or need to be calculated
  for (; it != end && success; ++it)
    {
      // Issue 1170: We need to add elements of the stoichiometry, reduced stoichiometry,
      // and link matrices, i.e., we have data objects which may change
#ifdef DEBUG_OUTPUT
      if (it->second->getDataComputable() != it->second)
        {
          std::cout << *static_cast< const CMathComputable * >(it->second) << std::endl;
        }
      else
        {
          std::cout << *static_cast< const CDataComputable * >(it->second) << std::endl;
        }

#endif // DEBUG_OUTPUT

      found = mComputables2Nodes.find(it->second);

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
      if (it->second->getDataComputable() != it->second)
        {
          std::cout << *static_cast< const CMathComputable * >(it->second) << std::endl;
        }
      else
        {
          std::cout << *static_cast< const CDataComputable * >(it->second) << std::endl;
        }

#endif // DEBUG_OUTPUT

      found = mComputables2Nodes.find(it->second);

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
  return pComputable->getComputableId();
}
