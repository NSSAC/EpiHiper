// BEGIN: Copyright 
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
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
#include "utilities/CCommunicate.h"

class CHealthState;
class CTransmission;
class CProgression;
class CNode;

struct json_t;

class CModel: public CAnnotation
{
private:
  static CModel * INSTANCE;

public:
  /**
   * Default constructor
   */
  CModel() = delete;

  /**
   * Specific construnctor
   * @param const std::string & modelFile
   */
  CModel(const std::string & modelFile);

  /**
   * Destructor
   */
  virtual ~CModel();

  typedef size_t state_t;

  static void Load(const std::string & modelFile);
  static void Release();

  virtual void fromJSON(const json_t * json) override;

  static const CHealthState & GetInitialState();

  static CHealthState * GetState(const std::string & id);

  static CHealthState * StateFromType(const state_t & type);

  static const bool & isValid();

  static bool ProcessTransmissions();

  static void StateChanged(CNode * pNode);

  static const std::vector< CTransmission > & GetTransmissions();

  static const std::vector< CHealthState > & GetStates();

  static const std::vector< CProgression > & GetProgressions();

  static int UpdateGlobalStateCounts();

  static CCommunicate::ErrorCode ReceiveGlobalStateCounts(std::istream & is, int sender);

  static void InitGlobalStateCountOutput();

  static void WriteGlobalStateCounts();

  const std::vector< CHealthState > & getStates() const;

  const size_t & getStateCount() const;

  const std::vector< CTransmission > & getTransmissions() const;

  const std::vector< CProgression > & getProgressions() const;

  CHealthState * stateFromType(const state_t & type);

private:
  struct PossibleTransmissions
  {
    CTransmission ** Transmissions = nullptr;
  };

  bool processTransmissions() const;
  void stateChanged(CNode * pNode) const;

  std::vector< CHealthState > mStates;
  size_t mStateCount;
  std::map< std::string, CHealthState * > mId2State;
  CHealthState const * mpInitialState;
  std::vector< CTransmission > mTransmissions;
  std::vector< CProgression > mProgressions;
  PossibleTransmissions * mPossibleTransmissions;

  const double * mpTransmissibility;
  bool mValid;
};

#endif /* SRC_DISEASEMODEL_CMODEL_H_ */
