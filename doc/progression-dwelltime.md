# State dependent Progression and Dwell Time

## Abstract
The state progression and the dwell time are specified within the disease model. Individual properties do not influence either. This solution is working and covers the majority of cases. However it is desirable in particular in the multi disease model case to allow individual based progression (HSM: to be picky, maybe better state this as 'modulated by some states and attributes of the individual'. ) The situation is similar to the transmission propensity however in that case EpiHiper already provides a plugin which allows to replace the default transmission propensity calculation.

## Implementation
The implementation for all three situations: transmission propensity, health-state progression, and progression dwell time will be treated similarly. In all three cases EpiHiper will use the default calculations unless a method is registered which overrides it. To register a plugin transmission, health-state, and progression will allow the registration on an individual (HSM: individual here means node/state yes?) level, e.g. the default propensity calculation may be changed for just a single transmission. 

(HSM: it seems the mathematical model should be specified before the implementation)

In order for a plugin to register a custom implementation it will be called after loading of the disease model has finished through a the method:
```
extern "C"
{
  void init();
}
```

A plugin will have access to either a singleton disease model `CModel::INSTANCE` (single disease case) or a singleton vector of disease models `CModel::INSTANCES` (multi-disease case). In the init method all custom implementations can be registered. Note, it is possible for a plugin to provide custom implementation for all tree situations. Furthermore it is possible to load multiple plugins. T

Plugin will be specified at runtime in the runParameters.json file. In the attribute `plugins` which is of type array containing path and name of the dynamic link library providing the plugin.

### Transmission Propensity Interface:
```
  typedef double (*transmission_propensity_type)(const CTransmission * pTransmission, const CEdge * pEdge);
```

### State Progression Interface
```
  typedef const CProgression * (*state_progression_type)(const CHealthState * pHealthState, const CNode * pNode);
```

### Progression Dwell Time Interface
```
  typedef unsigned int (*progression_dwell_time_type)(const CProgression * pProgression, const CNode * pNode);
```

