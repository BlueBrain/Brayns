.. _apiatlasexplorer-label:

Atlas Explorer API
==================

This page references the loaders and entrypoints registered by the Atlas Explorer plugin.

Loaders
-------

NRRD loader
~~~~~~~~~~~

Can load the following formats: **.nrrd**.

This loader does not support loading binary data using 'upload-model'.

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "type": {
                "description": "Voxel type to interpret the atlas being loaded",
                "type": "string",
                "enum": [
                    "scalar",
                    "orientation",
                    "flatmap",
                    "layer_distance",
                    "vector"
                ]
            }
        },
        "required": [
            "type"
        ],
        "additionalProperties": false
    }

Entrypoints
-----------

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
            "title": "UseCaseMessage",
            "type": "object",
            "properties": {
                "name": {
                    "description": "Use case name",
                    "type": "string"
                },
                "params_schema": {
                    "title": "JsonSchema",
                    "description": "Use case parameters schema",
                    "type": "object"
                }
            },
            "required": [
                "name",
                "params_schema"
            ],
            "additionalProperties": false
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
            "params",
            "use_case"
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
                "description": "Model bounds",
                "type": "object",
                "readOnly": true,
                "properties": {
                    "max": {
                        "description": "Top front right corner XYZ",
                        "type": "array",
                        "readOnly": true,
                        "items": {
                            "type": "number"
                        },
                        "minItems": 3,
                        "maxItems": 3
                    },
                    "min": {
                        "description": "Bottom back left corner XYZ",
                        "type": "array",
                        "readOnly": true,
                        "items": {
                            "type": "number"
                        },
                        "minItems": 3,
                        "maxItems": 3
                    }
                },
                "required": [
                    "max",
                    "min"
                ],
                "additionalProperties": false
            },
            "info": {
                "title": "ModelInfo",
                "description": "Model-specific info",
                "type": "object",
                "readOnly": true,
                "properties": {
                    "base_transform": {
                        "title": "Transform",
                        "description": "Model transform",
                        "type": "object",
                        "readOnly": true,
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
                    },
                    "load_info": {
                        "title": "LoadInfo",
                        "description": "Model load info",
                        "type": "object",
                        "readOnly": true,
                        "properties": {
                            "load_parameters": {
                                "description": "Loader settings",
                                "readOnly": true
                            },
                            "loader_name": {
                                "description": "Loader name",
                                "type": "string",
                                "readOnly": true
                            },
                            "path": {
                                "description": "File path in case of file load type",
                                "type": "string",
                                "readOnly": true
                            },
                            "source": {
                                "description": "Model load source",
                                "type": "string",
                                "readOnly": true,
                                "enum": [
                                    "from_file",
                                    "from_blob",
                                    "none"
                                ]
                            }
                        },
                        "required": [
                            "load_parameters",
                            "loader_name",
                            "path",
                            "source"
                        ],
                        "additionalProperties": false
                    },
                    "metadata": {
                        "description": "Model-specific metadata",
                        "type": "object",
                        "readOnly": true,
                        "additionalProperties": {
                            "type": "string"
                        }
                    }
                },
                "additionalProperties": false
            },
            "is_visible": {
                "description": "Wether the model is being rendered or not",
                "type": "boolean"
            },
            "model_id": {
                "description": "Model ID",
                "type": "integer",
                "readOnly": true,
                "minimum": 0
            },
            "model_type": {
                "description": "Model type",
                "type": "string",
                "readOnly": true
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
        "required": [
            "bounds",
            "info",
            "model_id",
            "model_type"
        ],
        "additionalProperties": false
    }
