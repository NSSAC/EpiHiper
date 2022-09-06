# Multi Disease Support

## Abstract
It is often required to simulate the epidemics of multiple disease simultaneously, for example flu and COVID-19 or to simulate several strains. 
These diseases will often interact, i.e., influencing each other. An argument can be made that no enhancements are needed as it possible to explicitly 
define all state combinations. However our currently used [flu model](https://github.com/NSSAC/smh-flu-usa-epihiper/blob/main/model/stomp.json) has 100 disease states. Thus simulating just 2 strains would require 10,000 states.  

## Multiple Independent Diseases
The support for multiple independent disease models is straight forward. Instead of loading just one model a list of disease models may be loaded therefore replacing the single model with a vector of models suffices to handle the models.

Each individual (node) currently has a single health state as well as unique infectivity, susceptibility, infectivity factor, and susceptibility factor. Replacing each of those with vectors would allow to record each models disease state and the according infectivity and susceptibility.

The transmission process will have to be adapted to support multi diseases. This again is straight forward by looping over all loaded disease models and computing their transmission processes independently. Note, it is possible through custom propensity functions to utilize all state information from both involved individuals thus having disease interdependence.

To support some commonality between diseases like susceptibility influenced by behaviors such as mask wearing the addition of common infectivity, common susceptibility, common infectivity factor, and common susceptibility factor would be beneficiary. Furthermore the current single transmissibility should be similarly replaced by a vector of transmissibilities and a common transmissibility.

In the implementation of initialization and interventions the currently used health state would have to be replaced by a tuple: disease-model, health-state since states are only unique within a model.

## Interaction between Diseases
The above described implementation of multiple independent diseases would already allow disease interaction through the use of interventions. However this naive approach would be computational costly as it would require us to select all nodes entering a specific health state and then scheduling the modifications of those. This selection process can be avoided by scheduling the modifications the health state is change schedule. This is analogues to the handling  of disease progression within a disease model. To reuse most of the capabilities of the existing single disease implementation the modifications will be given by an action ensemble. Sampling in these action ensemble must be restricted to individual as we are always dealing with just on node. The disease interactions will be encoded in JSON format and can be stored in multiple files.