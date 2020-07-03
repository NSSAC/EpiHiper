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

#ifndef SRC_INTERVENTION_CSET_H_
#define SRC_INTERVENTION_CSET_H_

#include <set>
#include <map>

#include "utilities/CAnnotation.h"
#include "math/CComputable.h"

#include "sets/CSetContent.h"

class CSet : public CSetContent, public CAnnotation
{
public:
  enum struct Type
  {
    global,
    local
  };

  CSet() = delete;

  CSet(const CSet & src);

  CSet(const json_t * json);

  virtual ~CSet();

  virtual CSetContent * copy() const override;

  const std::string & getId() const;

  const bool & isValid() const;

  virtual void fromJSON(const json_t * json) override;

  virtual bool computeProtected() override;

  virtual bool contains(CNode * pNode) const override;

  virtual bool contains(CEdge * pEdge) const override;

  virtual bool contains(const CValueInterface & value) const override;

  virtual std::vector< CEdge * >::const_iterator beginEdges() const override;

  virtual std::vector< CEdge * >::const_iterator endEdges() const override;

  virtual std::vector< CNode * >::const_iterator beginNodes() const override;

  virtual std::vector< CNode * >::const_iterator endNodes() const override;

  virtual const std::vector< CEdge * > & getEdges() const override;

  virtual const std::vector< CNode * > & getNodes() const override;

  virtual const CDBFieldValues & getDBFieldValues() const override;

  virtual const CContext< SetContent > & getContext() const override;

  virtual std::vector< CEdge * > & getEdges() override;

  virtual std::vector< CNode * > & getNodes() override;

  virtual CDBFieldValues & getDBFieldValues() override;

private:
  std::string mId;
  Type mType;
  CSetContent * mpSetContent;
  bool mValid;
};

#endif /* SRC_INTERVENTION_CSET_H_ */
