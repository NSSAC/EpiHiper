Interventions
=============

**Contents:**

* |intervention-introduction-synopsis|_
* |intervention-definition-synopsis|_
* :ref:`intervention-examples`

.. |intervention-introduction-synopsis| replace:: Interventions: changes to the state independent from the contagion model.
.. _`intervention-introduction-synopsis`: `intervention-introduction`_

.. _intervention-introduction:

Introduction
------------

.. admonition:: Synopsis

  |intervention-introduction-synopsis|

An intervention describes changes to the system state independent from the contagion model. The intervention happens when a :doc:`trigger` evaluates to true. The trigger may be defined locally in the intervention itself or externally. External triggers refer to the interventions they trigger by the the unique id. When an intervention is triggered a user defined :doc:`actions` is applied to a target :doc:`sets` (nodes :math:`N` and/or edges :math:`E`) of the :doc:`network`. Note, it is possible that the target set may be empty. In that case only action specified under ``once`` in the :doc:`actions` are applied.

.. |intervention-definition-synopsis| replace:: Definition: Syntax for the definition of interventions
.. _`intervention-definition-synopsis`: `intervention-definition`_

.. _intervention-definition:

Definition
------------

.. admonition:: Synopsis

  |intervention-definition-synopsis|

.. code-block:: text

  intervention: (id | trigger) target actionEnsemble [annotation]
  target:       setContent [annotation]

An intervention mus either have unique id so that it can be referenced by an externally defined trigger or it a locally defined trigger.

.. list-table:: Intervention definition. 
  :name: intervention-definition-spec
  :header-rows: 1

  * - | Name
    - | Type 
    - | Description
  * - | id
    - | :doc:`uniqueId </schema/uniqueId>` 
    - | An id which has to be unique within the list of interventions
  * - | trigger
    - | object
    - | The :doc:`trigger` which must be true in oder for the action ensemble
      | to be applied.
  * - | target
    - | object
    - | The :doc:`sets` (nodes :math:`N` and/or edges :math:`E`) of the :doc:`network` 
  * - | actionEnsemble
    - | object
    - | The :doc:`actions` applied to the target set .
  * - | ann:* 
    - | :doc:`annotation </schema/annotation>`
    - | Optional annotation of the intervention

.. _intervention-examples:

Examples
--------

Intervention to maintain the day of the week. This intervention is triggered at every tick. Note the variable :math:`week\_day` is reset every seven days:

.. code-block:: JSON

  "variables": [
    {
      "id": "week_day",
      "initialValue": 0,
      "scope": "local",
      "reset": 7
    }
  ],
  "interventions": [
    {
      "ann:id": "maintain_week_day",
      "ann:label": "update the week_day",
      "trigger": {
        "value": true
      },
      "target": {
        "set": {
          "idRef": "%empty%"
        }
      },
      "once": [
        {
          "operations": [
            {
              "target": {
                "variable": {
                  "idRef": "week_day"
                }
              },
              "operator": "+=",
              "value": {
                "number": 1
              }
            }
          ]
        }
      ]
    }
  ]


