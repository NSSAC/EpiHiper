Initialization
==============

.. include:: _schema_links.rst

**Contents:**

* |initialization-introduction-synopsis|_
* |initialization-definition-synopsis|_
* :ref:`initialization-examples`

.. |initialization-introduction-synopsis| replace:: Initialization: Changes to the systems state prior to any simulation.
.. _`initialization-introduction-synopsis`: `initialization-introduction`_

.. _initialization-introduction:

Introduction
------------

.. admonition:: Synopsis

  |initialization-introduction-synopsis|

In oder to specify the state of the system (nodes :math:`N` and/or edges :math:`E` attributes) EpiHiper uses user defined initialization. An initialization is a user defined :doc:`actions` applied to a target :doc:`sets` (nodes :math:`N` and/or edges :math:`E`) of the :doc:`network`. Note, it is possible that the target set may be empty. In that case only action specified under ``once`` in the :doc:`actions` are applied.

.. |initialization-definition-synopsis| replace:: Definition: Syntax for the definition of initialization
.. _`initialization-definition-synopsis`: `initialization-definition`_


.. _initialization-definition:

Definition
------------

.. admonition:: Synopsis

  |initialization-definition-synopsis|

.. code-block:: text

  intervention: target actionEnsemble [annotation]
  target:       setContent [annotation]

.. list-table:: Initialization definition. 
  :name: initialization-definition-spec
  :header-rows: 1

  * - | Name
    - | Type 
    - | Description
  * - | target
    - | object
    - | The :doc:`sets` (nodes :math:`N` and/or edges :math:`E`) of the :doc:`network` 
  * - | actionEnsemble
    - | object
    - | The :doc:`actions` applied to the target set .
  * - | ann:* 
    - | :doc:`annotation </schema/annotation>`
    - | Optional annotation of the initialization
    
.. _initialization-examples:

Examples
--------

The nodes with ids 3 & 5 are put in the health state :math:`I`, which is infectious, prior to the simulation.

.. code-block:: JSON

  "sets": [
    {
      "id": "infections",
      "scope": "local",
      "content": {
        "elementType": "node",
        "scope": "local",
        "left": {
          "node": {
            "property": "id"
          }
        },
        "operator": "in",
        "right": {
          "valueList": {
            "id": [
              3, 5
            ]
          }
        }
      }
    }
  ],
  "initializations": [
    {
      "ann:id": "initial_infections",
      "target": {
        "set": {
          "idRef": "infections"
        }
      },
      "foreach": [
        {
          "operations": [
            {
              "target": {
                "node": {
                  "property": "healthState"
                }
              },
              "operator": "=",
              "value": {
                "healthState": "I"
              }
            }
          ]
        }
      ]
    }
  ]

