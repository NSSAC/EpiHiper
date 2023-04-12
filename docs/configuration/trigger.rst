Trigger
=======

**Contents:**

* |trigger-introduction-synopsis|_
* |trigger-definition-synopsis|_
* |trigger-boolean-synopsis|_
* :ref:`trigger-examples`

.. |trigger-introduction-synopsis| replace:: Trigger: a Boolean expression which causes interventions.
.. _`trigger-introduction-synopsis`: `trigger-introduction`_

.. _trigger-introduction:

Introduction
------------

.. admonition:: Synopsis

  |trigger-introduction-synopsis|

In EpiHiper trigger are a core building block of :doc:`intervention`. They are used to determine the time point when an intervention is performed.

.. |trigger-definition-synopsis| replace:: Definition: Syntax for the definition of trigger
.. _`trigger-definition-synopsis`: `trigger-definition`_

.. _trigger-definition:

Definition
----------

.. admonition:: Synopsis

  |trigger-definition-synopsis|

.. code-block:: text
  
  :                trigger interventionIds [annotation]
  trigger:         boolean
  interventionIds: list(id)
  
.. list-table:: Trigger definition. 
  :name: trigger-definition-spec
  :header-rows: 1

  * - | Name
    - | Type 
    - | Description
  * - | trigger
    - | object
    - | A :ref:`trigger-boolean`
  * - | interventionIds
    - | list(id)
    - | A list of ids referencing :doc:`intervention` to be performed.
  * - | ann:* 
    - | `annotation <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L96>`_
    - | Optional annotation of the trigger.


.. |trigger-boolean-synopsis| replace:: Boolean expression: specification.

.. _`trigger-boolean-synopsis`: `trigger-boolean`_

.. _trigger-boolean:

Boolean Expression
------------------

.. admonition:: Synopsis

  |trigger-boolean-synopsis|

EpiHiper allows for the definition of complex Boolean expressions utilizing the current state of the simulation.

.. code-block:: text
  
  :              (not)
               | (and | or)
               | (value)
               | (operator left right)
  not:         Boolean Expression
  and:         list(Boolean Expression)
  or:          list(Boolean Expression)
  value:       true | false
  operator:      (== | != | <= | < | >= | >)
               | (in | not in)
  left:        (value | variable | observable | node | edge | sizeof)
  right:         (value | variable | observable | node | edge | sizeof)
               | valueList
  value:       (boolean | number | id | healthState | trait feature enum)
  variable:    idRef
  node:        property
  property:    id | susceptibilityFactor | infectivityFactor | healthState | nodeTrait feature
  edge:        property
  property:      targetId | sourceId | locationId | duration | weight | active 
               | (targetActivity | sourceActivity | edgeTrait) feature
  sizeof:      set content
  valueList:   (boolean | number | id | healthState | trait feature enum)

.. list-table:: Trigger boolean definition. 
  :name: trigger-boolean-spec
  :header-rows: 1

  * - | Name
    - | Type 
    - | Description
  * - | not
    - | object 
    - | Negate a :ref:`trigger-boolean`.
  * - | and | or
    - | list 
    - | An operation (and|or) applied to a list of :ref:`trigger-boolean`.
  * - | value
    - | boolean 
    - | A boolean value: true | false
  * - | operator
    - | string
    - | The operator used for comparing left and right:
      | == | != | <= | < | >= | > | in | not in
  * - | left 
    - | object
    - | The left operant of the comparison
  * - | right 
    - | object
    - | The right operant of the comparison. Depending on the operator
      | this must be either a value or a valueList.
  * - | value
    - | object
    - | A `value <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L277>`_
  * - | valueList
    - | object
    - | A `list of values <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L315>`_ 
  * - | node 
    - | object
    - | A `node property <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L637>`_
  * - | edge 
    - | object
    - | An `edge property <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L680>`_
  * - | sizeof
    - | object
    - | Function to determine the size of the given :ref:`set content <sets-content>`.

The normative JSON schema can be found at:  `boolean <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L836>`_ 

.. _trigger-examples:

Examples
--------

Schedule an antigen test on the first and third weekday starting at day 30 day of the simulation.

.. code-block:: JSON

  "triggers": [
    {
      "ann:id": "t_antigen_test",
      "trigger": {
        "and": [
          {
            "left": {
              "observable": "time"
            },
            "operator": ">=",
            "right": {
              "value": {
                "number": 30
              }
            }
          },
          {
            "left": {
              "variable": {
                "idRef": "week_day"
              }
            },
            "operator": ">=",
            "right": {
              "value": {
                "number": 1
              }
            }
          },
          {
            "left": {
              "variable": {
                "idRef": "week_day"
              }
            },
            "operator": "<=",
            "right": {
              "value": {
                "number": 3
              }
            }
          }
        ]
      },
      "interventionIds": [
        "antigen_test"
      ]
    }
  ]