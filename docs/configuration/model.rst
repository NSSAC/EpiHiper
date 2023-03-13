Contagion Model
===============

**Contents:**

* |model-introduction-synopsis|_
* |model-states-synopsis|_
* |model-progressions-synopsis|_
* |model-transmissions-synopsis|_

.. |model-introduction-synopsis| replace:: Contagion Model: a fully programmable model comprised from a set :math:`\mathcal{X} = \{X_1, X_2, \ldots, X_m\}` of **states**.
.. _`model-introduction-synopsis`: `model-introduction`_

Introduction
------------

.. admonition:: Synopsis

   |model-introduction-synopsis|

.. _model-introduction:

The contagion model is fully programmable, and starts from a set :math:`\mathcal{X} = \{X_1, X_2, \ldots, X_m\}` of **states**. The **progression**, which captures the evolution of states whithin a person, is represented using a probabilistic timed transition system (PTTS) over :math:`\mathcal{X}`. These are an extension of finite state machines with the additional features that state progressions are probabilistic and timed. 

A PTTS is a set of states, where each state has an :math:`id`, a common set of attributes values, and one or more labeled sets of weighted progressions with dwell time distributions to other states. The label on the progression sets is used to select the appropriate set of progressions, factoring in for example pharmaceutical treatments that have been applied to an individual. The attributes of a state describe the levels of infectivity, susceptibility, and symptoms an individual who is in that state possess. Once an individual enters a state, the amount of time that they will remain in that state is drawn from the dwell time distribution. Using the notion of **contact configurations**, one may succinctly specify the contact between individuals, factoring in the disease states that can cause a **transmission**.

It includes distributions for dwell times as well as weights for progressions out of states for which there are multiple health state outcomes. 

As an illustration we consider a hypothetical case of a classic influenza (or COVID-like) outbreak in Albemarle County, Virginia. 

Here we use the set

.. math::

  \begin{equation*}
    \mathcal{X} = \{S, E, Isymp, Iasymp, R\} %\;,
  \end{equation*}

to encode the five {health states}  susceptible :math:`S`, exposed :math:`E`, infectious and symptomatic :math:`Isymp`, infectious and asymptomatic :math:`Iasymp`, and recovered :math:`R` with the combined transmission and progression diagram as follows:

.. _exdiagram:

.. math::

  \begin{align*}
                    & \phantom{\underset{p = 0.33}{\searrow}} \; Isymp \\
                    & \overset{p = 0.67}{\nearrow} \phantom{Iasymp} \searrow \\
    S \Rightarrow E & \phantom{\underset{p = 0.33}{\searrow} Iasymp \nearrow} \; R \\
                    & \underset{p = 0.33}{\searrow} \phantom{Iasymp} \nearrow \\
                    & \phantom{\underset{p = 0.33}{\searrow}} \; Iasymp
  \end{align*}

Apart from the double arrow/edge :math:`S \Rightarrow E` which specifies transmission, each edge corresponds to a progression in the disease  progression model. We note that disease  progression from health state :math:`E` to :math:`Isymp` is twice as likely as  progression from :math:`E` to :math:`Iasymp` (or :math:`0.67/0.33` to be precise). The dwell time distribution for both progressions out of the state `E`, which we denote by edges :math:`(E, Isymp)` and :math:`(E, Iasymp)`, are

.. math::

  \begin{equation*}
    \{0: 0.1; 1: 0.2; 2: 0.6; 3: 0.1\} \;,
  \end{equation*}

meaning that, e.g., the probability of a dwell time of duration 2 (days) is :math:`0.6`. Similarly, the dwell time distributions for the progressions :math:`(Isymp, R)` and :math:`(Iasymp, R)` out of states :math:`Isymp` and :math:`Iasymp` are both

.. math::

  \begin{equation*}
    \{3: 0.3; 4: 0.4; 5: 0.2; 6: 0.1\} \;.
  \end{equation*}

Note that a dwell time distribution is associated with an edge (health state progression) and that the unit of time is one iteration, which in this example equals one day. 

To describe the **transmission process**, we refer to :numref:`epihiper-toy-network`, which

.. figure:: /_images/epihiper-toy-network.png
   :alt: epihiper-toy-network
   :name: epihiper-toy-network
   :align: center

   An example network for EpiHiper with individuals :math:`P`, :math:`P'` and :math:`P''`. Here, the infectious person `P'` may infect the susceptible person :math:`P`, who as a result may progression from health state :math:`X` to an exposed state :math:`X'`.


