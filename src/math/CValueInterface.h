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

/*
 * CValueInterface.h
 *
 *  Created on: Aug 7, 2019
 *      Author: shoops
 */

#ifndef SRC_MATH_CVALUEINTERFACE_H_
#define SRC_MATH_CVALUEINTERFACE_H_

#include <iostream>

#include "traits/CTraitData.h"
#include "diseaseModel/CModel.h"

class CValueInterface
{
public:
  enum struct Type
  {
    boolean,
    number,
    healthState,
    traitData,
    traitValue,
    string,
    id
  };

  CValueInterface() = delete;

  CValueInterface(const Type & type, void * pValue);

  CValueInterface(const CValueInterface & src);

  CValueInterface(bool & boolean);

  CValueInterface(double & number);

  CValueInterface(size_t & healthState);

  CValueInterface(CTraitData::base & traitData);

  CValueInterface(CTraitData::value & traitValue);

  CValueInterface(std::string & str);

  CValueInterface(CValueInterface & src);

  virtual ~CValueInterface();

  const bool & toBoolean() const;

  const double & toNumber() const;

  const size_t & toId() const;

  const CTraitData::base & toTraitData() const;

  const CTraitData::value & toTraitValue() const;

  const std::string & toString() const;

  const Type & getType() const;

  void toBinary(std::ostream & os) const;

  void fromBinary(std::istream & is);

  bool operator<(const CValueInterface & rhs) const;

  bool operator<=(const CValueInterface & rhs) const;

  bool operator>(const CValueInterface & rhs) const;

  bool operator>=(const CValueInterface & rhs) const;

  bool operator==(const CValueInterface & rhs) const;

  bool operator!=(const CValueInterface & rhs) const;

protected:
  Type mType;
  void * mpValue;


};

#endif /* SRC_MATH_CVALUEINTERFACE_H_ */
