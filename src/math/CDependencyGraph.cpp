// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2024 Rector and Visitors of the University of Virginia 
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
#include "sets/CSetContent.h"
#include "network/CNetwork.h"
#include "network/CEdge.h"
#include "network/CNode.h"

// static
void CDependencyGraph::buildGraph()
{
  INSTANCE.clear();
  ComputeOnceSequence.clear();
  UpToDate.clear();

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
      else if (pComputable->isStatic()
              && !INSTANCE.dependsOn(pComputable, &CActionQueue::getCurrentTick()))
        {
          // This is required so that the static computable is updated the first time it is accessed.
          INSTANCE.addPrerequisite(pComputable,  &CActionQueue::getCurrentTick());
        }
    }

  // CConditionDefinition::RequiredComputables already includes all computables for trigger and action conditions.
#ifdef USE_PROCESS_GROUPS
  INSTANCE.getProcessGroups(CommonUpdateOrder, Changed, CConditionDefinition::RequiredComputables);

  for (CComputable::Sequence & sequence : CommonUpdateOrder)
    {
      CComputable::Sequence::iterator itSeq = sequence.begin();
      CComputable::Sequence::iterator endSeq = sequence.end();

      for (; itSeq != endSeq; ++itSeq)
        UpToDate.insert(*itSeq);
    }

#else
  INSTANCE.getUpdateSequence(CommonUpdateOrder, Changed, CConditionDefinition::RequiredComputables);

  CComputable::Sequence::iterator itSeq = CommonUpdateOrder.begin();
  CComputable::Sequence::iterator endSeq = CommonUpdateOrder.end();

  for (; itSeq != endSeq; ++itSeq)
    UpToDate.insert(*itSeq);
#endif
}

// static
bool CDependencyGraph::applyUpdateOrder()
{
  return applyUpdateOrder(CommonUpdateOrder);
}

// static
bool CDependencyGraph::applyComputeOnceOrder()
{
  return applyComputableSequence(ComputeOnceSequence);
}

// static
bool CDependencyGraph::applyComputableSequence(CComputable::Sequence & updateSequence)
{
  std::vector< CSetContent::Filter< CEdge > > EdgeFilter;
  std::vector< CSetContent::Filter< CNode > > LocalNodeFilter;
  std::vector< CSetContent::Filter< CNode > > GlobalNodeFilter;

  bool success = true;
  CComputable::Sequence::iterator it = updateSequence.begin();
  CComputable::Sequence::iterator end = updateSequence.end();

  for (; it != end && success; ++it)
    {
#ifdef USE_PROCESS_GROUPS
      if (dynamic_cast< CSetContent * >(*it))
        {
          if ((*it)->needsCompute())
            {
              CSetContent * pSetContent = static_cast< CSetContent * >(*it);

              switch (pSetContent->filterType())
                {
                case CSetContent::FilterType::edge:
                  {
                    if (pSetContent && pSetContent->collectorEnabled())
                      success &= (*it)->compute();
                    else
                      {
                        CSetContent::Filter< CEdge > Filter(pSetContent);
                        Filter.start();
                        EdgeFilter.push_back(CSetContent::Filter< CEdge >(pSetContent));
                        CLogger::debug("CDependencyGraph::applyComputableSequence: adding '{}' to EdgeFilter.", pSetContent->getComputableId());
                      }
                  }
                  break;

                case CSetContent::FilterType::node:
                  {
                    if (pSetContent && pSetContent->collectorEnabled())
                      success &= (*it)->compute();
                    else
                      {
                        CSetContent::Filter< CNode > Filter(pSetContent);
                        Filter.start();
                        LocalNodeFilter.push_back(Filter);

                        if (pSetContent->getScope() == CSetContent::Scope::global)
                          GlobalNodeFilter.push_back(Filter);

                        CLogger::debug("CDependencyGraph::applyComputableSequence: adding '{}' to NodeFilter.", pSetContent->getComputableId());
                      }
                  }
                  break;

                case CSetContent::FilterType::none:
                  success &= (*it)->compute();
                  break;
                }
            }
        }
      else
        success &= (*it)->compute();
#else
      success &= (*it)->compute();
#endif
    }

  if (!EdgeFilter.empty())
    {
      CEdge * pEdge = CNetwork::Context.Active().beginEdge();
      CEdge * pEdgeEnd = CNetwork::Context.Active().endEdge();

      for (; pEdge != pEdgeEnd; ++pEdge)
        for (CSetContent::Filter< CEdge > & filter : EdgeFilter)
          filter.addMatching(pEdge);

      for (CSetContent::Filter< CEdge > & filter : EdgeFilter)
          filter.finish();

    }

  if (!LocalNodeFilter.empty())
    {
      std::map< size_t, CNode * >::const_iterator itGlobal = CNetwork::Context.Active().beginRemoteNodes();
      std::map< size_t, CNode * >::const_iterator endGlobal = CNetwork::Context.Active().endRemoteNodes();
      CNode * pNode = CNetwork::Context.Active().beginNode();
      CNode * pNodeEnd = CNetwork::Context.Active().endNode();
      CNode * pFirstLocalNode = pNode;

      if (!GlobalNodeFilter.empty())
        {
          for (; itGlobal != endGlobal && itGlobal->second < pFirstLocalNode; ++itGlobal)
            {
              // CLogger::trace("CDependencyGraph::applyComputableSequence: Active.mRemoteNodes[{}]: {}", itGlobal->first, (void *) itGlobal->second);
              for (CSetContent::Filter< CNode > & filter : GlobalNodeFilter)
                filter.addMatching(itGlobal->second);
            }
        }

      for (; pNode != pNodeEnd; ++pNode)
        for (CSetContent::Filter< CNode > & filter : LocalNodeFilter)
          filter.addMatching(pNode);

      if (!GlobalNodeFilter.empty())
        {
          for (; itGlobal != endGlobal; ++itGlobal)
            {
              // CLogger::trace("CDependencyGraph::applyComputableSequence: Active.mRemoteNodes[{}]: {}", itGlobal->first, (void *) itGlobal->second);
              for (CSetContent::Filter< CNode > & filter : GlobalNodeFilter)
                filter.addMatching(itGlobal->second);
            }
        }

      for (CSetContent::Filter< CNode > & filter : LocalNodeFilter)
        filter.finish();
    }

  return success;
}

