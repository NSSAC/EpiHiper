# Multi Disease Support

## Abstract
It is often required to simulate the epidemics of multiple disease simultaneously, for example flu and COVID-19, or to simulate several strains. These diseases will often interact where for example health state transitions (timing and propensities) of each disease may be impacted by variables and states associated to other diseases. An argument can be made that no enhancements are needed for EpiHiper as it possible to explicitly define all state combinations and updated transitions for the multi-disease setting. However, our current [flu model](https://github.com/NSSAC/smh-flu-usa-epihiper/blob/main/model/stomp.json) has 100 disease states. Thus simulating just two strains, as an example, would require 10,000 states.  

## Multiple Dependent Diseases
Supporting multiple, independent disease models is straightforward. Instead of loading just one model, a list of disease models may be loaded, effectively vectorizing this aspect. 

Each individual (node) currently has a single health state and unique infectivity, susceptibility, infectivity scaling, and susceptibility scaling factors. Replacing each of those with vectors would allow to record each models disease state and the according infectivity and susceptibility.

To fully support interacting diseases, the transmission process will have to be adapted. Conceptually, this is straightforward: one would loop over all loaded disease models and compute each transmission processes independently. An update to the design of the EpiHiper propensity function would support integration of all relevant state information from the infectious and the susceptible individuals. The addition of this coupling function factor to the propensity would provide a succinct and general approach for integrating disease interdependence. The approach would also extend directly to, for example, integrate environmental factors if/when relevant.

(HSM: A paragraph on how the disease progression, or at least it specification, would have to be updated for the multi-disease context. The transmissions and dwell times would factor through a function similar to the coupling function introduced in the propensity for disease transmission.)

To support some commonality between diseases like susceptibility influenced by behaviors such as mask wearing the addition of common infectivity, common susceptibility, common infectivity factor, and common susceptibility factor would be beneficiary. (HSM: I do not fully understand the previous sentence.) Furthermore the current single, global transmissibility factor should be replaced by (1) a vector of transmissibilities and (2) a common (scalar) transmissibility. This would allow one to separately scale transmission across diseases *and* scale the overall evolution of the ensemble of disease models. 

The current initialization and intervention semantic is sufficient except when vectorized (per disease) elements (health-state etc.) need to be accessed. In those cases the addition of an index (the id of the corresponding disease model) will suffice. This addition will allow us to access all existing and vectorized attributes in set construction, as targets for modification, and in conditions.

Any health-state transition scheduled by the disease progression or transmission will be only executed if the health-state at execution time is unchanged from the scheduling time. If another disease or intervention modifies the state beforehand the scheduled action will be ignored.
This is analogues to the single disease model. 

Benefits of the new design are the ability to use existing calibrated and proven models to create simulations of multiple epidemics without any additional work. For example we can easily study the effect of COVID-19 based interventions like home isolation on a simultaneous flu epidemic by just adding an existing calibrated flu model. Furthermore multiple strains implemented by copies of the same disease model with different transmissibilities can exist independently. The only thing which needs to be specified is the interaction between strains, e.g. partial protection from the other strains after recovery. The description of these which will be handled in the next section. These is a lot easier than to define a new language and corresponding tools which create a fully expanded disease models with possibly thousands of states. 

(HSM: here I would think a paragraph discussing the benefits of the new design: (a) taxonomy of specification, (b) taxonomy of number of states in multi-disease representation)


## Interaction between Diseases
The above described implementation of multiple independent diseases would already allow disease interaction through the use of interventions. However this naive approach would be computational costly as it would require us to select all nodes entering a specific health state and then scheduling the modifications of those. This selection process can be avoided by scheduling the modifications the moment health state change is executed. This is analogues to the handling  of disease progression within a disease model. To reuse most of the capabilities of the existing single disease implementation the modifications will be given by an action ensemble. Sampling in these action ensemble must be restricted to individual as we are always dealing with just on node. The disease interactions will be encoded in JSON format and can be stored in multiple files.

(HSM: a paragraph on actions. In particular, dependencies between scheduled actions: transition for disease A from state x1 to x2 scheduled at time T; for disease B, a later action may be executed before T, potentially invalidating the disease A action already scheduled. Question 1: how does one preserve the ability to not keep track of history; Question 2: will one have to accept some degree of approximation?)
(SH: I moved this ti interaction between diseases)

(HSM: I am a little unclear on the previous paragraph. I would have expected paragraphs on (a) interventions and what those would look like - don't expect much to change here vis-a-vis what is already there; I expect most changes to happen in the actions; possibly some changes to set construction and predicates; (b) some paragraph about specifying intervention and their representations (JSON); (c) some paragraph talking about disease model reuse. The coupling and their specifications will be complicated and limit reuse. However, I do not see a way around this)

(SH: These changes are minimal an described in the previous section)

