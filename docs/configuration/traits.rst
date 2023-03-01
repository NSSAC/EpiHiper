Traits
======

In EpiHiper the notion of configurable trait collections allows one to associate a custom list of dynamically changing attributes to certain model components. Currently EpiHiper allows trait collections for: 

* **Nodes:** The attribute ``nodeTrait`` provides for custom extension of each node's state.

* **Edges:** The attributes ``targetActivity``, ``sourceActivity``, and ``edgeTrait`` provide for custom extensions of each edge's state:

  * ``targetActivity``: detailed information about the activity of the target node of an edge.

  * ``sourceActivity``: detailed information about the activity of the source node of an edge.

  * ``edgeTrait``: detailed information about the edge itself or its location.

Internally, EpiHiper represents trait collections using a suitably defined bit-field, a fact that will be transparent to the modeler, the only exception being the case of too high trait space complexity causing a bit overflow error. 

Specification
-------------

To define these trait collections, the following syntax is used:

.. code-block:: bash

  trait:   traitId list(feature) [annotation]
  feature: featureId list(enum) default [annotation]
  enum:    enumId [annotation]

The trait collections can be specified in several locations (:doc:`network`, :doc:`initialization`, :doc:`intervention`, and :doc:`traits`). New features can be added to a trait in any of these locations. However, modifying a previously defined feature is not allowed and will cause an error. Identical definitions of the same feature in multiple places are allowed.

Here featureName is the name of the trait within the specified trait collection; TypeDefinition is the definition of its type; value is an optional default value for this trait of the defined type, and annString is an optional description of the trait. Trait type definitions. Currently, the only supported TypeDefinition is a string enumeration; it is specified as follows:

enum array< string > default [ <annotation>]

Here string is the enumeration element name and annotation is its optional annotation, the default annotation being string. An enumeration does not have a default value. Note. A likely future extension for TypeDefinition is an integer of a specified bit field precision. A particular example includes activity type. 

.. _traits-text-encoding:

Text encoding
-------------