shows a network where a susceptible person :math:`P` is in contact with infectious persons :math:`P'` and :math:`P''`. Focusing on the the pair :math:`(P',P)`, we combine the {state susceptibility} and {state infectivity} of their respective health states :math:`X_k` and :math:`X_i` with the {infectivity scaling factor} of :math:`P'` and the {susceptibility scaling factor} of :math:`P` to form the propensity associated with the contact configuration :math:`T_{i,j,k} = T(X_i, X_j, X_k)` for the potential progression of the health state of person :math:`P` to :math:`X_j` as:

.. math::
  :label: propensity-main

  \begin{equation*} 
    \rho(P, P', T_{i,j,k},e) = \bigl[T \cdot \tau\bigr] \times w_e \times \alpha_e \times \bigl[\beta_s(P) \cdot \sigma(X_i)\bigr] \times \bigl[\beta_\iota(P') \cdot \iota(X_k) \bigr] \times \omega(T_{i,j,k})
  \end{equation*}

Here, :math:`T` is the duration of contact for the edge :math:`e = (P', P, w, \alpha, T)`, :math:`w` is an edge weight, and :math:`\alpha` is a Boolean value indicating whether or not the edge is active (e.g., not disabled because of an ongoing school closure). 

In the example, there are two transmission configurations (1 susceptible state `\times` 2 infectious states) are

.. math::

  \begin{equation*}
    T_s = T(S,E,Isymp) 
    \text{ and } 
    T_a = T(S,E,Iasymp) \;,
  \end{equation*}

both having the default weight of :math:`\omega = 1.0`.

Regarding infectivity, people in either of the states :math:`Isymp` and :math:`Iasymp` can transmit infections, with the asymptomatic reduction in infectivity being 60\% and thus :math:`\beta_\iota(Iasymp) = 0.40` while the susceptibility and infectivity values of all other health states have the default value of :math:`1.0`. In addition, the user may define a collection of **traits** to associate with each edge or node, see Section :doc:`traits` for full details. 

For each time step, and for each person :math:`P`, the propensities :math:`\rho` from :eq:`propensity-main` are collected across all edges :math:`e` and contact configurations `T` as the sequence :math:`\rho_P = (\rho(P, P', T, e)_{P', T, e})`. To determine if :math:`P` becomes infected is modeled using a Gillespie process :cite:p:`Gillespie:76,Gillespie:77` the person :math:`P'` to whom one attributes :math:`P` becoming infected is also determined as part of this step. 

