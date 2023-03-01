Contact Network
===============

The EpiHiper contact network :math:`G` is a graph that captures the contacts between individuals. It captures the specifics of those contacts as listed in :numref:`network-edge-spec`. Edges are directed and endpoints are referred to as sources and targets.


**Remark:** Note again that :math:`G` only captures contacts and their durations; it does not capture when these happen within an
iteration. It is one of the assumptions of the model that order of contacts within an iteration does not matter.

We point out that there is **no aggregation of edges in the construction of the contact network**. Whereas the may be pairs of individuals in contact at different times with precisely the same characteristics at those times (e.g., household members in contact at beginning and end of days), those contacts will be represented by separated edges; the contacts will not be combined to a single edge with the accumulated time of contact. The EpiHiper transmission process handles the two cases to make them equivalent.

**Remark:** Isolated vertices have no bearing on the disease dynamics and are not represented in the EpiHiper networks.

**Network formats:** EpiHiper supports a binary format and an ASCII format, both of which have a a common JSON header. This header appears as the first line of the file in both formats; it is a standardized JSON `object <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/networkSchema.json>`_ with all newline characters and redundant whitespace characters omitted.

Graph header format
-------------------

The elements of the JSON header are described in :numref:`network-json-header`

.. list-table:: The common JSON header for ASCII and binary formats
  :name: network-json-header
  :header-rows: 1
  
  * - | JSON property
    - | Description
  * - | ann:* 
    - | `Annotation <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L96>`_ for the network
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
    - | JSON `trait object <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L2141>`_ for encoding of activity type
  * - | sizeofEdgeTrait  
    - | The size of the edgeTrait measured in bytes  (currently 0 or 4)
  * - | traitEncoding 
    - | JSON `trait object <https://github.com/NSSAC/EpiHiper-Schema/blob/master/schema/typeRegistry.json#L2141>`_ for encoding of edge features
  * - | hasActiveField 
    - | Boolean flag stating if active is included as edge field
  * - | hasWeightField 
    - | Boolean flag stating if weight is included as edge field
  * - | hasLocationIDField 
    - | Boolean flag stating if a location ID is included as edge field

.. list-table:: Edge properties in the EpiHiper network format. In both ASCII and binary format the order of the fields is the same as the top-to-bottom order listed in the table
  :name: network-edge-spec
  :header-rows: 1
  
  * - | Name
    - | Type (binary \| text)
    - | Description
  * - | targetPID
    - | size_t \| :math:`\mathbb{N}_0`
    - | The PID of the target node
  * - | targetActivity
    - | bitset<32> \| :ref:`encoding <traits-text-encoding>`
    - | The activity of the target node at time of contact
  * - | sourcePID
    - | size_t \| :math:`\mathbb{N}_0`
    - | The PID of the source node
  * - | sourceActivity
    - | bitset<32> \| :ref:`encoding <traits-text-encoding>`
    - | The activity of the source node at time of contact
  * - | duration
    - | double \| :math:`x \ge 0`
    - | Duration of contact in unit number-of-subticks
  * - | [locationID]
    - | size_t \| :math:`\mathbb{N}_0`
    - | The location ID where the contact takes place
  * - | [edgeTrait]
    - | bitset<32> \| :ref:`encoding <traits-text-encoding>`
    - | A per-network customizable bit-field of edge parameters
  * - | [active]
    - | 1 byte bool \| (0 or 1)
    - | A Boolean value specifying if the given edge is active
  * - | [weight]
    - | double \| :math:`x \ge 0`
    - | A weight given to each edge.


**Edge trait edge cases:** If ``sizeofEdgeTrait`` is zero or if traitEncoding is empty, then (a) the ASCII/CSV version has an empty ``edgeTrait`` column (successive commas), and (b) the binary version has no data (0 bytes) for ``edgeTrait``.

ASCII format
------------

After removal of the first line of the file (the common header line), the remaining file is a valid CSV file with the following  header string: 

.. code-block:: bash

  sourcePID,sourceActivity,targetPID,targetActivity,duration
  [,locationID][,edgeTrait][,active][,weight]

The (non-)presence of the optional last four fields is specified in the JSON header object. Note that there are precisely ``numberOfEdges + 2`` lines in the entire CSV file (1 for JSON header, 1 for CSV header, ``numberOfEdges`` edges).

Binary format
-------------

To avoid string interpretation and thus speed up loading of the network EpiHiper supports binary edge encoding. The order of the attributes is the same as in the csv file. Furthermore the binary encoded network must include the same 2 header lines as the ASCII encoding. The binary encoding can be found in :numref:`network-edge-spec`. 
