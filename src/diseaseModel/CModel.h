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

#ifndef SRC_DISEASEMODEL_CMODEL_H_
#define SRC_DISEASEMODEL_CMODEL_H_

#include <map>
#include <set>
#include <string>
#include <vector>

#include "utilities/CAnnotation.h"

class CHealthState;
class CTransmission;
class CProgression;
class CNode;

struct json_t;

class CModel: public CAnnotation
{
private:
  static CModel * INSTANCE;

  /**
   * Default constructor
   */
  CModel() = delete;

  /**
   * Copy construnctor
   * @param const std::string & modelFile
   */
  CModel(const std::string & modelFile);

  /**
   * Destructor
   */
  virtual ~CModel();

public:
  typedef size_t state_t;

  static void load(const std::string & modelFile);
  static void release();

  void fromJSON(const json_t * json);

  static const CHealthState & getInitialState();

  static CHealthState * getState(const std::string & id);

  static state_t stateToType(const CHealthState * pState);

  static CHealthState * stateFromType(const state_t & type);

  static const bool & isValid();

  static bool processTransmissions();

  static void stateChanged(CNode * pNode);

  static const std::vector< CTransmission > & getTransmissions();

private:
  bool _processTransmissions() const;
  void _stateChanged(CNode * pNode) const;

  CHealthState * mStates;
  std::map< std::string, CHealthState * > mId2State;
  CHealthState const * mpInitialState;
  std::vector< CTransmission > mTransmissions;
  std::vector< CProgression > mProgressions;
  std::map< const CHealthState *, std::map< const CHealthState *,  const CTransmission * > > mPossibleTransmissions;
  std::map< const CHealthState *, std::vector< const CProgression * > > mPossibleProgressions;
  bool mValid;
};

#endif /* SRC_DISEASEMODEL_CMODEL_H_ */
