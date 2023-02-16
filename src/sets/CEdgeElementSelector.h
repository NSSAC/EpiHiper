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

#ifndef SRC_SETS_CEDGEELEMENTSELECTOR_H_
#define SRC_SETS_CEDGEELEMENTSELECTOR_H_

#include "math/CEdgeProperty.h"
#include "math/CValueList.h"
#include "sets/CSetContent.h"

class CFieldValue;
class CFieldValueList;
class CObservable;

class CEdgeElementSelector: public CSetContent
{
public:
  CEdgeElementSelector();

  CEdgeElementSelector(const CEdgeElementSelector & src);

  CEdgeElementSelector(const json_t * json);

  virtual ~CEdgeElementSelector();

  virtual CSetContent * copy() const override;

  virtual void fromJSON(const json_t * json) override;

  virtual bool computeProtected() override;

private:
  bool all();
  bool propertySelection();
  bool propertyIn();
  bool propertyNotIn();
  bool withTargetNodeIn();
  bool withSourceNodeIn();
  bool inDBTable();
  bool withDBFieldSelection();
  bool withDBFieldWithin();
  bool withDBFieldNotWithin();

  CEdgeProperty mEdgeProperty;
  CValue * mpValue;
  CValueList * mpValueList;
  CSetContent * mpSelector;
  std::string mDBTable;
  std::string mDBField;
  CObservable * mpObservable;
  CFieldValue * mpDBFieldValue;
  CFieldValueList * mpDBFieldValueList;
  CValueInterface::pComparison mpComparison;
  std::string mSQLComparison;
  bool (CEdgeElementSelector::*mpCompute)();
};

#endif /* SRC_SETS_CEDGEELEMENTSELECTOR_H_ */