To determine if an infection takes place, and also to whom we attribute the infection (e.g., :math:`P'` or :math:`P''` in  :numref:`epihiper-toy-network`, we use the Direct Gillespie Method. 

**Contagion model assumptions**. It is assumed that (i) propensities for a person are independent across contact configurations, and (ii) that during any time step no person can change their health state. The first assumption is quite common and not unreasonable for the contact networks that are used. The second assumption can always be accommodated by reducing the size of the time step. Its real purpose is to ensure **order invariance** of contacts within a time step, thus providing the required guarantee for algorithm correctness.

.. list-table:: EpiHiper core model parameters
  :name: core-model-parameters
  :header-rows: 1

  * - | Parameter
    - | Description
  * - | :math:`P`, :math:`P'`
    - | Persons/agents/nodes
  * - | :math:`X_i`
    - | Health state :math:`i`
  * - | :math:`\sigma(X_i)`
    - |  Susceptibility of health state :math:`X_i`
  * - | :math:`\iota(X_i)`
    - | Infectivity of health state :math:`X_i`
  * - | :math:`\beta_\sigma(P)`
    - | Susceptibility scaling factor for person :math:`P`
  * - | :math:`\beta_\iota(P)`
    - | Infectivity scaling factor for person :math:`P`
  * - | :math:`w_e`
    - | Weight of edge :math:`e = (P, P')`
  * - | :math:`\alpha_e`
    - | Flag indicating whether the edge :math:`e` is active
  * - | :math:`T(X_i,X_j,X_k)`
    - | Contact configuration for a susceptible progression from :math:`X_i` to :math:`X_j`
      | in the presence of state :math:`X_k`
  * - | :math:`\omega_{i,j,k}`
    - | Transmission weight of contact configuration :math:`T(X_i, X_j, X_k)`
  * - | :math:`\tau`
    - | Transmissibility
  * - | :math:`\rho(P, P', T_{i,j,k},e)`
    - | Contact propensity

.. |model-states-synopsis| replace:: States: a declaration of states of the contagion model
.. _`model-states-synopsis`: `model-states`_

.. _model-states:

States
------

.. admonition:: Synopsis

   |model-states-synopsis|

.. _model-states-specification:

Specification
^^^^^^^^^^^^^

To define the states of the contagion model, the following syntax is used:

.. code-block:: bash

  states:       list(state)
  state:        id susceptibility infectivity [annotation]
  initialState: idRef

.. list-table:: List of state attributes
  :name: model-states-attributes
  :header-rows: 1
  
  * - | Name
    - | Type 
    - | Description
  * - | id
    - | `unique id <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L6>`_ 
    - | An id which has to be unique within the list of states
  * - | susceptibility
    - | :math:`0 \le x` 
    - | The susceptibility of the state
  * - | infectivity
    - | :math:`0 \le x` 
    - | The infectivity of the state
  * - | ann:* 
    - | `annotation <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L96>`_
    - | Optional annotation of the state

The ``idRef`` property of the ``initalState`` must refer to an existing id in the list of the states. The normative JSON schema can be found at:  `states <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/diseaseModelSchema.json#L19>`_

.. _model-states-examples:

Examples
^^^^^^^^

.. code-block:: JSON

  "states": [
    {
      "id": "susceptible",
      "ann:label": "Susceptible",
      "susceptibility": 1.0,
      "infectivity": 0
    },
    {
      "id": "exposed",
      "ann:label": "Exposed",
      "susceptibility": 0,
      "infectivity": 0
    },
    {
      "id": "infectious",
      "ann:label": "Infectious",
      "susceptibility": 0,
      "infectivity": 0.1
    },
    {
      "id": "hospitalized",
      "ann:label": "Hospitalized",
      "susceptibility": 0,
      "infectivity": 0.2
    },
    {
      "id": "funeral",
      "ann:label": "Funeral",
      "susceptibility": 0,
      "infectivity": 0.2
    },
    {
      "id": "removed",
      "ann:label": "Removed",
      "susceptibility": 0,
      "infectivity": 0
    }
  ],
  "initialState": "susceptible",

.. |model-progressions-synopsis| replace:: Progressions: the description of all progressions from entry states to exit state, including dwell-time distributions and probabilities.
.. _`model-progressions-synopsis`: `model-progressions`_

.. _model-progressions:

Progressions
------------

.. admonition:: Synopsis

   |model-progressions-synopsis|

.. _model-progressions-specification:

Specification
^^^^^^^^^^^^^

To define the progressions between states of the contagion model, the following syntax is used:

.. code-block:: bash

  transitions:  list(transition)
  transition:   id entryState exitState probability dwellTime 
                [susceptibilityFactorOperation] [infectivityFactorOperation] [annotation]

.. list-table:: List of state attributes
  :name: model-progressions-attributes
  :header-rows: 1

  * - | Name
    - | Type 
    - | Description
  * - | id
    - | `unique id <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L6>`_ 
    - | An id which has to be unique within the list 
      | of transitions
  * - | entryState
    - | idRef 
    - | The entryState must refer to an existing id in the list 
      | of states.
  * - | exitState
    - | idRef 
    - | The exitState must refer to an existing id in the list 
      | of states.
  * - | probability
    - | :math:`0 \le x \le 1` 
    - | The probability that the entry state changes to the 
      | exit state 
  * - | dwellTime
    - | `distribution <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L133>`_ 
    - | The time before the state change occurs
  * - | susceptibilityFactorOperation
    - | `operation <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L111>`_ 
    - | The numeric operation to be performed on an  
      | individuals susceptibility factor when the 
      | state change occurs
  * - | infectivityFactorOperation
    - | `operation <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L111>`_ 
    - | The numeric operation to be performed on an  
      | individuals infectivity factor when the 
      | state change occurs
  * - | ann:* 
    - | `annotation <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L96>`_
    - | Optional annotation of the state

If the optional ``susceptibilityFactorOperation`` or ``infectivityFactorOperation`` are missing no operation will be exectuted, i.e., the current factor will be preserved. The normative JSON schema can be found at:  `transitions <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/diseaseModelSchema.json#L80>`_

.. _model-progressions-examples:

Examples
^^^^^^^^

.. code-block:: JSON

  "transitions": [
    {
      "id": "exposed2infectious",
      "ann:label": "Exposed->Infectious",
      "entryState": "exposed",
      "exitState": "infectious",
      "probability": 1,
      "dwellTime": {"fixed": 3}
    },
    {
      "id": "infectious2hospitalized",
      "ann:label": "Infectious->Hospitalized",
      "entryState": "infectious",
      "exitState": "hospitalized",
      "probability": 0.1111,
      "dwellTime": {"fixed": 2}
    },
    {
      "id": "infectious2funeral",
      "ann:label": "Infectious->Funeral",
      "entryState": "infectious",
      "exitState": "funeral",
      "probability": 0.6667,
      "dwellTime": {"fixed": 10}
    },
    {
      "id": "infectious2removed",
      "ann:label": "Infectious->Removed",
      "entryState": "infectious",
      "exitState": "removed",
      "probability": 0.2222,
      "dwellTime": {"fixed": 12}
    },
    {
      "id": "hospitalized2funeral",
      "ann:label": "Hospitalized->Funeral",
      "entryState": "hospitalized",
      "exitState": "funeral",
      "probability": 0.1111,
      "dwellTime": {"fixed": 15}
    },
    {
      "id": "hospitalized2removed",
      "ann:label": "Hospitalized->Removed",
      "entryState": "hospitalized",
      "exitState": "removed",
      "probability": 0.8889,
      "dwellTime": {"fixed": 15}
    },
    {
      "id": "funeral2removed",
      "ann:slabel": "Funeral->Removed",
      "entryState": "funeral",
      "exitState": "removed",
      "probability": 1,
      "dwellTime": {"fixed": 1}
    }
  ]

.. |model-transmissions-synopsis| replace:: Transmissions: the description of the contact configurations :math:`T(X_i, X_j, X_k)`  for individuals causing a state change.

.. _`model-transmissions-synopsis`: `model-transmissions`_

.. _model-transmissions:

Transmissions
-------------

.. admonition:: Synopsis

   |model-transmissions-synopsis|

.. _model-transmissions-specification:

Specification
^^^^^^^^^^^^^

To define possible transmission between occurring between individuals in the contagion model, the following syntax is used:

.. code-block:: bash

  transmissions:    list(transmission) [transmissibility]
  transmission:     id entryState exitState probability transmissibility 
                    [susceptibilityFactorOperation] [infectivityFactorOperation] [annotation]
  transmissibility: x >= 0

.. list-table:: List of state attributes
  :name: model-transmissions-attributes
  :header-rows: 1

  * - | Name
    - | Type 
    - | Description
  * - | id
    - | `unique id <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L6>`_ 
    - | An id which has to be unique within the list 
      | of transmissions
  * - | entryState
    - | idRef 
    - | The entryState must refer to an existing id in the list 
      | of states.
  * - | exitState
    - | idRef 
    - | The exitState must refer to an existing id in the list 
      | of states.
  * - | contactState
    - | idRef 
    - | The contactState must refer to an existing id in the list 
      | of states.
  * - | transmissibility
    - | :math:`0 \le x` 
    - | The transmissibility of the for each contact. 
  * - | susceptibilityFactorOperation
    - | `operation <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L111>`_ 
    - | The numeric operation to be performed on an  
      | individuals susceptibility factor when the 
      | state change occurs
  * - | infectivityFactorOperation
    - | `operation <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L111>`_ 
    - | The numeric operation to be performed on an  
      | individuals infectivity factor when the 
      | state change occurs
  * - | ann:* 
    - | `annotation <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L96>`_
    - | Optional annotation of the state

If the optional ``susceptibilityFactorOperation`` or ``infectivityFactorOperation`` are missing no operation will be executed, i.e., the current factor will be preserved. The normative JSON schema can be found at:  `transmissions <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/diseaseModelSchema.json#L47>`_. The optional model attribute `transmissibility` is used to scale all individual transmissibilities. Its default value is :math:`1.0`

Examples
^^^^^^^^

.. code-block:: JSON

  "transmissions": [
    {
      "id": "contactWithInfectious",
      "ann:label": "Susceptible -> Exposed {Infectious}",
      "entryState": "susceptible",
      "exitState": "exposed",
      "contactState": "infectious",
      "transmissibility": 1
    },
    {
      "id": "contactWithHospitalized",
      "ann:label": "Susceptible -> Exposed {Hospitalized}",
      "entryState": "susceptible",
      "exitState": "exposed",
      "contactState": "hospitalized",
      "transmissibility": 1
    },
    {
      "id": "contactWithFuneral",
      "ann:label": "Susceptible -> Exposed {Funeral}",
      "entryState": "susceptible",
      "exitState": "exposed",
      "contactState": "funeral",
      "transmissibility": 1
    }
  ],
  "transmissibility": 0.8

Bibliography
------------

.. bibliography:: 