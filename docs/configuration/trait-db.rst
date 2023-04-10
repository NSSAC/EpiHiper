
Trait Databases
===============

**Contents:**

* |traitdb-introduction-synopsis|_

..
  * |traitdb-definition-synopsis|_
  * |traitdb-content-synopsis|_
  * :ref:`traitdb-examples`

.. |traitdb-introduction-synopsis| replace:: Trait databases: EpiHiper allows custom read-only attributes for individuals and locations contain elements.

.. _`traitdb-introduction-synopsis`: `traitdb-introduction`_

.. _traitdb-introduction:

Introduction
------------

EpiHiper supports the use of two custom database tables that are referred to as the person trait database (personTraitDB) and the location trait database (locationTraitDB). These tables provide the opportunity to attach more detailed information to the nodes (people) and the contacts (edges) of the simulation. We remark that these tables are created outside of EpiHiper, usually as part of the construction of synthetic populations and networks. At a high level, the the person- and location trait database tables support construction of interventions that can carefully target specific subset of the population and their contact edges based on these additional attributes. Note that basic epidemic scenarios can often be run without this feature. However, more advanced case studies often benefit from having carefully prepared trait database tables.

**Person trait database:**

The person trait database is indexed by the person ID (pid) and supports any number of associated tables and fields. This may include a person's age group, prior vaccination history, and a flag indicating if they are classified as emergency response personnel, etc.

**Location trait database:**

Each edge of the contact network has an associated location ID (lid) encoding the location where that contact took place.  The location trait database is indexed by the lid and may have any number associated tables and fields. Examples include the latitude and longitude of the location, and details about the location being indoors or outdoors, etc.

**Database:**

EpiHiper currently supports only the PostgreSQL relational database. A user may direct EpiHiper to an existing database through appropriate host names, ports numbers, credentials, schemas and table names. Alternatively, a user may provide an existing database image to EpiHiper that may be loaded to software database.

