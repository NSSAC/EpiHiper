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

#ifndef SRC_INTERVENTION_CSETCONTENT_H_
#define SRC_INTERVENTION_CSETCONTENT_H_

#include "intervention/CValueList.h"
#include "dependencies/CComputable.h"

class CNode;
class CEdge;

class CSetContent: public CComputable
{
public:
  static CSetContent * create(const json_t * json);
  static CSetContent * copy(const CSetContent * pSetContent);
  static void destroy(CSetContent *& pSetContent);

  CSetContent();

  CSetContent(const CSetContent & src);

  virtual ~CSetContent();

  const bool & isValid() const;

protected:
  std::set< CNode * > mNodes;
  std::set< CEdge * > mEdges;
  std::map< CValueList::Type, std::set< CValue > > mDBFieldValues;
  bool mValid;
};

#endif /* SRC_INTERVENTION_CSETCONTENT_H_ */
