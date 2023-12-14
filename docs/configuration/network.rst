Contact Network
===============

**Contents:**

* |network-introduction-synopsis|_
* |network-edges-synopsis|_
* |network-metadata-synopsis|_
* |network-encoding-synopsis|_
* :ref:`network-examples`

.. |network-introduction-synopsis| replace:: Contact Network: a graph :math:`G` that captures the contacts between individuals.
.. _`network-introduction-synopsis`: `network-introduction`_

.. _network-introduction:

Introduction
------------

.. admonition:: Synopsis

   |network-introduction-synopsis|

The EpiHiper contact network :math:`G` is a graph that captures the contacts between individuals. It captures the specifics of those contacts as listed in :numref:`network-edge-spec`. Edges are directed and endpoints are referred to as sources and targets.

**Remark:** Note again that :math:`G` only captures contacts and their durations; it does not capture when these happen within an
iteration. It is one of the assumptions of the model that order of contacts within an iteration does not matter.

We point out that there is **no aggregation of edges in the construction of the contact network**. Whereas there may be pairs of individuals in contact at different times with precisely the same characteristics at those times (e.g., household members in contact at beginning and end of days), those contacts will be represented by separated edges; the contacts will not be combined to a single edge with the accumulated time of contact. The EpiHiper transmission process handles the two cases to make them equivalent.

**Remark:** Isolated vertices have no bearing on the disease dynamics and are not represented in the EpiHiper networks.

.. |network-edges-synopsis| replace:: Edges: the edges of the contact network
.. _`network-edges-synopsis`: `network-edges`_

.. _network-edges:

Edges
-----

.. admonition:: Synopsis

   |network-edges-synopsis|

To define the edges of the contact network, the following syntax is used:

.. code-block:: text

  edge: targetPID targetActivity sourcePID sourceActivity duration
        [locationID] [edgeTrait] [active] [weight]

Each edge in the contact network has the attributes given in :numref:`network-edge-spec`. Attributes

.. list-table:: Edge properties in the EpiHiper network format. 
  :name: network-edge-spec
  :header-rows: 1

  * - | Name
    - | Type 
    - | Description
  * - | targetPID
    - | :math:`n \in \mathbb{N}_0`
    - | The PID of the target node
  * - | targetActivity
    - | :doc:`activityTrait <traits>`
    - | The activity of the target node at time of contact
  * - | sourcePID
    - | :math:`n \in \mathbb{N}_0`
    - | The PID of the source node
  * - | sourceActivity
    - | :doc:`activityTrait <traits>`
    - | The activity of the source node at time of contact
  * - | duration
    - | :math:`n \in \mathbb{N}_0`
    - | Duration of contact in unit number-of-subticks
  * - | [locationID]
    - | :math:`n \in \mathbb{N}_0`
    - | The location ID where the contact takes place
  * - | [edgeTrait]
    - | :doc:`edgeTrait <traits>`
    - | A per-network customizable bit-field of edge parameters
  * - | [active]
    - | Boolean
    - | A Boolean value specifying if the given edge is active
  * - | [weight]
    - | :math:`0 \le x \in \mathbb{R}`
    - | A weight given to each edge.

.. |network-metadata-synopsis| replace:: Meta Data: information describing network specifics 
.. _`network-metadata-synopsis`: `network-metadata`_

.. _network-metadata:

Meta Data
----------

.. admonition:: Synopsis

   |network-metadata-synopsis|

To define the meta data of the contact network, the following syntax is used:

.. code-block:: text

  : encoding accumulationTime timeResolution numberOfNodes numberOfEdges
    sizeofPID sizeofActivity activityEncoding sizeofEdgeTrait traitEncoding
    hasActiveField hasWeightField hasLocationIDField [annotation]

