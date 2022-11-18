// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
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

class CNodeElementSelector: public CSetContent
{
public:
  CNodeElementSelector();

  CNodeElementSelector(const CNodeElementSelector & src);

  CNodeElementSelector(const json_t * json);

  virtual ~CNodeElementSelector();

  virtual CSetContent * copy() const override;

  virtual void fromJSON(const json_t * json) override;

  virtual bool computeProtected() override;

private:
  bool all();
  bool propertySelection();
  bool propertyIn();
  bool propertyNotIn();
  bool withIncomingEdge();
  bool inDBTable();
  bool withDBFieldSelection();
  bool withDBFieldWithin();
  bool withDBFieldNotWithin();

  CNodeProperty mNodeProperty;
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
  bool mLocalScope;
  bool (CNodeElementSelector::*mpCompute)();
};

#endif /* SRC_SETS_CNODEELEMENTSELECTOR_H_ */
