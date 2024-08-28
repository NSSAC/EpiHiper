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

#ifndef SRC_SETS_CEDGEELEMENTSELECTOR_H_
#define SRC_SETS_CEDGEELEMENTSELECTOR_H_

#include <memory>
#include "math/CEdgeProperty.h"
#include "math/CValueList.h"
#include "sets/CSetContent.h"

class CFieldValue;
class CFieldValueList;
class CObservable;
class CVariable;

class CEdgeElementSelector: public CSetContent
{
public:
  CEdgeElementSelector();

  CEdgeElementSelector(const CEdgeElementSelector & src);

  CEdgeElementSelector(const json_t * json);

  virtual ~CEdgeElementSelector();

  virtual void fromJSONProtected(const json_t * json) override;

  virtual void determineIsStatic() override;

  virtual FilterType filterType() const override;

  virtual bool filter(const CEdge * pEdge) const override;

protected:
  virtual bool computeSetContent() override;

  virtual bool lessThanProtected(const CSetContent & rhs) const override;

  virtual void setScopeProtected() override;

private:
  bool all();
  bool propertySelection();
  bool withTargetNodeIn();
  bool withTargetNodeNotIn();
  bool withSourceNodeIn();
  bool withSourceNodeNotIn();
  bool dbAll();
  bool dbSelection();
  bool dbIn();
  bool dbNotIn();

  bool filterPropertySelection(const CEdge * pEdge) const;
  bool filterPropertyIn(const CEdge * pEdge) const;
  bool filterPropertyNotIn(const CEdge * pEdge) const;
  
  CEdgeProperty mEdgeProperty;
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
  bool (CEdgeElementSelector::*mpCompute)();
  bool (CEdgeElementSelector::*mpFilter)(const CEdge *) const;
};

#endif /* SRC_SETS_CEDGEELEMENTSELECTOR_H_ */
