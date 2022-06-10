.. _apiatlasexplorer-label:

Atlas Explorer API methods
--------------------------

This page references the entrypoints of the Atlas Explorer plugin.

get-available-atlas-usecases
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Returns a list of available atlas visualization usecases for the given model.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "model_id": {
                "description": "ID of the model holding an atlas volume",
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
            "type": "string"
        }
    }

----

visualize-atlas-usecase
~~~~~~~~~~~~~~~~~~~~~~~

Visualizes the specified use case based on the atlas data of the given model.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "model_id": {
                "description": "ID of the model holding an atlas volume",
                "type": "integer",
                "minimum": 0
            },
            "params": {
                "description": "Additional use case parameters"
            },
            "use_case": {
                "description": "Use case name",
                "type": "string"
            }
        },
        "required": [
            "model_id",
            "use_case",
            "params"
        ],
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "bounds": {
                "title": "Bounds",
                "description": "Model axis-aligned bounds",
                "type": "object",
                "readOnly": true,
                "properties": {
                    "max": {
                        "description": "Bounds maximum (top front right corner)",
                        "type": "array",
                        "readOnly": true,
                        "items": {
                            "type": "number"
                        },
                        "minItems": 3,
                        "maxItems": 3
                    },
                    "min": {
                        "description": "Bounds minimum (bottom back left corner)",
                        "type": "array",
                        "readOnly": true,
                        "items": {
                            "type": "number"
                        },
                        "minItems": 3,
                        "maxItems": 3
                    }
                },
                "additionalProperties": false
            },
            "is_visible": {
                "description": "Wether the model is being rendered or not",
                "type": "boolean"
            },
            "metadata": {
                "description": "Model-specific metadata",
                "type": "object",
                "readOnly": true,
                "additionalProperties": {
                    "type": "string"
                }
            },
            "model_id": {
                "description": "Model ID",
                "type": "integer",
                "readOnly": true,
                "minimum": 0
            },
            "transform": {
                "title": "Transform",
                "description": "Model transform",
                "type": "object",
                "properties": {
                    "rotation": {
                        "description": "Rotation XYZW",
                        "type": "array",
                        "items": {
                            "type": "number"
                        },
                        "minItems": 4,
                        "maxItems": 4
                    },
                    "scale": {
                        "description": "Scale XYZ",
                        "type": "array",
                        "items": {
                            "type": "number"
                        },
                        "minItems": 3,
                        "maxItems": 3
                    },
                    "translation": {
                        "description": "Translation XYZ",
                        "type": "array",
                        "items": {
                            "type": "number"
                        },
                        "minItems": 3,
                        "maxItems": 3
                    }
                },
                "additionalProperties": false
            }
        },
        "additionalProperties": false
    }
