.. _apicircuitinfo-label:

Circuit Info API methods
------------------------

This page references the entrypoints of the Circuit Info plugin.

ci-get-afferent-cell-ids
~~~~~~~~~~~~~~~~~~~~~~~~

Return a list of afferent synapses cell GIDs from a circuit and a set of source cells.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "path": {
                "description": "Path to the circuit config file",
                "type": "string"
            },
            "sources": {
                "description": "Source cells GIDs",
                "type": "array",
                "items": {
                    "type": "integer",
                    "minimum": 0
                }
            }
        },
        "required": [
            "path",
            "sources"
        ],
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "ids": {
                "description": "Afferent cells unique GIDs",
                "type": "array",
                "items": {
                    "type": "integer",
                    "minimum": 0
                }
            }
        },
        "required": [
            "ids"
        ],
        "additionalProperties": false
    }

----

ci-get-cell-data
~~~~~~~~~~~~~~~~

Return data attached to one or many cells.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "ids": {
                "description": "List of cell IDs",
                "type": "array",
                "items": {
                    "type": "integer",
                    "minimum": 0
                }
            },
            "path": {
                "description": "Path to circuit config file",
                "type": "string"
            },
            "properties": {
                "description": "Desired properties",
                "type": "array",
                "items": {
                    "type": "string"
                }
            }
        },
        "required": [
            "path",
            "ids",
            "properties"
        ],
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "etypes": {
                "description": "Requested cell e-types",
                "type": "array",
                "items": {
                    "type": "string"
                }
            },
            "layers": {
                "description": "Requested cell layers",
                "type": "array",
                "items": {
                    "type": "string"
                }
            },
            "morphology_classes": {
                "description": "Requested cell morphology classes",
                "type": "array",
                "items": {
                    "type": "string"
                }
            },
            "mtypes": {
                "description": "Requested cell m-types",
                "type": "array",
                "items": {
                    "type": "string"
                }
            },
            "orientations": {
                "description": "Requested cell orientations",
                "type": "array",
                "items": {
                    "type": "number"
                }
            },
            "positions": {
                "description": "Requested cell positions",
                "type": "array",
                "items": {
                    "type": "number"
                }
            }
        },
        "required": [
            "etypes",
            "mtypes",
            "morphology_classes",
            "layers",
            "positions",
            "orientations"
        ],
        "additionalProperties": false
    }

----

ci-get-cell-ids
~~~~~~~~~~~~~~~

Return the list of GIDs from a circuit.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "path": {
                "description": "Path to the circuit config file",
                "type": "string"
            },
            "targets": {
                "description": "Targets to query",
                "type": "array",
                "items": {
                    "type": "string"
                }
            }
        },
        "required": [
            "path"
        ],
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "ids": {
                "description": "List of cell GIDs",
                "type": "array",
                "items": {
                    "type": "integer",
                    "minimum": 0
                }
            }
        },
        "required": [
            "ids"
        ],
        "additionalProperties": false
    }

----

ci-get-cell-ids-from-model
~~~~~~~~~~~~~~~~~~~~~~~~~~

Return the list of GIDs from a loaded circuit.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "model_id": {
                "description": "ID of the circuit model",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "model_id"
        ],
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "ids": {
                "description": "List of cell GIDs",
                "type": "array",
                "items": {
                    "type": "integer",
                    "minimum": 0
                }
            }
        },
        "required": [
            "ids"
        ],
        "additionalProperties": false
    }

----

ci-get-efferent-cell-ids
~~~~~~~~~~~~~~~~~~~~~~~~

Return a list of efferent synapses cell GIDs from a circuit and a set of source cells.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "path": {
                "description": "Path to the circuit config file",
                "type": "string"
            },
            "sources": {
                "description": "Source cells GIDs",
                "type": "array",
                "items": {
                    "type": "integer",
                    "minimum": 0
                }
            }
        },
        "required": [
            "path",
            "sources"
        ],
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "ids": {
                "description": "Efferent cells unique GIDs",
                "type": "array",
                "items": {
                    "type": "integer",
                    "minimum": 0
                }
            }
        },
        "required": [
            "ids"
        ],
        "additionalProperties": false
    }

----

ci-get-projection-efferent-cell-ids
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Return a list of efferent projected synapses cell GIDs from a circuit and a set of source cells.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "path": {
                "description": "Path to the circuit config file",
                "type": "string"
            },
            "projection": {
                "description": "Projection name",
                "type": "string"
            },
            "sources": {
                "description": "Source cells GIDs",
                "type": "array",
                "items": {
                    "type": "integer",
                    "minimum": 0
                }
            }
        },
        "required": [
            "path",
            "projection",
            "sources"
        ],
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "ids": {
                "description": "Efferent cells GIDs",
                "type": "array",
                "items": {
                    "type": "integer",
                    "minimum": 0
                }
            }
        },
        "required": [
            "ids"
        ],
        "additionalProperties": false
    }

