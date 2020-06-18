// BEGIN: Copyright 
// Copyright (C) 2019 - 2020 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

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
  bool propertyWithin();
  bool withNodeIn();
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
  CEdgeProperty::pGetNode mpGetNode;
  bool (CEdgeElementSelector::*mpCompute)();
};

#endif /* SRC_SETS_CEDGEELEMENTSELECTOR_H_ */
