Sets
====

**Contents:**

* |sets-introduction-synopsis|_
* |sets-definition-synopsis|_
* |sets-content-synopsis|_
* :ref:`sets-examples`

.. |sets-introduction-synopsis| replace:: Sets: in EpiHiper set contain elements (nodes :math:`N` and/or edges :math:`E`) of the :doc:`network` or values :math:`V` selected from :doc:`person-db`.
.. _`sets-introduction-synopsis`: `sets-introduction`_

.. _sets-introduction:

Introduction
------------

.. admonition:: Synopsis

  |sets-introduction-synopsis|

In EpiHiper sets of are the building blocks of :doc:`initialization` and :doc:`intervention` where they are used to specify the targets of an :doc:`actions`. Furthermore, the size (number of contained elements) can be used to define a :doc:`trigger`.

.. |sets-definition-synopsis| replace:: Definition: syntax for the definition of sets
.. _`sets-definition-synopsis`: `sets-definition`_

.. _sets-definition:

Definition
----------

.. admonition:: Synopsis

  |sets-definition-synopsis|

.. code-block:: bash
  
  set:   id scope content [annotation]
  scope: local | global

.. list-table:: Set definition. 
  :name: sets-definition-spec
  :header-rows: 1

  * - | Name
    - | Type 
    - | Description
  * - | id
    - | `unique id <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L6>`_ 
    - | An id which has to be unique within the list of sets
  * - | scope
    - | local|global
    - | The scope for computing the elements of a set required for correct simulation
  * - | content
    - | :ref:`sets-content`
    - | The content of the set
  * - | ann:* 
    - | `annotation <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L96>`_
    - | Optional annotation of the set

The attribute ``scope`` is used to enhance performance. Local content is faster to compute and suffices in most situations.
The normative JSON schema can be found at:  `set <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L1875>`_ 

.. |sets-content-synopsis| replace:: Content: specification of the elements contained in a set.

.. _`sets-content-synopsis`: `sets-content`_

.. _sets-content:

Content
-------

.. admonition:: Synopsis

  |sets-content-synopsis|

.. code-block:: bash
  
  :              (elementType [scope] operator left right)
               | (elementType [scope] operator selector)
               | (elementType [scope])
               | (elementType table)
               | (operation sets)
               | (set)
  elementType: node | edge | dbField
  scope:       local | global
  operator:      (== | != | <= | < | >= | >)
               | (in | not in)
               | (withTargetNodeIn | withSourceNodeIn | withIncomingEdgeIn)
  left:          (property [feature])
               | (field [table])
  property:      (targetId | sourceId | locationId | duration | weight | active 
                  | targetActivity | sourceActivity | edgeTrait)
               | (id | susceptibilityFactor | infectivityFactor | healthState | nodeTrait)
  right:       value | valueList
  selector:    content
  operations:  union | intersection
  sets:        list(content)
  set:         idRef

We have several alternative syntaxes to specify the content. The third specifies either all nodes or all edges and the fourth specifies all nodes (PIDs) in a table. Here the attribute ``scope`` is only required if the ``elementType`` is ``node``. Furthermore, the valid values for ``feature``, ``field``, and ``table`` must be defined in :doc:`traits` or :doc:`person-db`.

.. list-table:: Set content definition. 
  :name: sets-content-spec
  :header-rows: 1

  * - | Name
    - | Type 
    - | Description
  * - | elementType
    - | string 
    - | The type of the element node | edge | dbField
  * - | scope
    - | string 
    - | The scope (local | global) for computing the elements.
      | Only required if the ``elementType`` is ``node``
  * - | operator
    - | string
    - | The operator used for comparing left and right:
      | == | != | <= | < | >= | > | in | not in
      | | withTargetNodeIn | withSourceNodeIn | withIncomingEdgeIn.
  * - | left 
    - | object
    - | The left operant of the comparison
  * - | property
    - | string
    - | Properties of `edges <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L680>`_ or `nodes <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L637>`_
  * - | right 
    - | object
    - | A `value <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L277>`_ or `list of values <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L315>`_ depending on the operator
  * - | selector 
    - | object
    - | `Set content <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L1783>`_ used to filter nodes or edges based on attributes which are lists.
  * - | operation
    - | string
    - | A set operation either union or intersection
  * - | sets
    - | list
    - | List of set content.
  * - | set
    - | object
    - | Reference to another set definition.
  * - | idRef
    - | string 
    - | A reference of an id of a existing set .

The normative JSON schema can be found at:  `setContent <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L1783>`_ 

.. _sets-examples:

Examples
--------

A set containing all edges of contacts of school children. These are contacts to other children, teachers, or other schill employes.

.. code-block:: JSON

    {
      "id": "school_edges",
      "scope": "local",
      "content": {
        "operation": "union",
        "sets": [
          {
            "scope": "local",
            "elementType": "edge",
            "left": {
              "edge": {
                "property": "targetActivity",
                "feature": "activityType"
              }
            },
            "operator": "==",
            "right": {
              "value": {
                "trait": "activityTrait",
                "feature": "activityType",
                "enum": "school"
              }
            }
          },
          {
            "scope": "local",
            "elementType": "edge",
            "left": {
              "edge": {
                "property": "sourceActivity",
                "feature": "activityType"
              }
            },
            "operator": "==",
            "right": {
              "value": {
                "trait": "activityTrait",
                "feature": "activityType",
                "enum": "school"
              }
            }
          }
        ]
      }
    }