// static 
bool CDependencyGraph::applyUpdateOrder(CDependencyGraph::UpdateOrder & updateSequence)
{
  bool success = true;

#ifdef USE_PROCESS_GROUPS

  std::vector< CComputable::Sequence >::iterator it = updateSequence.begin();
  std::vector< CComputable::Sequence >::iterator end = updateSequence.end();

  for (int level = 0; it != end && success; ++it, ++level)
    {
      CLogger::debug("CDependencyGraph::applyUpdateOrder: Level[{}] size: {}.", level, it->size());
      success &= applyComputableSequence(*it);
    }

#else

  success &= applyComputableSequence(updateSequence);

#endif

  return success;
}

// static
bool CDependencyGraph::getUpdateOrder(CDependencyGraph::UpdateOrder & updateSequence,
                                         const CComputableSet & requestedComputables)
{
  CComputableSet & Changed = CVariableList::INSTANCE.changedVariables().Master();
  Changed.insert(&CActionQueue::getCurrentTick());

#ifdef USE_PROCESS_GROUPS

  return INSTANCE.getProcessGroups(updateSequence, Changed, requestedComputables, UpToDate);

#else

  return INSTANCE.getUpdateSequence(updateSequence, Changed, requestedComputables, UpToDate);

#endif
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
      mComputables2Nodes.emplace(itSrc->first, pNode);
      Src2New.emplace(itSrc->second, pNode);
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
      found = mComputables2Nodes.emplace(pComputable, new CDependencyNode(pComputable)).first;

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

  CComputableSet::const_iterator it = changedComputables.begin();
  CComputableSet::const_iterator end = changedComputables.end();

  // Mark all nodes which are changed or need to be calculated
  for (; it != end && success; ++it)
    {
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

  // Mark all nodes which are requested and its prerequisites.
  for (; it != end && success; ++it)
    {
      found = mComputables2Nodes.find(it->second);

      if (found != notFound)
        {
          found->second->setChanged(false);
          success &= found->second->updateCalculatedState(changedComputables, true);
        }
    }

  it = requestedComputables.begin();
  end = requestedComputables.end();

  // Mark all nodes which are requested and its prerequisites.
  for (; it != end && success; ++it)
    {
      if (it->second == NULL)
        {
          success = false; // we should not have NULL elements here
          break;
        }

      found = mComputables2Nodes.find(it->second);

      if (found != notFound)
        {
          found->second->setRequested(true);
          success &= found->second->updatePrerequisiteState(changedComputables, true);
        }
    }

  if (!success) goto finish;

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

  CComputableSet::const_iterator it = changedComputables.begin();
  CComputableSet::const_iterator end = changedComputables.end();

  // Mark all nodes which are changed or need to be calculated
  for (; it != end && success; ++it)
    {
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

  // Mark all nodes which are requested and its prerequisites.
  for (; it != end && success; ++it)
    {
      found = mComputables2Nodes.find(it->second);

      if (found != notFound)
        {
          found->second->setChanged(false);
          success &= found->second->updateCalculatedState(changedComputables, true);
        }
    }

  it = requestedComputables.begin();
  end = requestedComputables.end();

  // Mark all nodes which are requested and its prerequisites.
  for (; it != end && success; ++it)
    {
      if (it->second == NULL)
        {
          success = false; // we should not have NULL elements here
          break;
        }

      found = mComputables2Nodes.find(it->second);

      if (found != notFound)
        {
          found->second->setRequested(true);
          success &= found->second->updatePrerequisiteState(changedComputables, true);
        }
    }

  if (!success) goto finish;

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
      ProcessGroups.clear();
    }

  processGroups = ProcessGroups;

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

  // Mark all nodes which are changed or need to be calculated
  for (; it != end && success; ++it)
    {
      found = mComputables2Nodes.find(it->second);

      if (found != notFound)
        {
          success &= found->second->updateDependentState(changedComputables, true);
        }
    }

  // Mark all nodes which are ignored and thus break dependencies.
  it = ignoredComputables.begin();
  end = ignoredComputables.end();

  // Mark all nodes which are requested and its prerequisites.
  for (; it != end && success; ++it)
    {
      found = mComputables2Nodes.find(it->second);

      if (found != notFound)
        {
          success &= found->second->updateIgnoredState(changedComputables, true);
        }
    }

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
