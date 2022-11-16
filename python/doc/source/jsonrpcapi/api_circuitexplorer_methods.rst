.. _apicircuitexplorer-label:

Circuit Explorer API methods
----------------------------

This page references the entrypoints of the Circuit Explorer plugin.

get-circuit-ids
~~~~~~~~~~~~~~~

For neuron/astrocyte/vasculature models, return the list of ids loaded.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "model_id": {
                "description": "ID of the model to query",
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
        "type": "array",
        "items": {
            "type": "integer",
            "minimum": 0
        }
    }

----

set-circuit-thickness
~~~~~~~~~~~~~~~~~~~~~

Modify the geometry radiuses (spheres, cones, cylinders and SDF geometries).

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "model_id": {
                "description": "ID of the circuit model",
                "type": "integer",
                "minimum": 0
            },
            "radius_multiplier": {
                "description": "Scaling factor",
                "type": "number"
            }
        },
        "required": [
            "model_id",
            "radius_multiplier"
        ],
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.
