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

#include "intervention/CInitialization.h"
#include "actions/CActionDefinition.h"
#include "actions/CActionQueue.h"
#include "actions/Changes.h"
#include "actions/CConditionDefinition.h"
#include "db/CConnection.h"
#include "db/CSchema.h"
#include "diseaseModel/CModel.h"
#include "intervention/CInitialization.h"
#include "intervention/CIntervention.h"
#include "intervention/CTrigger.h"
#include "math/CComputable.h"
#include "math/CDependencyGraph.h"
#include "math/CObservable.h"
#include "math/CSizeOf.h"
#include "network/CEdge.h"
#include "network/CNetwork.h"
#include "sets/CSetReference.h"
#include "sets/CSetList.h"
#include "traits/CTrait.h"
#include "utilities/CCommunicate.h"
#include "utilities/CRandom.h"
#include "utilities/CSimConfig.h"
#include "utilities/CStatus.h"
#include "variables/CVariableList.h"

// static
std::vector< CActionDefinition * > CActionDefinition::INSTANCES;

// static
CComputableSet CConditionDefinition::RequiredComputables;

// static
CActionQueue * CActionQueue::pINSTANCE(NULL);

// static
std::set< const CNode * > Changes::Nodes;

// static
std::set< const CEdge * > Changes::Edges;

// static
std::stringstream Changes::DefaultOutput;

// static
size_t Changes::Tick = -1;

// static
CConnection * CConnection::pINSTANCE = NULL;

// static
CSchema CSchema::INSTANCE;

// static
CModel * CModel::INSTANCE(NULL);

// static
std::vector< CInitialization * > CInitialization::INSTANCES;

// static
std::map< std::string, CIntervention * > CIntervention::INSTANCES;

// static
std::vector< CTrigger * > CTrigger::INSTANCES;

// static
bool * CTrigger::pGlobalTriggered(NULL);

// static
CComputableSet CTrigger::RequiredTargets;

// static
CComputableSet CComputable::Instances;

// static
CDependencyGraph CDependencyGraph::INSTANCE;

// static
CComputable::Sequence CDependencyGraph::UpdateSequence;

// static
CComputableSet CDependencyGraph::UpToDate;

// static
CObservable::ObservableMap CObservable::Observables;

// static
std::vector< CSizeOf * > CSizeOf::INSTANCES;

// static
bool CEdge::HasEdgeTrait(false);

// static
bool CEdge::HasActiveField(false);

// static
bool CEdge::HasWeightField(false);

// static
CNetwork * CNetwork::INSTANCE(NULL);

// static
CSetList CSetList::INSTANCE;

// static
std::vector< CSetReference * > CSetReference::UnResolved;

// static
std::map< std::string, CTrait > CTrait::INSTANCES;

// static
CTrait const * CTrait::ActivityTrait;

// static
CTrait const * CTrait::EdgeTrait;

// static
CTrait const * CTrait::NodeTrait;

// static
int CCommunicate::MPIRank(-1);

// static
int CCommunicate::MPINextRank(-1);

// static
int CCommunicate::MPIPreviousRank(-1);

// static
int CCommunicate::MPIProcesses(-1);

// static
int CCommunicate::ReceiveSize(0);

// static
char * CCommunicate::ReceiveBuffer(NULL);

// static
MPI_Win CCommunicate::MPIWin;

// static
size_t CCommunicate::RMAIndex(0);

// static
size_t CCommunicate::MPIWinSize(0);

// static
double * CCommunicate::RMABuffer(NULL);

// static
CRandom::generator_t CRandom::G;

// static
CSimConfig * CSimConfig::INSTANCE(NULL);

// static
json_t * CStatus::pJSON(NULL);

// static
json_t * CStatus::pId(NULL);

// static
json_t * CStatus::pName(NULL);

// static
json_t * CStatus::pStatus(NULL);

// static
json_t * CStatus::pDetail(NULL);

// static
json_t * CStatus::pProgress(NULL);

// static
CVariableList CVariableList::INSTANCE;
