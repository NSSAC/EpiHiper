Observables
===========

.. include:: _schema_links.rst

**Contents:**

* |observables-introduction-synopsis|_
* |observables-specification-synopsis|_
* :ref:`observables-examples`

.. |observables-introduction-synopsis| replace:: Observables: numerical information about the systems state,
.. _`observables-introduction-synopsis`: `observables-introduction`_

.. _observables-introduction:

Introduction
------------

.. admonition:: Synopsis

   |observables-introduction-synopsis|

EpiHiper provides information information of the system state to the user to the user through observables. Observables comprise `time`, `total population`, and absolute and relative values about the number of nodes in each individual of the :doc:`model`. Furthermore the changes in and out of such a state are provided. Observables may be used in a :doc:`trigger` condition, and as  as the right hand side of an operation (see :doc:`actions`). Observables are read read-only.

.. |observables-specification-synopsis| replace:: Specification: how to access EpiHiper provided observables. 
.. _`observables-specification-synopsis`: `observables-specification`_

.. _observables-specification:

Specification
-------------

.. admonition:: Synopsis

   |observables-specification-synopsis|

.. code-block:: text

  observable:   time | totalPopulation | (healthState type [subset])
  healthState:  id
  type:         absolute | relative
  subset:       in | out | current

.. list-table:: Variable definition. 
  :name: observables-definition-spec
  :header-rows: 1

  * - | Name
    - | Type 
    - | Description
  * - | healthState
    - | string
    - | An id of a state of the :doc:`model`.
  * - | type
    - | absolute|relative
    - | Specifies whether the total or relative (to total population) 
      | value is requested
  * - | subset
    - | in|out|current
    - | select whether the number of nodes entering (in), existing (out), 
      | or are currently in the state.


The normative JSON schema can be found at:  |schema_observables|_ 

.. _observables-examples:

Examples
--------

Trigger for seeding (introducing infected nodes) prior to tick 10.

.. code-block:: JSON

  "triggers": [
    {
      "ann:id": "t_seed",
      "trigger": {
        "left": {
          "observable": "time"
        },
        "operator": "<",
        "right": {
          "value": {
            "number": 10
          }
        }
      },
      "interventionIds": [
        "seed"
      ]
    }
  ]