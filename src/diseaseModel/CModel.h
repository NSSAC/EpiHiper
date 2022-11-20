// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
// All rights reserved 
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

  static state_t StateToType(const CHealthState * pState);

  static CHealthState * StateFromType(const state_t & type);

  static const bool & isValid();

  static bool ProcessTransmissions();

  static void StateChanged(CNode * pNode);

  static const std::vector< CTransmission > & GetTransmissions();

  static int UpdateGlobalStateCounts();

  static CCommunicate::ErrorCode ReceiveGlobalStateCounts(std::istream & is, int sender);

  static void InitGlobalStateCountOutput();

  static void WriteGlobalStateCounts();

  static const CProgression * NextProgression(const CModel::state_t & state); 

  const CProgression * nextProgression(const CModel::state_t & state) const; 

  const CHealthState * getStates() const;

  const size_t & getStateCount() const;

  const std::vector< CTransmission > & getTransmissions() const;

  state_t stateToType(const CHealthState * pState) const;

  CHealthState * stateFromType(const state_t & type) const;
private:
  struct PossibleTransmissions
  {
    CTransmission ** Transmissions;
  };

  struct PossibleProgressions
  {
    double A0;
    std::vector< const CProgression * > Progressions;
  };

  bool processTransmissions() const;
  void stateChanged(CNode * pNode) const;

  CHealthState * mStates;
  size_t mStateCount;
  std::map< std::string, CHealthState * > mId2State;
  CHealthState const * mpInitialState;
  std::vector< CTransmission > mTransmissions;
  std::vector< CProgression > mProgressions;
  PossibleTransmissions * mPossibleTransmissions;
  PossibleProgressions * mPossibleProgressions;
  const double * mpTransmissibility;
  bool mValid;
};

#endif /* SRC_DISEASEMODEL_CMODEL_H_ */
