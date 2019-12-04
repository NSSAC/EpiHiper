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

#include "math/CDependencyNode.h"

#include "math/CComputable.h"
#include "math/CDependencyNodeIterator.h"

CDependencyNode::CDependencyNode():
  mpComputable(NULL),
  mPrerequisites(),
  mDependents(),
  mChanged(false),
  mRequested(false)
{}

CDependencyNode::CDependencyNode(const CComputable * pComputable):
  mpComputable(pComputable),
  mPrerequisites(),
  mDependents(),
  mChanged(false),
  mRequested(false)
{}

CDependencyNode::CDependencyNode(const CDependencyNode & src):
  mpComputable(src.mpComputable),
  mPrerequisites(src.mPrerequisites),
  mDependents(src.mDependents),
  mChanged(src.mChanged),
  mRequested(src.mRequested)
{}

CDependencyNode::~CDependencyNode()
{}

const CComputable * CDependencyNode::getComputable() const
{
  return mpComputable;
}

void CDependencyNode::addPrerequisite(CDependencyNode * pComputable)
{
  mPrerequisites.push_back(pComputable);
}

void CDependencyNode::removePrerequisite(CDependencyNode * pNode)
{
  std::vector< CDependencyNode * >::iterator it = mPrerequisites.begin();
  std::vector< CDependencyNode * >::iterator end = mPrerequisites.end();

  for (; it != end; ++it)
    if (*it == pNode)
      {
        mPrerequisites.erase(it);
        break;
      }
}

std::vector< CDependencyNode * > & CDependencyNode::getPrerequisites()
{
  return mPrerequisites;
}

void CDependencyNode::addDependent(CDependencyNode * pNode)
{
  mDependents.push_back(pNode);
}

void CDependencyNode::removeDependent(CDependencyNode * pNode)
{
  std::vector< CDependencyNode * >::iterator it = mDependents.begin();
  std::vector< CDependencyNode * >::iterator end = mDependents.end();

  for (; it != end; ++it)
    if (*it == pNode)
      {
        mDependents.erase(it);
        break;
      }
}

std::vector< CDependencyNode * > & CDependencyNode::getDependents()
{
  return mDependents;
}

bool CDependencyNode::updateDependentState(const CComputableSet & changedComputables,
    bool ignoreCircularDependecies)
{
  bool success = true;

  CDependencyNodeIterator itNode(this, CDependencyNodeIterator::Dependents);
  itNode.setProcessingModes(CDependencyNodeIterator::Before);

  while (itNode.next())
    {
      // If we have a recursive dependency we need make sure that this is due to
      // an intensive/extensive value pair
      if (itNode.state() == CDependencyNodeIterator::Recursive)
        {
          success &= itNode->createMessage(ignoreCircularDependecies);
          continue;
        }

      // The node itself is not modified.
      if (*itNode == this)
        {
          continue;
        }

      // We are guaranteed that the current node has a parent as the only node without is this,
      // which is handled above.
      if (!itNode->isChanged())
        {
          itNode->setChanged(true);
        }
      else
        {
          itNode.skipChildren();
        }
    }

  return success;
}

bool CDependencyNode::updatePrerequisiteState(const CComputableSet & changedComputables,
    bool ignoreCircularDependecies)
{
  bool success = true;

  CDependencyNodeIterator itNode(this, CDependencyNodeIterator::Prerequisites);
  itNode.setProcessingModes(CDependencyNodeIterator::Before);

  while (itNode.next())
    {
      // If we have a recursive dependency we need make sure that this is due to
      // an intensive/extensive value pair
      if (itNode.state() == CDependencyNodeIterator::Recursive)
        {
          success &= itNode->createMessage(ignoreCircularDependecies);
          continue;
        }

      // The node itself is not modified.
      if (*itNode == this)
        {
          continue;
        }

      // We are guaranteed that the current node has a parent as the only node without is this,
      // which is handled above.
      if (!itNode->isRequested() &&
          changedComputables.find(const_cast< CComputable * >(itNode->getComputable())) == changedComputables.end())
        {
          itNode->setRequested(true);
        }
      else
        {
          itNode.skipChildren();
        }
    }

  return success;
}

bool CDependencyNode::updateCalculatedState(const CComputableSet & changedComputables,
    bool ignoreCircularDependecies)
{
  bool success = true;

  CDependencyNodeIterator itNode(this, CDependencyNodeIterator::Prerequisites);
  itNode.setProcessingModes(CDependencyNodeIterator::Before);

  while (itNode.next())
    {
      // If we have a recursive dependency we need make sure that this is due to
      // an intensive/extensive value pair
      if (itNode.state() == CDependencyNodeIterator::Recursive)
        {
          success &= itNode->createMessage(ignoreCircularDependecies);
          continue;
        }

      // The node itself is not modified.
      if (*itNode == this)
        {
          continue;
        }

      // We are guaranteed that the current node has a parent as the only node without is this,
      // which is handled above.
      if (itNode->isChanged())
        {
          itNode->setChanged(false);
        }
      else
        {
          itNode.skipChildren();
        }
    }

  return success;
}

