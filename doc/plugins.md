# Plugins

## Abstract
The health-state progression process comprises the determination of two values __next health-state__ and __dwell time__ (delay when to enter the next health-state). Both are calculated based on the the disease model specification. Properties of the individual (vertex of the contact network) for which these values are determined do not influence the outcome. This solution is working and covers the majority of cases. The situation for the transmission process is comparable however in the case we already consider a limit set of vertex values (susceptibility and infectivity) other properties are ignored. It is desirable in particular in the multi disease model case to allow vertex based modulation of the disease transmission and progression processes.

## Implementation
The implementation for all three situations: transmission propensity, next health-state, and dwell time will be treated similarly. In all three cases EpiHiper will use the default calculations unless a method is registered which overrides it. EpiHiper will support the use of plugins, implemented as dynamic link libraries, which may override the default behavior. For the plugin to work the signature of the default methods and the plugin provided overrides (custom methods) need to have the same signatures:

### Transmission Propensity Interface:
```
  static double transmission_propensity(const CTransmission * pTransmission, const CEdge * pEdge);
```

### Next Health-State Interface
```
  static const CProgression * state_progression(const CHealthState * pHealthState, const CNode * pNode);
```

### Dwell Time Interface
```
  static unsigned int progression_dwell_time(const CProgression * pProgression, const CNode * pNode);
```

The registration of custom calculations for transmission propensity, next health-state, or dwell time will be allowed on individual instances of the appropriate classes (transmission, health-state, and progression). Note, the custom methods have access to all const methods of the provided arguments. In particular it is possible to call the default method. It is important to understand that the EpiHiper simulation process is unaware whether the default or custom methods are called, i.e., if default and custom methods return the same result the simulation outcomes will be indistinguishable. 

In order for a plugin to register overriding custom methods the plugin needs to provide an entry point for EpiHiper to call. The declaration of the entrypoint is: 
```
extern "C"
{
  void EpiHiperPluginInit();
}

```

EpiHiper will call the method EpiHiperPluginInit without any arguments after the disease model is loaded, i.e. all transmission, health-states, and progressions are available. It is the responsibility of the plugin to select the instances for custom overrides. To facilitate access EpiHiper provided the following methods:
```
  static const std::vector< CTransmission > & GetTransmissions();

  static const std::vector< CHealthState > & GetStates();

  static const std::vector< CProgression > & GetProgressions();
```

Furthermore each instance provides a setter method:
```
  void setCustomMethod(custom_type pCustomMethod) const;
```
Here `custom_type` is the above described interface matching the class of the instance.

This process is illustrated in an [example](../plugin/).

To instruct EpiHiper to load a plugin (or plugins) one has to add the attribute `"plugins"`, which is of type array and contains the full pathes to the dynamic link libraries to be loaded, in the configuration file given to EpiHiper, e.g. insert:
```
  "plugins": [
    "/home/shoops/workspace/EpiHiper/build/plugin/libEpiHiperPlugin.so"
  ]
```
