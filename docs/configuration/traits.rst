..
  .. |traits-specification-synopsis| replace:: States: a declaration of states of the contagion model
  .. _`traits-specification-synopsis`: `traits-specification`_

  .. _traits-specification:

Traits
======

**Contents:**

* |traits-introduction-synopsis|_
* |traits-specification-synopsis|_
* |traits-text-encoding-synopsis|_
* :ref:`traits-examples`

.. |traits-introduction-synopsis| replace:: Traits: collection of customizable dynamically changing attributes.
.. _`traits-introduction-synopsis`: `traits-introduction`_

.. _traits-introduction:

Introduction
------------

.. admonition:: Synopsis

   |traits-introduction-synopsis|

In EpiHiper the notion of configurable trait collections allows one to associate a custom list of dynamically changing attributes to certain model components. This effectively allows the user to extend the state of the system. Currently EpiHiper allows trait collections for: 

* **Nodes:** The attribute ``nodeTrait`` provides for custom extension of each node's state.

* **Edges:** The attributes ``targetActivity``, ``sourceActivity``, and ``edgeTrait`` provide for custom extensions of each edge's state:

  * ``targetActivity``: detailed information about the activity of the target node of an edge.

  * ``sourceActivity``: detailed information about the activity of the source node of an edge.

  * ``edgeTrait``: detailed information about the edge itself or its location.

Internally, EpiHiper represents trait collections using a suitably defined bit-field, a fact that will be transparent to the modeler, the only exception being the case of too high trait space complexity causing a bit overflow error. 

.. |traits-specification-synopsis| replace:: Specification: how to define custom attributes. 
.. _`traits-specification-synopsis`: `traits-specification`_

.. _traits-specification:

Specification
-------------

.. admonition:: Synopsis

   |traits-specification-synopsis|

To define these trait collections, the following syntax is used:

.. code-block:: bash

  trait:   traitId list(feature) [annotation]
  feature: featureId list(enum) default [annotation]
  enum:    enumId [annotation]

The trait collections can be specified in several locations (:doc:`network`, :doc:`initialization`, :doc:`intervention`, and :doc:`traits`). New features can be added to a trait in any of these locations. However, modifying a previously defined feature is not allowed and will cause an error. Identical definitions of the same feature in multiple places are allowed.

Here ``featureId`` is the unique id of a feature within the specified trait collection; ``enumId`` is a unique identifier within the scope of a feature; a ``default`` value must be provided for each feature so that the state will be well defined at the beginning of any simulation; furthermore optional `annotation <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L96>`_ may be provided

The normative JSON schema can be found at:

  * `trait <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L2141>`_
  * `feature <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L2102>`_
  * `enum <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L2068>`_

.. |traits-text-encoding-synopsis| replace:: Text encoding: format of traits utilized in csv files.
.. _`traits-text-encoding-synopsis`: `traits-text-encoding`_

.. _traits-text-encoding:

Text encoding
-------------

.. admonition:: Synopsis

  |traits-text-encoding-synopsis|

Text encoding of traits is is required for text format of the :doc:`network`.

.. code-block:: bash

  featureIndex_1:enumIndex_1|featureIndex_2|enumIndex_2, ..., featureIndex_n|enumIndex_n

Here the index starts with ``1`` for the first feature or enum defined in JSON according to the `schema <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L2141>`_. If a feature is omitted in the encoding the default value is used.


.. _traits-examples:

Examples
--------


**JSON activity trait** encoding:

.. code-block:: JSON

    "activityEncoding": {
      "id": "activityTrait",
      "features": [
        {
          "id": "activityType",
          "default": "other",
          "enums": [
            {
              "id": "home"
            },
            {
              "id": "work"
            },
            {
              "id": "shop"
            },
            {
              "id": "other"
            },
            {
              "id": "school"
            },
            {
              "id": "college"
            },
            {
              "id": "religion"
            }
          ]
        }
      ]
    }


**Text encoding** of activity type ``work`` based on the above described activity trait:

.. code-block:: bash

  1:2