bool CDependencyNode::updateIgnoredState(const CComputableSet & changedComputables,
    bool ignoreCircularDependecies)
{
  bool success = true;

  if (isChanged())
    {
      setChanged(false);

      CDependencyNodeIterator itNode(this, CDependencyNodeIterator::Dependents);
      itNode.setProcessingModes(CDependencyNodeIterator::Before);

      while (itNode.next())
        {
          // If we have a recursive dependency we need make sure that this is due to
          // an intensive/extensive value pair
          if (itNode.state() == CDependencyNodeIterator::Recursive)
            {
              success &= itNode->createMessage(ignoreCircularDependecies);
              continue;
            }

          // The node itself is not modified.
          if (*itNode == this)
            {
              continue;
            }

          bool PrerequisiteChanged = false;
          std::vector< CDependencyNode * >::const_iterator it = itNode->getPrerequisites().begin();
          std::vector< CDependencyNode * >::const_iterator end = itNode->getPrerequisites().end();

          for (; it != end; ++it)
            if ((*it)->isChanged())
              {
                PrerequisiteChanged = true;
                break;
              }

          if (!PrerequisiteChanged)
            {
              itNode->updateIgnoredState(changedComputables, ignoreCircularDependecies);
            }
        }

      return itNode.state() == CDependencyNodeIterator::End;
    }

  return success;
}

bool CDependencyNode::buildUpdateSequence(std::vector < CComputable * > & updateSequence,
    bool ignoreCircularDependecies)
{
  bool success = true;

  if (!mChanged || !mRequested)
    return success;

  CDependencyNodeIterator itNode(this, CDependencyNodeIterator::Prerequisites);
  itNode.setProcessingModes(CDependencyNodeIterator::Flag(CDependencyNodeIterator::Before) | CDependencyNodeIterator::After);

  while (itNode.next())
    {
      const CComputable * pMathComputable = itNode->getComputable();

      switch (itNode.state())
        {
          case CDependencyNodeIterator::Recursive:
            success &= itNode->createMessage(ignoreCircularDependecies);
            break;

          case CDependencyNodeIterator::Before:

            if (!itNode->isChanged() || !itNode->isRequested())
              {
                itNode.skipChildren();
              }

            break;

          case CDependencyNodeIterator::After:

            // This check is not needed as unchanged or unrequested nodes
            // are skipped in Before processing.
            if (itNode->isChanged() && itNode->isRequested() && pMathComputable != NULL)
              {
                updateSequence.push_back(const_cast< CComputable * >(itNode->getComputable()));
                itNode->setChanged(false);
              }

            break;

          default:
            break;
        }
    }

  mChanged = false;

  return success;
}

void CDependencyNode::setChanged(const bool & changed)
{
  mChanged = changed;
}

const bool & CDependencyNode::isChanged() const
{
  return mChanged;
}

void CDependencyNode::setRequested(const bool & requested)
{
  mRequested = requested;
}

const bool & CDependencyNode::isRequested() const
{
  return mRequested;
}

void CDependencyNode::reset()
{
  mChanged = false;
  mRequested = false;
}

void CDependencyNode::remove()
{
  std::vector< CDependencyNode * >::iterator it = mPrerequisites.begin();
  std::vector< CDependencyNode * >::iterator end = mPrerequisites.end();

  for (; it != end; ++it)
    {
      (*it)->removeDependent(this);
    }

  it = mDependents.begin();
  end = mDependents.end();

  for (; it != end; ++it)
    {
      (*it)->removePrerequisite(this);
    }
}

void CDependencyNode::updateEdges(const std::map< CDependencyNode *, CDependencyNode * > & map)
{
  std::vector< CDependencyNode * >::iterator it = mPrerequisites.begin();
  std::vector< CDependencyNode * >::iterator end = mPrerequisites.end();

  for (; it != end; ++it)
    {
      std::map< CDependencyNode *, CDependencyNode * >::const_iterator found = map.find(*it);
      *it = found->second;
    }

  it = mDependents.begin();
  end = mDependents.end();

  for (; it != end; ++it)
    {
      std::map< CDependencyNode *, CDependencyNode * >::const_iterator found = map.find(*it);
      *it = found->second;
    }
}

bool CDependencyNode::createMessage(bool ignoreCircularDependecies)
{
  if (!ignoreCircularDependecies)
    {
      if (getComputable() != NULL)
        {
          // CCopasiMessage(CCopasiMessage::ERROR, MCMathModel + 3, getComputable()->getCN().c_str());
        }
      else
        {
          // CCopasiMessage(CCopasiMessage::ERROR, MCMathModel + 3, "cn not found");
        }
    }

  return ignoreCircularDependecies;
}
