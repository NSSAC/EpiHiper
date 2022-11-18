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

#include "intervention/CInitialization.h"
#include "actions/CActionDefinition.h"
#include "actions/CActionQueue.h"
#include "actions/CChanges.h"
#include "actions/CConditionDefinition.h"
#include "db/CConnection.h"
#include "db/CSchema.h"
#include "diseaseModel/CModel.h"
#include "diseaseModel/CAnalyzer.h"
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
CComputableSet CTrigger::RequiredTargets;

// static
CComputableSet CComputable::Instances;

// static
std::vector< CActionDefinition * > CActionDefinition::INSTANCES;

// static
CComputableSet CConditionDefinition::RequiredComputables;

// static 
CContext< CActionQueue::sActionQueue > CActionQueue::Context = CContext< CActionQueue::sActionQueue >();

// static
CTick CActionQueue::CurrenTick = CTick();

// static 
size_t CActionQueue::TotalPendingActions = 0;

// static 
std::stringstream CActionQueue::RemoteActions = std::stringstream();

// static 
CContext< CChanges::Changes > CChanges::Context = CContext< CChanges::Changes >();

// static 
std::map< size_t, std::set< const CNode * > > CChanges::RankToNodesRequested;

// static
size_t CChanges::Tick = -1;

// static
CConnection * CConnection::pINSTANCE = NULL;

// static
bool CConnection::required = false;

// static
CSchema CSchema::INSTANCE;

// static
CModel * CModel::INSTANCE(NULL);

// static
CAnalyzer * CAnalyzer::INSTANCE(NULL);

// static
std::vector< CInitialization * > CInitialization::INSTANCES;

// static
std::map< std::string, CIntervention * > CIntervention::INSTANCES;

// static
std::vector< CTrigger * > CTrigger::INSTANCES;

// static
bool * CTrigger::pGlobalTriggered(NULL);

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
bool CEdge::HasLocationId(false);

// static
bool CEdge::HasEdgeTrait(false);

// static
bool CEdge::HasActiveField(false);

// static
bool CEdge::HasWeightField(false);

// static
CContext< CNetwork > CNetwork::Context = CContext< CNetwork >();

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
MPI_Comm * CCommunicate::MPICommunicator(NULL);

// static 
CContext< size_t > CCommunicate::ThreadIndex = CContext< size_t >();

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
CRandom::CContext CRandom::G;

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
