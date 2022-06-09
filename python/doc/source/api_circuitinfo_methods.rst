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

This entrypoint has no result, the "result" field is still present but is always null.

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

This entrypoint has no result, the "result" field is still present but is always null.

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

This entrypoint has no result, the "result" field is still present but is always null.

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

This entrypoint has no result, the "result" field is still present but is always null.

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

This entrypoint has no result, the "result" field is still present but is always null.

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

This entrypoint has no result, the "result" field is still present but is always null.

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

This entrypoint has no result, the "result" field is still present but is always null.

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

This entrypoint has no result, the "result" field is still present but is always null.

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

This entrypoint has no result, the "result" field is still present but is always null.

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

This entrypoint has no result, the "result" field is still present but is always null.

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

This entrypoint has no result, the "result" field is still present but is always null.

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

This entrypoint has no result, the "result" field is still present but is always null.
