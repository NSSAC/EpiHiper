# State dependent Progression and Dwell Time

## Abstract
The state progression and the dwell time are specified within the disease model. Individual properties do not influence either. This solution is working and covers the majority of cases. However it is desirable in particular in the multi disease model case to allow individual based progression.

## Implementation
For both the state progression and dwell time we will allow the replacement of the EpiHiper build in implementation by user defined methods through a plugin system as currently in use for the transmission propensity calculation. The user may chose to provide no plugins (EpiHiper build in progression and dwell time calculation), a progression plugin (attribute `progressionPlugin` in runParameters.json), a dwell time plugin (attribute `dwellTimePlugin` in runParameters.json), or both. The plugins must be provided as a dynamic link library which provides the appropriate method described below.

### Progression Plugin Interface
```
  size_t nextProgression(CNode * pNode, CModel * pModel)
```

### Dwell Time Plugin Interface
```
  unsigned int dwellTime(CNode * pNode, CProgression * pProgression, CModel * pModel)
```

### Pointer to Model
The pointer to a model is technically not required for single disease models as the global singleton may be accessed. However for the multi disease model the information must be provided. Therefore to provide a common interface it will be always provided by the caller. 
