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

#include "math/CDependencyNodeIterator.h"

#include "math/CDependencyNode.h"

CDependencyNodeIterator::CStackElement::CStackElement():
  mpNode(NULL),
  mType(Dependents),
  mItChild(),
  mEndChild(),
  mpParent(NULL)
{}

CDependencyNodeIterator::CStackElement::CStackElement(const CDependencyNodeIterator::CStackElement & src):
  mpNode(src.mpNode),
  mType(src.mType),
  mItChild(src.mItChild),
  mEndChild(src.mEndChild),
  mpParent(src.mpParent)
{}

CDependencyNodeIterator::CStackElement::CStackElement(CDependencyNode * pNode,
    const CDependencyNodeIterator::Type & type,
    const CDependencyNode * pParent):
  mpNode(pNode),
  mType(type),
  mItChild(),
  mEndChild(),
  mpParent(pParent)
{
  if (pNode != NULL)
    {
      switch (mType)
        {
          case Dependents:
            mItChild = mpNode->getDependents().begin();
            mEndChild = mpNode->getDependents().end();
            break;

          case Prerequisites:
            mItChild = mpNode->getPrerequisites().begin();
            mEndChild = mpNode->getPrerequisites().end();
            break;
        }
    }
}

CDependencyNodeIterator::CStackElement::~CStackElement()
{}

CDependencyNodeIterator::CDependencyNodeIterator():
  mStack(),
  mVisited(),
  mType(Dependents),
  mCurrentState(End),
  mProcessingModes(After | End | Recursive)
{}

CDependencyNodeIterator::CDependencyNodeIterator(const CDependencyNodeIterator & src):
  mStack(src.mStack),
  mVisited(src.mVisited),
  mType(src.mType),
  mCurrentState(src.mCurrentState),
  mProcessingModes(src.mProcessingModes)
{}

CDependencyNodeIterator::CDependencyNodeIterator(CDependencyNode * pNode,
    const CDependencyNodeIterator::Type & type):
  mStack(),
  mVisited(),
  mType(type),
  mCurrentState(Start),
  mProcessingModes(After | End | Recursive)
{
  mStack.push(CStackElement(pNode, mType, NULL));
  mVisited.insert(pNode);
}

CDependencyNodeIterator::~CDependencyNodeIterator()
{}

void CDependencyNodeIterator::increment()
{
  // We must not further process any recursive
  if (mCurrentState == Recursive)
    {
      // We ignore all child elements of this node, i.e. we are after;
      mCurrentState = After;
    }

  if (mStack.empty())
    {
      mCurrentState = End;

      return;
    }

  CStackElement & Current = mStack.top();

  if (mCurrentState != After)
    {
      if (Current.mItChild != Current.mEndChild)
        {
          CDependencyNode * pNode = *Current.mItChild;

          mStack.push(CStackElement(*Current.mItChild, Current.mType, Current.mpNode));

          // This will process any children since the iterator is context unaware.
          // It is therefore expected that we encounter recursive dependencies for
          // intensive/extensive value pairs.
          Current.mItChild++;

          if (mVisited.find(pNode) != mVisited.end())
            {
              // Indicate that this node is already in the stack and processing would lead to an
              // infinite recursion.
              mCurrentState = Recursive;
            }
          else
            {
              mVisited.insert(pNode);
              mCurrentState = Before;
            }

          return;
        }

      if (Current.mItChild == Current.mEndChild)
        {
          mCurrentState = After;

          return;
        }
    }

  mVisited.erase(Current.mpNode);
  mStack.pop();

  if (mStack.empty())
    {
      mCurrentState = End;

      return;
    }

  CStackElement & Parent = mStack.top();

  if (Parent.mItChild != Parent.mEndChild)
    {
      mCurrentState = Intermediate;
    }
  else
    {
      mCurrentState = After;
    }

  return;
}

bool CDependencyNodeIterator::next()
{
  if (mCurrentState != Start)
    {
      increment();
    }
  else
    {
      mCurrentState = Before;
    }

  while (!mProcessingModes.isSet(mCurrentState))
    {
      increment();
    }

  return mCurrentState & ~End;
}

const CDependencyNodeIterator::State & CDependencyNodeIterator::skipChildren()
{
  CStackElement & Current = mStack.top();
  Current.mItChild = Current.mEndChild;
  mCurrentState = After;

  return mCurrentState;
}

CDependencyNode * CDependencyNodeIterator::operator*()
{
  return mStack.top().mpNode;
}

CDependencyNode * CDependencyNodeIterator::operator->()
{
  return mStack.top().mpNode;
}

const CDependencyNode * CDependencyNodeIterator::parent()
{
  return mStack.top().mpParent;
}

const CDependencyNodeIterator::State & CDependencyNodeIterator::state() const
{
  return mCurrentState;
}

size_t CDependencyNodeIterator::level() const
{
  return mStack.size();
}

void CDependencyNodeIterator::setProcessingModes(const CDependencyNodeIterator::Flag & processingModes)
{
  mProcessingModes = (processingModes | End | Recursive);
}

CDependencyNodeIterator::Flag CDependencyNodeIterator::getProcessingModes() const
{
  return (mProcessingModes & ~(CDependencyNodeIterator::Flag(End) | Recursive));
}
