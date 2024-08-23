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

#ifndef SRC_SETS_CNODEELEMENTSELECTOR_H_
#define SRC_SETS_CNODEELEMENTSELECTOR_H_

#include <string>

#include "math/CNodeProperty.h"
#include "math/CValueList.h"
#include "sets/CSetContent.h"

class CFieldValue;
class CFieldValueList;
class CObservable;
class CVariable;
class CSetCollectorInterface;

class CNodeElementSelector: public CSetContent
{
public:
  CNodeElementSelector();

  CNodeElementSelector(const CNodeElementSelector & src);

  CNodeElementSelector(const json_t * json);

  virtual ~CNodeElementSelector();

  virtual void determineIsStatic() override;

  virtual FilterType filterType() const override;

  virtual bool filter(const CNode * pNode) const override;

protected:
  virtual bool computeSetContent() override;

  virtual void fromJSONProtected(const json_t * json) override;

  virtual bool lessThanProtected(const CSetContent & rhs) const override;

  virtual void setScopeProtected() override;

private:
  static CNodeElementSelector * pALL;
  
  bool all();
  bool propertySelection();
  bool withIncomingEdgeIn();
  bool withIncomingEdgeNotIn();
  bool dbAll();
  bool dbSelection();
  bool dbIn();
  bool dbNotIn();

  bool filterPropertySelection(const CNode * pNode) const;
  bool filterPropertyIn(const CNode * pNode) const;
  bool filterPropertyNotIn(const CNode * pNode) const;
  
  CNodeProperty mNodeProperty;
  CValue * mpValue;
  CValueList * mpValueList;
  CSetContent::shared_pointer mpSelector;
  std::string mDBTable;
  std::string mDBField;
  CObservable * mpObservable;
  CVariable * mpVariable;
  CFieldValue * mpDBFieldValue;
  CFieldValueList * mpDBFieldValueList;
  CValueInterface::pComparison mpComparison;
  std::string mSQLComparison;
  bool (CNodeElementSelector::*mpCompute)();
  bool (CNodeElementSelector::*mpFilter)(const CNode *) const;
  std::shared_ptr< CSetCollectorInterface > mpCollector;
};

#endif /* SRC_SETS_CNODEELEMENTSELECTOR_H_ */
