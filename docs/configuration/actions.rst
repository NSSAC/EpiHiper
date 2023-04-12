Action Ensemble
===============

**Contents:**

* |actions-introduction-synopsis|_
* |actions-definition-synopsis|_

* :ref:`actions-examples`

.. |actions-introduction-synopsis| replace:: Action ensemble: a description of discrete state changes to the system during :doc:`initialization` and :doc:`intervention` 

.. _`actions-introduction-synopsis`: `actions-introduction`_

.. _actions-introduction:

Introduction
------------

.. admonition:: Synopsis

  |actions-introduction-synopsis|

An action ensemble describes changes to the systems state performed during :doc:`initialization` and :doc:`intervention`. These changes may modify node or edge attributes as well as user defined :doc:`variables`. An action ensemble is applied to a target provided as context by an initialization or an intervention. Actions are scheduled with an optional delay to ensure proper random order of execution of simultaneous actions with equal priority.

.. |actions-definition-synopsis| replace:: Definition: syntax for the definition of an action ensemble
.. _`actions-definition-synopsis`: `actions-definition`_

.. _actions-definition:

Definition
----------

.. admonition:: Synopsis

  |actions-definition-synopsis|

.. code-block:: text
  
  :            [once] [foreach] [sampling] [annotation]
  once:        list(action)
  foreach:     list(action)
  action:      operations [priority] [delay] [condition] [annotation]
  operations:  list(operation)
  operation:   target operator (value | observable | variable | sizeof)
  target:      (type 
                 (duration | weight | active | edgeTrait)
                 | (susceptibilityFactor | infectivityFactor | healthState | nodeTrait))
               | (variable)
  type:        node | edge
  operator:    = | *= | /= | += | -=
  priority:    x (default: 1)
  delay:       0 <= n (default: 0)
  condition:   Boolean
  sampling:    type (probability | percent | count) 
               [sampled] [nonsampled] [annotation]
  type:        individual | group | absolute
  probability: number | variable
  percent:     number | variable
  count:       number | variable
  sampled:     [foreach] [sampling] [annotation]
  nonsampled:  [foreach] [sampling] [annotation]

.. list-table:: Action ensemble definition. 
  :name: actions-definition-spec
  :header-rows: 1

  * - | Name
    - | Type 
    - | Description
  * - | once
    - | list(action)
    - | Actions which are executed once independent from 
      | the target set.
  * - | foreach
    - | list(action)
    - | Actions which are executed for each element in the target set.
  * - | action
    - | object
    - | An action to be performed.
  * - | target
    - | object
    - | Writable properties of `edges <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L680>`_  or `nodes <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L637>`_ or a variable.
  * - | operator
    - | string
    - | The operator used for calculating the assigned value:
      | :math:`\qquad` = | \*= | /= | += | -=
      | The operations argument is given by a value, an observable, 
      | a variable, or the function sizeof.
  * - | priority
    - | :math:`x \in \mathbb{R}`
    - | Order of action execution: 
      | lowest priority first, random order if equal priority
  * - | delay
    - | :math:`n \in \mathbb{N}_0`
    - | Optional delay of execution of the action in ticks.
  * - | condition
    - | object
    - | Boolean expression which must evaluate to true at the 
      | time of execution.
  * - | sampling
    - | object
    - | Optional sampling of the elements in the target set in context.
  * - | type 
    - | string
    - | The type of sampling (individual | group | absolute).
  * - | probability
    - | :math:`0 \le x \in \mathbb{R} \le 1`
    - | The probability for individual or group sampling 
      | given by a number or variable.
  * - | percent
    - | :math:`0 \le x \in \mathbb{R} \le 100`
    - | The percent for individual or group sampling 
      | given by a number or variable.
  * - | count
    - | :math:`n \in \mathbb{N}_0`
    - | The count of element for absolute sampling 
      | given by a number or variable.
  * - | sampled
    - | object
    - | Action ensemble for the sampled subset of the target set.
  * - | nonsampled
    - | object
    - | Action ensemble for the non-sampled subset of the target set.
  * - | ann:* 
    - | `annotation <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L96>`_
    - | Optional annotation of the set.

.. _actions-examples:

Examples
--------

80% of individuals with a positive antigen test start isolation immediately (delay = 0). The remaining 20% start with 1 day delay.

.. code-block:: JSON

      "sampling": {
        "type": "individual",
        "percent": {
          "number": 80
        },
        "sampled": {
          "foreach": [
            {
              "delay": 0,
              "operations": [
                {
                  "target": {
                    "node": {
                      "property": "nodeTrait",
                      "feature": "to_start_isolation"
                    }
                  },
                  "operator": "=",
                  "value": {
                    "trait": "nodeTrait",
                    "feature": "to_start_isolation",
                    "enum": "true"
                  }
                }
              ]
            },
            {
              "delay": 1,
              "operations": [
                {
                  "target": {
                    "node": {
                      "property": "nodeTrait",
                      "feature": "to_start_isolation"
                    }
                  },
                  "operator": "=",
                  "value": {
                    "trait": "nodeTrait",
                    "feature": "to_start_isolation",
                    "enum": "false"
                  }
                }
              ]
            }
          ]
        }
      }
