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

#ifndef SRC_DISEASEMODEL_MODEL_H_
#define SRC_DISEASEMODEL_MODEL_H_

#include <map>
#include <set>
#include <string>
#include <vector>

#include "utilities/Annotation.h"

class State;
class Transmission;
class Progression;
class NodeData;

struct json_t;

class Model: public Annotation
{
private:
  static Model * INSTANCE;

  /**
   * Default constructor
   */
  Model() = delete;

  /**
   * Copy construnctor
   * @param const std::string & modelFile
   */
  Model(const std::string & modelFile);

  /**
   * Destructor
   */
  virtual ~Model();

public:
  typedef size_t state_t;

  static void load(const std::string & modelFile);
  static void release();

  void fromJSON(const json_t * json);

  static const State & getInitialState();

  static state_t stateToType(const State * pState);

  static State * stateFromType(const state_t & type);

  static const bool & isValid();

  static bool processTransmissions();

  static void stateChanged(NodeData * pNode);

  static const std::vector< Transmission > & getTransmissions();

private:
  bool _processTransmissions() const;
  void _stateChanged(NodeData * pNode) const;

  State * mStates;
  std::map< std::string, State * > mId2State;
  State const * mpInitialState;
  std::vector< Transmission > mTransmissions;
  std::vector< Progression > mProgressions;
  std::map< const State *, std::map< const State *,  const Transmission * > > mPossibleTransmissions;
  std::map< const State *, std::vector< const Progression * > > mPossibleProgressions;
  bool mValid;
};

#endif /* SRC_DISEASEMODEL_MODEL_H_ */
