Variables
=========

.. include:: _schema_links.rst

**Contents:**

* |variables-introduction-synopsis|_
* |variables-specification-synopsis|_
* :ref:`variables-examples`

.. |variables-introduction-synopsis| replace:: Variables: are possibly time varying user defined numerical values.
.. _`variables-introduction-synopsis`: `variables-introduction`_

.. _variables-introduction:

Introduction
------------

.. admonition:: Synopsis

   |variables-introduction-synopsis|

Variables are possibly time varying numerical values. They have an initial value and may periodically reset. Variables may be used in a :doc:`trigger` condition, as the target or as the right hand side of an operation (see :doc:`actions`). Variables which can be computed by a single process individually are marked as local. Local variables do not have any communication overhead. Global variables are updated and read by all process. Note, this may introduce race conditions and the identical reproduction of a simulation can not be guaranteed for identical seeds if the the simulation process depends on the variables value during execution phase.

.. |variables-specification-synopsis| replace:: Specification: how to define custom variables. 
.. _`variables-specification-synopsis`: `variables-specification`_

.. _variables-specification:

Specification
-------------

.. admonition:: Synopsis

   |variables-specification-synopsis|

Variables can be defined in :doc:`initialization` and :doc:`intervention`

.. code-block:: text

  variables: list(variable) [annotation]
  variable:  id scope initialValue [reset] [annotation]
  scope:     local | global

.. list-table:: Variable definition. 
  :name: variables-definition-spec
  :header-rows: 1

  * - | Name
    - | Type 
    - | Description
  * - | id
    - | |schema_unique_id|_ 
    - | An id which has to be unique within the list of variables
  * - | scope
    - | local|global
    - | The scope for computing a variable (local | global).
  * - | initialValue
    - | :math:`x \in \mathbb{R}`
    - | The initial value of the variable.
  * - | reset
    - | :math:`n \in \mathbb{N}`
    - | Optionally the variable may be reset to the initial value ever :math:`n` 
      | simulation steps
  * - | ann:* 
    - | |schema_annotation|_
    - | Optional annotation of the variable


The normative JSON schema can be found at:  |schema_variables|_ 

.. _variables-examples:

Examples
--------

**Local variable**:

Maintaining a weekday. The variable `week_day` is incremented by one every tick is reset every 7 days.

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

**Global variable**:

30% of symptomatic individuals are receiving a prophylactic treatment which reduces the infectivity to 20%. If and only if the daily limit (500) and total limit (20,000) is not surpassed. This example introduces the above mentioned race condition when computed in more that one process/thread.

.. code-block:: JSON

  "variables": [
    {
      "id": "totalTreatments",
      "ann:label": "Total Treatments",
      "scope": "global",
      "initialValue": 0
    },
    {
      "id": "dailyTreatments",
      "ann:label": "Daily Treatments",
      "scope": "global",
      "initialValue": 0,
      "reset": 1
    }
  ],
  "interventions": [
    {
      "$comment": "Treatment",
      "trigger": {
        "operator": ">",
        "left": {
          "sizeof": {
            "set": {
              "idRef": "enteredIsymp"
            }
          }
        },
        "right": {
          "value": {
            "number": 0
          }
        }
      },
      "target": {
        "set": {
          "idRef": "enteredIsymp"
        }
      },
      "sampling": {
        "type": "individual",
        "number": 30,
        "sampled": {
          "foreach": [
            {
              "delay": 1,
              "condition": {
                "and": [
                  {
                    "left": {
                      "variable": {
                        "idRef": "dailyTreatments"
                      }
                    },
                    "operator": "<",
                    "right": {
                      "value": {
                        "number": 500
                      }
                    }
                  },
                  {
                    "left": {
                      "variable": {
                        "idRef": "totalTreatments"
                      }
                    },
                    "operator": "<",
                    "right": {
                      "value": {
                        "number": 20000
                      }
                    }
                  }
                ]
              },
              "operations": [
                {
                  "target": {
                    "variable": {
                      "idRef": "dailyTreatments"
                    }
                  },
                  "operator": "+=",
                  "value": {
                    "number": 1
                  }
                },
                {
                  "target": {
                    "variable": {
                      "idRef": "totalTreatments"
                    }
                  },
                  "operator": "+=",
                  "value": {
                    "number": 1
                  }
                },
                {
                  "target": {
                    "node": {
                      "property": "infectivityFactor"
                    }
                  },
                  "operator": "*=",
                  "value": {
                    "number": 0.2
                  }
                },
                {
                  "target": {
                    "node": {
                      "property": "nodeTrait",
                      "feature": "treatment"
                    }
                  },
                  "operator": "=",
                  "value": {
                    "trait": "nodeTrait",
                    "feature": "treatment",
                    "enum": "true"
                  }
                }
              ]
            },
            {
              "delay": 6,
              "condition": {
                "left": {
                  "node": {
                    "property": "nodeTrait",
                    "feature": "treatment"
                  }
                },
                "operator": "==",
                "right": {
                  "value": {
                    "trait": "nodeTrait",
                    "feature": "treatment",
                    "enum": "true"
                  }
                }
              },
              "operations": [
                {
                  "target": {
                    "node": {
                      "property": "infectivityFactor"
                    }
                  },
                  "operator": "/=",
                  "value": {
                    "number": 0.2
                  }
                },
                {
                  "target": {
                    "node": {
                      "property": "nodeTrait",
                      "feature": "treatment"
                    }
                  },
                  "operator": "=",
                  "value": {
                    "trait": "nodeTrait",
                    "feature": "treatment",
                    "enum": "false"
                  }
                }
              ]
            }
          ]
        }
      }
    }
  ]
