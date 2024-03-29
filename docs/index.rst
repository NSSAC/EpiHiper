.. EpiHiper documentation master file, created by
   sphinx-quickstart on Fri Feb 17 11:11:18 2023.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to EpiHiper's documentation!
====================================

.. toctree::
   :maxdepth: 2
   :caption: Contents:

.. image:: /_images/epihiper-icon.png
   :alt: epihiper-icon

EpiHiper captures a discrete-time, contagion model evolving over a network. For given population and contact network, the model incorporates a **transmission process** (contagion) and a **within-host progression process**. Furthermore EpiHiper supports **programmable interventions**. Interventions are supported through a flexible and general framework for specifying intervention targets of sub-populations or contacts and a rich set of **actions** that can be associated to interventions to modify states. Additionally, it permits per-person or per-edge, customizable traits that can be used to influence when interventions trigger, or that can serve as conditions for actions to be executed.

.. toctree::
   :maxdepth: 2
   :caption: Getting started

   quickstart/get-started

.. toctree::
   :maxdepth: 2
   :caption: Configuration

   configuration/model.rst
   configuration/network.rst
   configuration/traits.rst
   configuration/sets.rst
   configuration/actions.rst
   configuration/initialization.rst
   configuration/intervention.rst
   configuration/trigger.rst
   configuration/variables.rst
   configuration/observables.rst
   configuration/trait-db.rst
   
.. toctree::
   :maxdepth: 2
   :caption: Examples
   
   examples/examples.rst
   
.. toctree::
   :maxdepth: 2
   :caption: Schema
   
   schema/actionEnsemble.rst
   schema/annotation.rst
   schema/edgeProperty.rst
   schema/nodeProperty.rst
   schema/network.rst
   schema/observable.rst
   schema/operation.rst
   schema/state.rst
   schema/trait.rst
   schema/transmission.rst
   schema/transition.rst
   schema/variable.rst

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