----

ci-get-projections
~~~~~~~~~~~~~~~~~~

Return a list of projection names available on a circuit.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "path": {
                "description": "Path to the circuit config file",
                "type": "string"
            }
        },
        "required": [
            "path"
        ],
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "projections": {
                "description": "Projections names",
                "type": "array",
                "items": {
                    "type": "string"
                }
            }
        },
        "required": [
            "projections"
        ],
        "additionalProperties": false
    }

----

ci-get-report-info
~~~~~~~~~~~~~~~~~~

Return information about a specific report from a circuit.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "path": {
                "description": "Path to the circuit BlueConfig from which to get the report",
                "type": "string"
            },
            "report": {
                "description": "Name of the report from where to get the information",
                "type": "string"
            }
        },
        "required": [
            "path",
            "report"
        ],
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "data_unit": {
                "description": "Unit of the report values. Can be (mV, mA, ...)",
                "type": "string"
            },
            "end_time": {
                "description": "Time at which the simulation ends",
                "type": "number"
            },
            "frame_count": {
                "description": "Number of simulation frames in the report",
                "type": "integer",
                "minimum": 0
            },
            "frame_size": {
                "description": "Number of values per frame in the report",
                "type": "integer",
                "minimum": 0
            },
            "start_time": {
                "description": "Time at which the simulation starts",
                "type": "number"
            },
            "time_step": {
                "description": "Time between two consecutive simulation frames",
                "type": "number"
            },
            "time_unit": {
                "description": "Unit of the report time values",
                "type": "string"
            }
        },
        "required": [
            "start_time",
            "end_time",
            "time_step",
            "data_unit",
            "time_unit",
            "frame_count",
            "frame_size"
        ],
        "additionalProperties": false
    }

----

ci-get-reports
~~~~~~~~~~~~~~

Return a list of reports from a circuit.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "path": {
                "description": "Path to the circuit config file",
                "type": "string"
            }
        },
        "required": [
            "path"
        ],
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "reports": {
                "description": "Report names",
                "type": "array",
                "items": {
                    "type": "string"
                }
            }
        },
        "required": [
            "reports"
        ],
        "additionalProperties": false
    }

----

ci-get-spike-report-info
~~~~~~~~~~~~~~~~~~~~~~~~

Return wether the circuit has a spike report and its path.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "path": {
                "description": "Path of the circuit to test",
                "type": "string"
            }
        },
        "required": [
            "path"
        ],
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "exists": {
                "description": "Check if the report exists",
                "type": "boolean"
            },
            "path": {
                "description": "Path to the spike report",
                "type": "string"
            }
        },
        "required": [
            "path",
            "exists"
        ],
        "additionalProperties": false
    }

----

ci-get-targets
~~~~~~~~~~~~~~

Return a list of targets from a circuit.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "path": {
                "description": "Path to the circuit config file",
                "type": "string"
            }
        },
        "required": [
            "path"
        ],
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "targets": {
                "description": "Target names",
                "type": "array",
                "items": {
                    "type": "string"
                }
            }
        },
        "required": [
            "targets"
        ],
        "additionalProperties": false
    }

----

ci-info
~~~~~~~

Return general info about a circuit.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "path": {
                "description": "Path of the circuit config file",
                "type": "string"
            }
        },
        "required": [
            "path"
        ],
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "cells_count": {
                "description": "Number of cells in this circuit",
                "type": "integer",
                "minimum": 0
            },
            "cells_properties": {
                "description": "List of available cell properties",
                "type": "array",
                "items": {
                    "type": "string"
                }
            },
            "e_types": {
                "description": "List of electrical types available in this circuit",
                "type": "array",
                "items": {
                    "type": "string"
                }
            },
            "m_types": {
                "description": "List of morphology types available in this circuit",
                "type": "array",
                "items": {
                    "type": "string"
                }
            },
            "reports": {
                "description": "List of report names",
                "type": "array",
                "items": {
                    "type": "string"
                }
            },
            "spike_report": {
                "description": "Path to the spike report file",
                "type": "string"
            },
            "targets": {
                "description": "List of target names",
                "type": "array",
                "items": {
                    "type": "string"
                }
            }
        },
        "required": [
            "cells_count",
            "cells_properties",
            "m_types",
            "e_types",
            "targets",
            "reports",
            "spike_report"
        ],
        "additionalProperties": false
    }