.. list-table:: List of meta data attributes
  :name: network-json-header
  :header-rows: 1
  
  * - | JSON property
    - | Description
  * - | encoding
    - | binary or text
  * - | accumulationTime 
    - | An annotation string specifying the duration of network accumulation
      | (default 24 hours)
  * - | timeResolution 
    - | The maximal value of the duration field of the network edges; captures
      | the resolution used in the network accumulation per tick.
  * - | numberOfNodes 
    - | The number of nodes in the network
  * - | numberOfEdges 
    - | The number of edges in the network
  * - | sizeofPID 
    - | The size of the PIDs measured in bytes
  * - | sizeofActivity 
    - | The size of the activities measured in bytes (currently 4)
  * - | activityEncoding 
    - | JSON `trait <../schema/trait.html#trait>`__ for encoding of activity type
  * - | sizeofEdgeTrait  
    - | The size of the edgeTrait measured in bytes  (currently 0 or 4)
  * - | traitEncoding 
    - | JSON `trait <../schema/trait.html#trait>`__ for encoding of edge features
  * - | hasActiveField 
    - | Boolean flag stating if active is included as edge field
  * - | hasWeightField 
    - | Boolean flag stating if weight is included as edge field
  * - | hasLocationIDField 
    - | Boolean flag stating if a location ID is included as edge field
  * - | ann:* 
    - | :doc:`annotation </schema/annotation>` for the network


.. |network-encoding-synopsis| replace:: Encoding: EpiHiper supports a binary format and an ASCII format.
.. _`network-encoding-synopsis`: `network-encoding`_

.. _network-encoding:

Encoding
--------

.. admonition:: Synopsis

   |network-encoding-synopsis|

EpiHiper supports a binary format and an ASCII format, both of which have common meta data. This appears as the first line of the file in both formats; it is a standardized JSON :doc:`network </schema/network>` with all newline characters and redundant whitespace characters omitted. Furthermore the second line contains the column headers in both format. Optional attributes ([...]) are omitted when encoding the contact network.

.. code-block:: text

  sourcePID,sourceActivity,targetPID,targetActivity,duration
  [,locationID][,edgeTrait][,active][,weight]


.. list-table:: Edge attribute encoding. In both ASCII and binary format the order of the fields is the same as the top-to-bottom order listed in the table
  :name: network-edge-encoding
  :header-rows: 1
  
  * - | Name
    - | Binary
    - | Text
  * - | targetPID
    - | size_t 
    - | :math:`n \in \mathbb{N}_0`
  * - | targetActivity
    - | bitset<32> 
    - | :ref:`trait encoding <traits-text-encoding>`
  * - | sourcePID
    - | size_t 
    - | :math:`n \in \mathbb{N}_0`
  * - | sourceActivity
    - | bitset<32> 
    - | :ref:`trait encoding <traits-text-encoding>`
  * - | duration
    - | double 
    - | :math:`0 \le x \in \mathbb{R}`
  * - | [locationID]
    - | size_t 
    - | :math:`n \in \mathbb{N}_0`
  * - | [edgeTrait]
    - | bitset<32> 
    - | :ref:`trait encoding <traits-text-encoding>`
  * - | [active]
    - | bool 
    - | (0 or 1)
  * - | [weight]
    - | double 
    - | :math:`0 \le x \in \mathbb{R}`


**ASCII format**
  After removal of the first line of the file (the common header line), the remaining file is a valid CSV file 

**Binary format**
  To avoid string interpretation and thus speed up loading of the network EpiHiper supports binary edge encoding. The order of the attributes is the same as in the csv file.  Note, that due to data alignment in C the size of the binary encoded edge will be larger than the sum of the attribute sizes. 

.. _network-examples:

Examples
--------

**JSON graph header** (first row) formatted for better readability:

.. code-block:: JSON

  {
    "$schema": "https://raw.githubusercontent.com/NSSAC/EpiHiper-Schema/master/schema/networkSchema.json",
    "epiHiperSchema": "https://raw.githubusercontent.com/NSSAC/EpiHiper-Schema/master/schema/networkSchema.json",
    "ann:label": "Wyoming(2017) - config_min_5_max_100_alpha_400 Wednesday network",
    "encoding": "text",
    "accumulationTime": "24 hours",
    "timeResolution": 86400,
    "numberOfNodes": 544276,
    "numberOfEdges": 27747598,
    "sizeofPID": 8,
    "sizeofActivity": 4,
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
    },
    "sizeofEdgeTrait": 0,
    "edgeTraitEncoding": {
      "id": "edgeTrait",
      "features": []
    },
    "hasLocationIDField": true,
    "hasActiveField": false,
    "hasWeightField": false
  }

**Text encoding** starting with row 2 (csv column headers) of text encoding:

.. code-block:: text

  targetPID,targetActivity,sourcePID,sourceActivity,duration,LID
  0,1:2,10105,1:2,900,7692
  0,1:2,10905,1:2,1800,7692
  0,1:2,11094,1:2,6840,7692
  0,1:2,11134,1:2,1800,7692