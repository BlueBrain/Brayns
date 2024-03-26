.. _apicore-label:

Core API
========

This page references the loaders and entrypoints registered by the Core plugin.

Loaders
-------

mesh
~~~~

Can load the following formats: **off**, **stl**, **ply**, **obj**.

This loader does not support loading binary data using 'upload-model'.

.. jsonschema::

    {}

----

mhd-volume
~~~~~~~~~~

Can load the following formats: **mhd**.

This loader does not support loading binary data using 'upload-model'.

.. jsonschema::

    {}

----

raw-volume
~~~~~~~~~~

Can load the following formats: **raw**.

This loader supports loading binary data using 'upload-model'.

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "data_type": {
                "description": "Volume byte data type",
                "type": "string",
                "enum": [
                    "unsinged_char",
                    "short",
                    "unsigned_short",
                    "half_float",
                    "float",
                    "double"
                ]
            },
            "dimensions": {
                "description": "Volume grid size XYZ",
                "type": "array",
                "items": {
                    "type": "integer",
                    "minimum": 0
                },
                "minItems": 3,
                "maxItems": 3
            },
            "spacing": {
                "description": "Volume grid cell spacing XYZ",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            }
        },
        "required": [
            "data_type",
            "dimensions",
            "spacing"
        ],
        "additionalProperties": false
    }

Entrypoints
-----------

add-bounded-planes
~~~~~~~~~~~~~~~~~~

Adds a list of axis-aligned bound limited planes.

**Params**:

.. jsonschema::

    {
        "type": "array",
        "items": {
            "title": "GeometryWithColor",
            "type": "object",
            "properties": {
                "color": {
                    "description": "Geometry color",
                    "type": "array",
                    "items": {
                        "type": "number"
                    },
                    "minItems": 4,
                    "maxItems": 4
                },
                "geometry": {
                    "title": "BoundedPlane",
                    "description": "Geometry data",
                    "type": "object",
                    "properties": {
                        "bounds": {
                            "title": "Box",
                            "description": "Axis-aligned bounds to limit the plane geometry",
                            "type": "object",
                            "properties": {
                                "max": {
                                    "description": "Top front right corner XYZ",
                                    "type": "array",
                                    "items": {
                                        "type": "number"
                                    },
                                    "minItems": 3,
                                    "maxItems": 3
                                },
                                "min": {
                                    "description": "Bottom back left corner XYZ",
                                    "type": "array",
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
                        "coefficients": {
                            "description": "Equation coefficients ABCD from Ax + By + Cz + D = 0",
                            "type": "array",
                            "items": {
                                "type": "number"
                            },
                            "minItems": 4,
                            "maxItems": 4
                        }
                    },
                    "required": [
                        "bounds",
                        "coefficients"
                    ],
                    "additionalProperties": false
                }
            },
            "required": [
                "color",
                "geometry"
            ],
            "additionalProperties": false
        }
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

----

add-boxes
~~~~~~~~~

Adds a list of boxes to the scene.

**Params**:

.. jsonschema::

    {
        "type": "array",
        "items": {
            "title": "GeometryWithColor",
            "type": "object",
            "properties": {
                "color": {
                    "description": "Geometry color",
                    "type": "array",
                    "items": {
                        "type": "number"
                    },
                    "minItems": 4,
                    "maxItems": 4
                },
                "geometry": {
                    "title": "Box",
                    "description": "Geometry data",
                    "type": "object",
                    "properties": {
                        "max": {
                            "description": "Top front right corner XYZ",
                            "type": "array",
                            "items": {
                                "type": "number"
                            },
                            "minItems": 3,
                            "maxItems": 3
                        },
                        "min": {
                            "description": "Bottom back left corner XYZ",
                            "type": "array",
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
                }
            },
            "required": [
                "color",
                "geometry"
            ],
            "additionalProperties": false
        }
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

----

add-capsules
~~~~~~~~~~~~

Adds a list of capsules to the scene.

**Params**:

.. jsonschema::

    {
        "type": "array",
        "items": {
            "title": "GeometryWithColor",
            "type": "object",
            "properties": {
                "color": {
                    "description": "Geometry color",
                    "type": "array",
                    "items": {
                        "type": "number"
                    },
                    "minItems": 4,
                    "maxItems": 4
                },
                "geometry": {
                    "title": "Capsule",
                    "description": "Geometry data",
                    "type": "object",
                    "properties": {
                        "p0": {
                            "description": "Start point of the capsule XYZ",
                            "type": "array",
                            "items": {
                                "type": "number"
                            },
                            "minItems": 3,
                            "maxItems": 3
                        },
                        "p1": {
                            "description": "End point of the capsule XYZ",
                            "type": "array",
                            "items": {
                                "type": "number"
                            },
                            "minItems": 3,
                            "maxItems": 3
                        },
                        "r0": {
                            "description": "Capsule radius at p0",
                            "type": "number"
                        },
                        "r1": {
                            "description": "Capsule radius at p1",
                            "type": "number"
                        }
                    },
                    "required": [
                        "p0",
                        "p1",
                        "r0",
                        "r1"
                    ],
                    "additionalProperties": false
                }
            },
            "required": [
                "color",
                "geometry"
            ],
            "additionalProperties": false
        }
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

----

add-clipping-bounded-planes
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Add a list of axis-aligned bound limited clipping planes.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "invert_normals": {
                "description": "Switches clipping side",
                "type": "boolean",
                "default": false
            },
            "primitives": {
                "description": "Clipping primitive list",
                "type": "array",
                "items": {
                    "title": "BoundedPlane",
                    "type": "object",
                    "properties": {
                        "bounds": {
                            "title": "Box",
                            "description": "Axis-aligned bounds to limit the plane geometry",
                            "type": "object",
                            "properties": {
                                "max": {
                                    "description": "Top front right corner XYZ",
                                    "type": "array",
                                    "items": {
                                        "type": "number"
                                    },
                                    "minItems": 3,
                                    "maxItems": 3
                                },
                                "min": {
                                    "description": "Bottom back left corner XYZ",
                                    "type": "array",
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
                        "coefficients": {
                            "description": "Equation coefficients ABCD from Ax + By + Cz + D = 0",
                            "type": "array",
                            "items": {
                                "type": "number"
                            },
                            "minItems": 4,
                            "maxItems": 4
                        }
                    },
                    "required": [
                        "bounds",
                        "coefficients"
                    ],
                    "additionalProperties": false
                }
            }
        },
        "required": [
            "primitives"
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

----

add-clipping-boxes
~~~~~~~~~~~~~~~~~~

Add a list of clipping boxes to the scene.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "invert_normals": {
                "description": "Switches clipping side",
                "type": "boolean",
                "default": false
            },
            "primitives": {
                "description": "Clipping primitive list",
                "type": "array",
                "items": {
                    "title": "Box",
                    "type": "object",
                    "properties": {
                        "max": {
                            "description": "Top front right corner XYZ",
                            "type": "array",
                            "items": {
                                "type": "number"
                            },
                            "minItems": 3,
                            "maxItems": 3
                        },
                        "min": {
                            "description": "Bottom back left corner XYZ",
                            "type": "array",
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
                }
            }
        },
        "required": [
            "primitives"
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

----

add-clipping-capsules
~~~~~~~~~~~~~~~~~~~~~

Add a list of clipping capsules to the scene.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "invert_normals": {
                "description": "Switches clipping side",
                "type": "boolean",
                "default": false
            },
            "primitives": {
                "description": "Clipping primitive list",
                "type": "array",
                "items": {
                    "title": "Capsule",
                    "type": "object",
                    "properties": {
                        "p0": {
                            "description": "Start point of the capsule XYZ",
                            "type": "array",
                            "items": {
                                "type": "number"
                            },
                            "minItems": 3,
                            "maxItems": 3
                        },
                        "p1": {
                            "description": "End point of the capsule XYZ",
                            "type": "array",
                            "items": {
                                "type": "number"
                            },
                            "minItems": 3,
                            "maxItems": 3
                        },
                        "r0": {
                            "description": "Capsule radius at p0",
                            "type": "number"
                        },
                        "r1": {
                            "description": "Capsule radius at p1",
                            "type": "number"
                        }
                    },
                    "required": [
                        "p0",
                        "p1",
                        "r0",
                        "r1"
                    ],
                    "additionalProperties": false
                }
            }
        },
        "required": [
            "primitives"
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

----

add-clipping-planes
~~~~~~~~~~~~~~~~~~~

Add a list of clipping planes to the scene.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "invert_normals": {
                "description": "Switches clipping side",
                "type": "boolean",
                "default": false
            },
            "primitives": {
                "description": "Clipping primitive list",
                "type": "array",
                "items": {
                    "title": "Plane",
                    "type": "object",
                    "properties": {
                        "coefficients": {
                            "description": "Equation coefficients ABCD from Ax + By + Cz + D = 0",
                            "type": "array",
                            "items": {
                                "type": "number"
                            },
                            "minItems": 4,
                            "maxItems": 4
                        }
                    },
                    "required": [
                        "coefficients"
                    ],
                    "additionalProperties": false
                }
            }
        },
        "required": [
            "primitives"
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

----

add-clipping-spheres
~~~~~~~~~~~~~~~~~~~~

Add a list of clipping spheres to the scene.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "invert_normals": {
                "description": "Switches clipping side",
                "type": "boolean",
                "default": false
            },
            "primitives": {
                "description": "Clipping primitive list",
                "type": "array",
                "items": {
                    "title": "Sphere",
                    "type": "object",
                    "properties": {
                        "center": {
                            "description": "Sphere center XYZ",
                            "type": "array",
                            "items": {
                                "type": "number"
                            },
                            "minItems": 3,
                            "maxItems": 3
                        },
                        "radius": {
                            "description": "Sphere radius",
                            "type": "number"
                        }
                    },
                    "required": [
                        "center",
                        "radius"
                    ],
                    "additionalProperties": false
                }
            }
        },
        "required": [
            "primitives"
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

----

add-light-ambient
~~~~~~~~~~~~~~~~~

Adds an ambient light which iluminates the scene from all directions.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "color": {
                "description": "Light color RGB normalized",
                "type": "array",
                "default": [
                    1,
                    1,
                    1
                ],
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "intensity": {
                "description": "Light intensity",
                "type": "number",
                "default": 1,
                "minimum": 0
            }
        },
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

----

add-light-directional
~~~~~~~~~~~~~~~~~~~~~

Adds a directional light which iluminates the scene from a given direction.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "color": {
                "description": "Light color RGB normalized",
                "type": "array",
                "default": [
                    1,
                    1,
                    1
                ],
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "direction": {
                "description": "Light direction XYZ",
                "type": "array",
                "default": [
                    -1,
                    -1,
                    0
                ],
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "intensity": {
                "description": "Light intensity",
                "type": "number",
                "default": 1,
                "minimum": 0
            }
        },
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

----

add-light-quad
~~~~~~~~~~~~~~

Add a quad light which iluminates the scene on a specific area.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "color": {
                "description": "Light color RGB normalized",
                "type": "array",
                "default": [
                    1,
                    1,
                    1
                ],
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "edge1": {
                "description": "Edge 1 XYZ",
                "type": "array",
                "default": [
                    1,
                    0,
                    0
                ],
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "edge2": {
                "description": "Edge 2 XYZ",
                "type": "array",
                "default": [
                    0,
                    0,
                    1
                ],
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "intensity": {
                "description": "Light intensity",
                "type": "number",
                "default": 1,
                "minimum": 0
            },
            "position": {
                "description": "Light base corner position XYZ",
                "type": "array",
                "default": [
                    0,
                    0,
                    0
                ],
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            }
        },
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

----

add-light-sphere
~~~~~~~~~~~~~~~~

Add a sphere or a point light (radius = 0).

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "color": {
                "description": "Light color RGB normalized",
                "type": "array",
                "default": [
                    1,
                    1,
                    1
                ],
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "intensity": {
                "description": "Light intensity",
                "type": "number",
                "default": 1,
                "minimum": 0
            },
            "position": {
                "description": "Light position XYZ",
                "type": "array",
                "default": [
                    0,
                    0,
                    0
                ],
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "radius": {
                "description": "Sphere radius",
                "type": "number",
                "default": 0
            }
        },
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

----

add-model
~~~~~~~~~

Add model from path and return model descriptor on success.

This entrypoint is asynchronous, it means that it can take a long time and send
progress notifications.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "loader_name": {
                "description": "Name of the loader used to parse the model file",
                "type": "string"
            },
            "loader_properties": {
                "description": "Settings to configure the loading process"
            },
            "path": {
                "description": "Path of the file to load",
                "type": "string"
            }
        },
        "required": [
            "loader_name",
            "loader_properties",
            "path"
        ],
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "array",
        "items": {
            "title": "ModelInstance",
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
    }

----

add-planes
~~~~~~~~~~

Adds a list of planes to the scene.

**Params**:

.. jsonschema::

    {
        "type": "array",
        "items": {
            "title": "GeometryWithColor",
            "type": "object",
            "properties": {
                "color": {
                    "description": "Geometry color",
                    "type": "array",
                    "items": {
                        "type": "number"
                    },
                    "minItems": 4,
                    "maxItems": 4
                },
                "geometry": {
                    "title": "Plane",
                    "description": "Geometry data",
                    "type": "object",
                    "properties": {
                        "coefficients": {
                            "description": "Equation coefficients ABCD from Ax + By + Cz + D = 0",
                            "type": "array",
                            "items": {
                                "type": "number"
                            },
                            "minItems": 4,
                            "maxItems": 4
                        }
                    },
                    "required": [
                        "coefficients"
                    ],
                    "additionalProperties": false
                }
            },
            "required": [
                "color",
                "geometry"
            ],
            "additionalProperties": false
        }
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

----

add-spheres
~~~~~~~~~~~

Adds a list of spheres to the scene.

**Params**:

.. jsonschema::

    {
        "type": "array",
        "items": {
            "title": "GeometryWithColor",
            "type": "object",
            "properties": {
                "color": {
                    "description": "Geometry color",
                    "type": "array",
                    "items": {
                        "type": "number"
                    },
                    "minItems": 4,
                    "maxItems": 4
                },
                "geometry": {
                    "title": "Sphere",
                    "description": "Geometry data",
                    "type": "object",
                    "properties": {
                        "center": {
                            "description": "Sphere center XYZ",
                            "type": "array",
                            "items": {
                                "type": "number"
                            },
                            "minItems": 3,
                            "maxItems": 3
                        },
                        "radius": {
                            "description": "Sphere radius",
                            "type": "number"
                        }
                    },
                    "required": [
                        "center",
                        "radius"
                    ],
                    "additionalProperties": false
                }
            },
            "required": [
                "color",
                "geometry"
            ],
            "additionalProperties": false
        }
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

----

cancel
~~~~~~

Cancel the task started by the request with the given ID.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "id": {
                "title": "RequestId",
                "description": "ID of the request to cancel",
                "oneOf": [
                    {
                        "type": "null"
                    },
                    {
                        "type": "integer"
                    },
                    {
                        "type": "string"
                    }
                ]
            }
        },
        "required": [
            "id"
        ],
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

clear-clip-planes
~~~~~~~~~~~~~~~~~

Old clear for clipping geometries, use 'clear-clipping-geometries' instead.

.. attention::

    This entrypoint is DEPRECATED, it will be removed or renamed in the next
    major release.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

clear-clipping-geometries
~~~~~~~~~~~~~~~~~~~~~~~~~

Clear all clipping geometries in the scene.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

clear-lights
~~~~~~~~~~~~

Clear all lights in the scene.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

clear-models
~~~~~~~~~~~~

Clear all models in the scene.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

clear-renderables
~~~~~~~~~~~~~~~~~

Clear all renderable models in the scene.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

color-model
~~~~~~~~~~~

Applies the specified color method to the model with the given color input.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "id": {
                "description": "ID of the model to color",
                "type": "integer",
                "minimum": 0
            },
            "method": {
                "description": "Coloring method",
                "type": "string"
            },
            "values": {
                "description": "Coloring parameters",
                "type": "object",
                "additionalProperties": {
                    "type": "array",
                    "items": {
                        "type": "number"
                    },
                    "minItems": 4,
                    "maxItems": 4
                }
            }
        },
        "required": [
            "id",
            "method",
            "values"
        ],
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

enable-simulation
~~~~~~~~~~~~~~~~~

A switch to enable or disable simulation on a model.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "enabled": {
                "description": "Enable simulation if true",
                "type": "boolean"
            },
            "model_id": {
                "description": "ID of the model to color",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "enabled",
            "model_id"
        ],
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

export-gbuffers
~~~~~~~~~~~~~~~

Renders and returns (or saves to disk) the Framebuffer G-Buffers.

This entrypoint is asynchronous, it means that it can take a long time and send
progress notifications.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "camera": {
                "title": "EngineObjectData",
                "description": "Camera definition",
                "type": "object",
                "properties": {
                    "name": {
                        "description": "Object type name",
                        "type": "string"
                    },
                    "params": {
                        "description": "Object parameters"
                    }
                },
                "additionalProperties": false
            },
            "camera_near_clip": {
                "description": "Camera near clipping distance",
                "type": "number"
            },
            "camera_view": {
                "title": "View",
                "description": "Camera view",
                "type": "object",
                "properties": {
                    "position": {
                        "description": "Camera position XYZ",
                        "type": "array",
                        "items": {
                            "type": "number"
                        },
                        "minItems": 3,
                        "maxItems": 3
                    },
                    "target": {
                        "description": "Camera target XYZ",
                        "type": "array",
                        "items": {
                            "type": "number"
                        },
                        "minItems": 3,
                        "maxItems": 3
                    },
                    "up": {
                        "description": "Camera up vector XYZ",
                        "type": "array",
                        "items": {
                            "type": "number"
                        },
                        "minItems": 3,
                        "maxItems": 3
                    }
                },
                "required": [
                    "position",
                    "target",
                    "up"
                ],
                "additionalProperties": false
            },
            "channels": {
                "description": "Framebuffer channels to export",
                "type": "array",
                "items": {
                    "type": "string",
                    "enum": [
                        "color",
                        "depth",
                        "albedo",
                        "normal"
                    ]
                }
            },
            "file_path": {
                "description": "Path to save the buffer as EXR, encoded data is returned if unset",
                "type": "string"
            },
            "renderer": {
                "title": "EngineObjectData",
                "description": "Renderer",
                "type": "object",
                "properties": {
                    "name": {
                        "description": "Object type name",
                        "type": "string"
                    },
                    "params": {
                        "description": "Object parameters"
                    }
                },
                "additionalProperties": false
            },
            "resolution": {
                "description": "Image resolution",
                "type": "array",
                "items": {
                    "type": "integer",
                    "minimum": 0
                },
                "minItems": 2,
                "maxItems": 2
            },
            "simulation_frame": {
                "description": "Simulation frame to render",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "channels"
        ],
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

get-application-parameters
~~~~~~~~~~~~~~~~~~~~~~~~~~

Get the current state of the application parameters.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "plugins": {
                "description": "Plugins loaded when the application was started",
                "type": "array",
                "readOnly": true,
                "items": {
                    "type": "string"
                }
            },
            "viewport": {
                "description": "Framebuffer resolution in pixels",
                "type": "array",
                "items": {
                    "type": "integer",
                    "minimum": 0
                },
                "minItems": 2,
                "maxItems": 2
            }
        },
        "required": [
            "plugins"
        ],
        "additionalProperties": false
    }

----

get-camera-near-clip
~~~~~~~~~~~~~~~~~~~~

Retreive the current camera clipping distance.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "distance": {
                "description": "Camera near clipping distance",
                "type": "number"
            }
        },
        "required": [
            "distance"
        ],
        "additionalProperties": false
    }

----

get-camera-orthographic
~~~~~~~~~~~~~~~~~~~~~~~

Returns the current camera as orthographic.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "height": {
                "description": "Height of the projection plane",
                "type": "number"
            }
        },
        "required": [
            "height"
        ],
        "additionalProperties": false
    }

----

get-camera-perspective
~~~~~~~~~~~~~~~~~~~~~~

Returns the current camera as perspective.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "aperture_radius": {
                "description": "Lens aperture radius",
                "type": "number"
            },
            "focus_distance": {
                "description": "Camera focus distance",
                "type": "number"
            },
            "fovy": {
                "description": "Vertical field of view",
                "type": "number"
            }
        },
        "additionalProperties": false
    }

----

get-camera-type
~~~~~~~~~~~~~~~

Returns the type of the current camera.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

.. jsonschema::

    {
        "type": "string"
    }

----

get-camera-view
~~~~~~~~~~~~~~~

Returns the camera view settings.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "position": {
                "description": "Camera position XYZ",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "target": {
                "description": "Camera target XYZ",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "up": {
                "description": "Camera up vector XYZ",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            }
        },
        "required": [
            "position",
            "target",
            "up"
        ],
        "additionalProperties": false
    }

----

get-color-methods
~~~~~~~~~~~~~~~~~

Returns a list of available coloring methods for the model.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "id": {
                "description": "Model ID",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "id"
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

get-color-ramp
~~~~~~~~~~~~~~

Get the color ramp of the given model.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "id": {
                "description": "Model ID",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "id"
        ],
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "colors": {
                "description": "RGBA colors",
                "type": "array",
                "items": {
                    "type": "array",
                    "items": {
                        "type": "number"
                    },
                    "minItems": 4,
                    "maxItems": 4
                },
                "maxItems": 256
            },
            "range": {
                "description": "Value range",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 2,
                "maxItems": 2
            }
        },
        "additionalProperties": false
    }

----

get-color-values
~~~~~~~~~~~~~~~~

Returns a list of input variables for a given model and color method.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "id": {
                "description": "ID of the model that will be colored",
                "type": "integer",
                "minimum": 0
            },
            "method": {
                "description": "Coloring method which values will be returned",
                "type": "string"
            }
        },
        "required": [
            "id",
            "method"
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

get-loaders
~~~~~~~~~~~

Retreive the description of all available loaders.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

.. jsonschema::

    {
        "type": "array",
        "items": {
            "title": "Loader",
            "type": "object",
            "properties": {
                "binary": {
                    "description": "True if loader supports loading binary data",
                    "type": "boolean",
                    "readOnly": true
                },
                "extensions": {
                    "description": "Supported file formats / extensions",
                    "type": "array",
                    "readOnly": true,
                    "items": {
                        "type": "string"
                    }
                },
                "input_parameters_schema": {
                    "title": "JsonSchema",
                    "description": "Loader params schema",
                    "type": "object",
                    "readOnly": true
                },
                "name": {
                    "description": "Loader name",
                    "type": "string",
                    "readOnly": true
                },
                "plugin": {
                    "description": "Plugin required to use the loader",
                    "type": "string",
                    "readOnly": true
                }
            },
            "required": [
                "binary",
                "extensions",
                "input_parameters_schema",
                "name",
                "plugin"
            ],
            "additionalProperties": false
        }
    }

----

get-material-carpaint
~~~~~~~~~~~~~~~~~~~~~

Returns the material of the given model as a car paint material, if possible.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "id": {
                "description": "Model ID",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "id"
        ],
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "flake_density": {
                "description": "Metal flake density",
                "type": "number",
                "minimum": 0,
                "maximum": 1
            }
        },
        "additionalProperties": false
    }

----

get-material-emissive
~~~~~~~~~~~~~~~~~~~~~

Returns the material of the given model as a emissive material, if possible.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "id": {
                "description": "Model ID",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "id"
        ],
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "color": {
                "description": "Emission color",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "intensity": {
                "description": "Emission intensity",
                "type": "number",
                "minimum": 0
            }
        },
        "additionalProperties": false
    }

----

get-material-ghost
~~~~~~~~~~~~~~~~~~

Returns the material of the given model as a ghost material, if possible.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "id": {
                "description": "Model ID",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "id"
        ],
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "object",
        "additionalProperties": false
    }

----

get-material-glass
~~~~~~~~~~~~~~~~~~

Returns the material of the given model as a glass material, if possible.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "id": {
                "description": "Model ID",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "id"
        ],
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "index_of_refraction": {
                "description": "Index of refraction of the glass",
                "type": "number"
            }
        },
        "additionalProperties": false
    }

----

get-material-matte
~~~~~~~~~~~~~~~~~~

Returns the material of the given model as a matte material, if possible.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "id": {
                "description": "Model ID",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "id"
        ],
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "opacity": {
                "description": "Surface opacity",
                "type": "number",
                "minimum": 0,
                "maximum": 1
            }
        },
        "additionalProperties": false
    }

----

get-material-metal
~~~~~~~~~~~~~~~~~~

Returns the material of the given model as a metal material, if possible.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "id": {
                "description": "Model ID",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "id"
        ],
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "roughness": {
                "description": "Surface roughness",
                "type": "number",
                "minimum": 0.01,
                "maximum": 1
            }
        },
        "additionalProperties": false
    }

----

get-material-phong
~~~~~~~~~~~~~~~~~~

Returns the material of the given model as a phong material, if possible.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "id": {
                "description": "Model ID",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "id"
        ],
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "opacity": {
                "description": "Surface opacity",
                "type": "number",
                "minimum": 0,
                "maximum": 1
            }
        },
        "additionalProperties": false
    }

----

get-material-plastic
~~~~~~~~~~~~~~~~~~~~

Returns the material of the given model as a plastic material, if possible.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "id": {
                "description": "Model ID",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "id"
        ],
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "opacity": {
                "description": "Surface opacity",
                "type": "number",
                "minimum": 0,
                "maximum": 1
            }
        },
        "additionalProperties": false
    }

----

get-material-principled
~~~~~~~~~~~~~~~~~~~~~~~

Returns the material of the given model as a principled material, if possible.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "id": {
                "description": "Model ID",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "id"
        ],
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "anisotropy": {
                "description": "Specular anisotropy reflection weight (Specular highlights depends on surface type/shape)",
                "type": "number",
                "minimum": 0,
                "maximum": 1
            },
            "anisotropy_rotation": {
                "description": "Rotation of the specular anisotropy reflection effect",
                "type": "number",
                "minimum": 0,
                "maximum": 1
            },
            "back_light": {
                "description": "For thin objects, weight of reflection and transmission (1 = 50/50, 2 = only transmission)",
                "type": "number",
                "minimum": 0,
                "maximum": 2
            },
            "coat": {
                "description": "Clear coat weight (thin lacquered/glossy layer on top of the surface)",
                "type": "number",
                "minimum": 0,
                "maximum": 1
            },
            "coat_color": {
                "description": "Clear coat color",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "coat_ior": {
                "description": "Clear coat index of refraction",
                "type": "number"
            },
            "coat_roughness": {
                "description": "Clear coat diffuse/specular reflection roughness",
                "type": "number"
            },
            "coat_thickness": {
                "description": "Clear coat thickness",
                "type": "number"
            },
            "diffuse": {
                "description": "Diffuse reflection weight",
                "type": "number",
                "minimum": 0,
                "maximum": 1
            },
            "edge_color": {
                "description": "Edge tint for metallic surfaces",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "ior": {
                "description": "Dielectric index of refraction",
                "type": "number",
                "minimum": 1
            },
            "metallic": {
                "description": "Alpha parameter between dielectric and metallic",
                "type": "number",
                "minimum": 0,
                "maximum": 1
            },
            "roughness": {
                "description": "Diffuse and specular reflection roughness",
                "type": "number"
            },
            "sheen": {
                "description": "Sheen effect weight (fabric-like effect such as satin or velvet)",
                "type": "number"
            },
            "sheen_color": {
                "description": "Sheen color",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "sheen_roughness": {
                "description": "Sheen diffuse/specular reflection roughness",
                "type": "number"
            },
            "sheen_tint": {
                "description": "Strenght of sheen color (0 = white, 1 = sheen color)",
                "type": "number"
            },
            "specular": {
                "description": "Specular reflection/transmission weight",
                "type": "number",
                "minimum": 0,
                "maximum": 1
            },
            "thickness": {
                "description": "Thickness of the object if thin = true",
                "type": "number"
            },
            "thin": {
                "description": "Specified wether the object is solid or thin (hollow)",
                "type": "boolean"
            },
            "transmission": {
                "description": "Specular transmission weight",
                "type": "number",
                "minimum": 0,
                "maximum": 1
            },
            "transmission_color": {
                "description": "Transmission attenuation color",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "transmission_depth": {
                "description": "Distance from surface at which the color will equal transmission color",
                "type": "number"
            }
        },
        "additionalProperties": false
    }

----

get-material-type
~~~~~~~~~~~~~~~~~

Returns the type of the material of a given model, if any.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "id": {
                "description": "Model ID",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "id"
        ],
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "string"
    }

----

get-model
~~~~~~~~~

Get all the information of the given model.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "id": {
                "description": "Model ID",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "id"
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

----

get-renderer-interactive
~~~~~~~~~~~~~~~~~~~~~~~~

Returns the current renderer as interactive renderer, if possible.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "ao_samples": {
                "description": "Ambient occlusion samples",
                "type": "integer",
                "minimum": 0
            },
            "background_color": {
                "description": "Background color",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 4,
                "maxItems": 4
            },
            "enable_shadows": {
                "description": "Enable casted shadows when rendering",
                "type": "boolean"
            },
            "max_ray_bounces": {
                "description": "Maximum ray bounces",
                "type": "integer",
                "minimum": 0
            },
            "samples_per_pixel": {
                "description": "Ray samples per pixel",
                "type": "integer",
                "minimum": 1
            }
        },
        "additionalProperties": false
    }

----

get-renderer-production
~~~~~~~~~~~~~~~~~~~~~~~

Returns the current renderer as production renderer, if possible.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "background_color": {
                "description": "Background color",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 4,
                "maxItems": 4
            },
            "max_ray_bounces": {
                "description": "Maximum ray bounces",
                "type": "integer",
                "minimum": 0
            },
            "samples_per_pixel": {
                "description": "Ray samples per pixel",
                "type": "integer",
                "minimum": 1
            }
        },
        "additionalProperties": false
    }

----

get-renderer-type
~~~~~~~~~~~~~~~~~

Returns the type of the renderer currently being used.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

.. jsonschema::

    {
        "type": "string"
    }

----

get-scene
~~~~~~~~~

Get the current state of the scene.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "bounds": {
                "title": "Bounds",
                "description": "Scene bounds",
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
            "models": {
                "description": "Scene models",
                "type": "array",
                "readOnly": true,
                "items": {
                    "title": "ModelInstance",
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
            }
        },
        "required": [
            "bounds",
            "models"
        ],
        "additionalProperties": false
    }

----

get-simulation-parameters
~~~~~~~~~~~~~~~~~~~~~~~~~

Get the current state of the simulation parameters.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "current": {
                "description": "Current simulation frame index",
                "type": "integer",
                "minimum": 0
            },
            "dt": {
                "description": "Delta time between two frames",
                "type": "number",
                "readOnly": true
            },
            "end_frame": {
                "description": "Final simulation frame index",
                "type": "integer",
                "readOnly": true,
                "minimum": 0
            },
            "start_frame": {
                "description": "Initial simulation frame index",
                "type": "integer",
                "readOnly": true,
                "minimum": 0
            },
            "unit": {
                "description": "Time unit",
                "type": "string",
                "readOnly": true
            }
        },
        "required": [
            "dt",
            "end_frame",
            "start_frame",
            "unit"
        ],
        "additionalProperties": false
    }

----

get-version
~~~~~~~~~~~

Get Brayns instance version.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "major": {
                "description": "Major version",
                "type": "integer",
                "readOnly": true
            },
            "minor": {
                "description": "Minor version",
                "type": "integer",
                "readOnly": true
            },
            "patch": {
                "description": "Patch version",
                "type": "integer",
                "readOnly": true
            },
            "pre_release": {
                "description": "Pre-release (empty for production)",
                "type": "string",
                "readOnly": true
            }
        },
        "required": [
            "major",
            "minor",
            "patch",
            "pre_release"
        ],
        "additionalProperties": false
    }

----

inspect
~~~~~~~

Inspect the scene at x-y position.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "position": {
                "description": "Normalized screen position XY",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 2,
                "maxItems": 2
            }
        },
        "required": [
            "position"
        ],
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "hit": {
                "description": "True if a model was at given position, otherwise the rest is invalid",
                "type": "boolean",
                "readOnly": true
            },
            "metadata": {
                "description": "Extra attributes depending on the type of model hitted",
                "readOnly": true
            },
            "model_id": {
                "description": "ID of the model that was hit at given position",
                "type": "integer",
                "readOnly": true,
                "minimum": 0
            },
            "position": {
                "description": "World position XYZ where the model was hit",
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
            "hit",
            "metadata",
            "model_id",
            "position"
        ],
        "additionalProperties": false
    }

----

instantiate-model
~~~~~~~~~~~~~~~~~

Creates new instances of the given model. The underneath data is shared across all instances.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "model_id": {
                "description": "ID of the model to instantiate",
                "type": "integer",
                "minimum": 0
            },
            "transforms": {
                "description": "Transformations to apply to the new instances",
                "type": "array",
                "items": {
                    "title": "Transform",
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
            }
        },
        "required": [
            "model_id",
            "transforms"
        ],
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "array",
        "items": {
            "title": "ModelInstance",
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
    }

----

quit
~~~~

Quit the application.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

registry
~~~~~~~~

Retreive the names of all registered entrypoints.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

.. jsonschema::

    {
        "type": "array",
        "items": {
            "type": "string"
        }
    }

----

remove-model
~~~~~~~~~~~~

Remove the model(s) from the ID list from the scene.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "ids": {
                "description": "Model ID list",
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

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

render-image
~~~~~~~~~~~~

Render an image of the current context and retreive it according to given params.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "accumulate": {
                "description": "Render all images until max accumulation",
                "type": "boolean",
                "default": false
            },
            "force": {
                "description": "Send image even if nothing new was rendered",
                "type": "boolean",
                "default": false
            },
            "format": {
                "description": "Encoding of returned image data (jpg or png)",
                "type": "string",
                "default": "jpg"
            },
            "jpeg_quality": {
                "description": "Quality if using JPEG encoding",
                "type": "integer",
                "default": 100,
                "minimum": 0,
                "maximum": 100
            },
            "render": {
                "description": "Disable render if set to false (download only)",
                "type": "boolean",
                "default": true
            },
            "send": {
                "description": "Send image once rendered",
                "type": "boolean",
                "default": true
            }
        },
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "accumulation": {
                "description": "Current frame accumulation",
                "type": "integer",
                "readOnly": true,
                "minimum": 0
            },
            "max_accumulation": {
                "description": "Maximum frame accumulation",
                "type": "integer",
                "readOnly": true,
                "minimum": 0
            }
        },
        "required": [
            "accumulation",
            "max_accumulation"
        ],
        "additionalProperties": false
    }

----

schema
~~~~~~

Get the JSON schema of the given entrypoint.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "endpoint": {
                "description": "Name of the endpoint",
                "type": "string"
            }
        },
        "required": [
            "endpoint"
        ],
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "async": {
                "description": "Check if the entrypoint is asynchronous (send progress and can be cancelled)",
                "type": "boolean",
                "readOnly": true
            },
            "deprecated": {
                "description": "If true, the entrypoint will be removed / renamed in the next release",
                "type": "boolean",
                "readOnly": true
            },
            "description": {
                "description": "Description of the entrypoint",
                "type": "string",
                "readOnly": true
            },
            "params": {
                "title": "JsonSchema",
                "description": "Input schema",
                "type": "object",
                "readOnly": true
            },
            "plugin": {
                "description": "Name of the plugin that loads the entrypoint",
                "type": "string",
                "readOnly": true
            },
            "returns": {
                "title": "JsonSchema",
                "description": "Output schema",
                "type": "object",
                "readOnly": true
            },
            "title": {
                "description": "Name of the entrypoint (method)",
                "type": "string",
                "readOnly": true
            }
        },
        "required": [
            "async",
            "deprecated",
            "description",
            "plugin",
            "title"
        ],
        "additionalProperties": false
    }

----

set-application-parameters
~~~~~~~~~~~~~~~~~~~~~~~~~~

Set the current state of the application parameters.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "plugins": {
                "description": "Plugins loaded when the application was started",
                "type": "array",
                "readOnly": true,
                "items": {
                    "type": "string"
                }
            },
            "viewport": {
                "description": "Framebuffer resolution in pixels",
                "type": "array",
                "items": {
                    "type": "integer",
                    "minimum": 0
                },
                "minItems": 2,
                "maxItems": 2
            }
        },
        "required": [
            "plugins"
        ],
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

set-camera-near-clip
~~~~~~~~~~~~~~~~~~~~

Update the camera near clipping distance.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "distance": {
                "description": "Camera near clipping distance",
                "type": "number"
            }
        },
        "required": [
            "distance"
        ],
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

set-camera-orthographic
~~~~~~~~~~~~~~~~~~~~~~~

Sets the current camera to an orthographic one, with the specified parameters.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "height": {
                "description": "Height of the projection plane",
                "type": "number"
            }
        },
        "required": [
            "height"
        ],
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

set-camera-perspective
~~~~~~~~~~~~~~~~~~~~~~

Sets the current camera to a perspective one, with the specified parameters.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "aperture_radius": {
                "description": "Lens aperture radius",
                "type": "number"
            },
            "focus_distance": {
                "description": "Camera focus distance",
                "type": "number"
            },
            "fovy": {
                "description": "Vertical field of view",
                "type": "number"
            }
        },
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

set-camera-view
~~~~~~~~~~~~~~~

Sets the camera view settings.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "position": {
                "description": "Camera position XYZ",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "target": {
                "description": "Camera target XYZ",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "up": {
                "description": "Camera up vector XYZ",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            }
        },
        "required": [
            "position",
            "target",
            "up"
        ],
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

set-color-ramp
~~~~~~~~~~~~~~

Set the color ramp of the given model.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "color_ramp": {
                "title": "ColorRamp",
                "description": "Color ramp",
                "type": "object",
                "properties": {
                    "colors": {
                        "description": "RGBA colors",
                        "type": "array",
                        "items": {
                            "type": "array",
                            "items": {
                                "type": "number"
                            },
                            "minItems": 4,
                            "maxItems": 4
                        },
                        "maxItems": 256
                    },
                    "range": {
                        "description": "Value range",
                        "type": "array",
                        "items": {
                            "type": "number"
                        },
                        "minItems": 2,
                        "maxItems": 2
                    }
                },
                "additionalProperties": false
            },
            "id": {
                "description": "Model ID",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "color_ramp",
            "id"
        ],
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

set-framebuffer-progressive
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Stablishes a progressive-resolution frame rendering on the engine.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "scale": {
                "description": "Frame size reduction factor",
                "type": "integer",
                "default": 4,
                "minimum": 1
            }
        },
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

set-framebuffer-static
~~~~~~~~~~~~~~~~~~~~~~

Stablishes a static frame rendering on the engine.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

set-material-carpaint
~~~~~~~~~~~~~~~~~~~~~

Updates the material of the given model to a car paint material. This material is only usable with the production renderer.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "material": {
                "title": "CarPaint",
                "description": "Material parameters",
                "type": "object",
                "properties": {
                    "flake_density": {
                        "description": "Metal flake density",
                        "type": "number",
                        "minimum": 0,
                        "maximum": 1
                    }
                },
                "additionalProperties": false
            },
            "model_id": {
                "description": "ID of the model to apply the material",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "material",
            "model_id"
        ],
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

set-material-emissive
~~~~~~~~~~~~~~~~~~~~~

Updates the material of the given model to an emisive material. This material is only usable with the production renderer.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "material": {
                "title": "Emissive",
                "description": "Material parameters",
                "type": "object",
                "properties": {
                    "color": {
                        "description": "Emission color",
                        "type": "array",
                        "items": {
                            "type": "number"
                        },
                        "minItems": 3,
                        "maxItems": 3
                    },
                    "intensity": {
                        "description": "Emission intensity",
                        "type": "number",
                        "minimum": 0
                    }
                },
                "additionalProperties": false
            },
            "model_id": {
                "description": "ID of the model to apply the material",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "material",
            "model_id"
        ],
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

set-material-ghost
~~~~~~~~~~~~~~~~~~

Updates the material of the given model to a ghost material. The ghost effect is only visible with the interactive renderer..

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "material": {
                "title": "Ghost",
                "description": "Material parameters",
                "type": "object",
                "additionalProperties": false
            },
            "model_id": {
                "description": "ID of the model to apply the material",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "material",
            "model_id"
        ],
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

set-material-glass
~~~~~~~~~~~~~~~~~~

Updates the material of the given model to a glass material. This material is only usable with the production renderer.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "material": {
                "title": "Glass",
                "description": "Material parameters",
                "type": "object",
                "properties": {
                    "index_of_refraction": {
                        "description": "Index of refraction of the glass",
                        "type": "number"
                    }
                },
                "additionalProperties": false
            },
            "model_id": {
                "description": "ID of the model to apply the material",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "material",
            "model_id"
        ],
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

set-material-matte
~~~~~~~~~~~~~~~~~~

Updates the material of the given model to a matte material. This material is only usable with the production renderer.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "material": {
                "title": "Matte",
                "description": "Material parameters",
                "type": "object",
                "properties": {
                    "opacity": {
                        "description": "Surface opacity",
                        "type": "number",
                        "minimum": 0,
                        "maximum": 1
                    }
                },
                "additionalProperties": false
            },
            "model_id": {
                "description": "ID of the model to apply the material",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "material",
            "model_id"
        ],
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

set-material-metal
~~~~~~~~~~~~~~~~~~

Updates the material of the given model to a metal material. This material is only usable with the production renderer.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "material": {
                "title": "Metal",
                "description": "Material parameters",
                "type": "object",
                "properties": {
                    "roughness": {
                        "description": "Surface roughness",
                        "type": "number",
                        "minimum": 0.01,
                        "maximum": 1
                    }
                },
                "additionalProperties": false
            },
            "model_id": {
                "description": "ID of the model to apply the material",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "material",
            "model_id"
        ],
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

set-material-phong
~~~~~~~~~~~~~~~~~~

Updates the material of the given model to the phong material. This material works with all renderers. It has a matte appearance..

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "material": {
                "title": "Phong",
                "description": "Material parameters",
                "type": "object",
                "properties": {
                    "opacity": {
                        "description": "Surface opacity",
                        "type": "number",
                        "minimum": 0,
                        "maximum": 1
                    }
                },
                "additionalProperties": false
            },
            "model_id": {
                "description": "ID of the model to apply the material",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "material",
            "model_id"
        ],
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

set-material-plastic
~~~~~~~~~~~~~~~~~~~~

Updates the material of the given model to a plastic material. This material is only usable with the production renderer.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "material": {
                "title": "Plastic",
                "description": "Material parameters",
                "type": "object",
                "properties": {
                    "opacity": {
                        "description": "Surface opacity",
                        "type": "number",
                        "minimum": 0,
                        "maximum": 1
                    }
                },
                "additionalProperties": false
            },
            "model_id": {
                "description": "ID of the model to apply the material",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "material",
            "model_id"
        ],
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

set-material-principled
~~~~~~~~~~~~~~~~~~~~~~~

Updates the material of the given model to a principled material. This material is only usable with the production renderer.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "material": {
                "title": "Principled",
                "description": "Material parameters",
                "type": "object",
                "properties": {
                    "anisotropy": {
                        "description": "Specular anisotropy reflection weight (Specular highlights depends on surface type/shape)",
                        "type": "number",
                        "minimum": 0,
                        "maximum": 1
                    },
                    "anisotropy_rotation": {
                        "description": "Rotation of the specular anisotropy reflection effect",
                        "type": "number",
                        "minimum": 0,
                        "maximum": 1
                    },
                    "back_light": {
                        "description": "For thin objects, weight of reflection and transmission (1 = 50/50, 2 = only transmission)",
                        "type": "number",
                        "minimum": 0,
                        "maximum": 2
                    },
                    "coat": {
                        "description": "Clear coat weight (thin lacquered/glossy layer on top of the surface)",
                        "type": "number",
                        "minimum": 0,
                        "maximum": 1
                    },
                    "coat_color": {
                        "description": "Clear coat color",
                        "type": "array",
                        "items": {
                            "type": "number"
                        },
                        "minItems": 3,
                        "maxItems": 3
                    },
                    "coat_ior": {
                        "description": "Clear coat index of refraction",
                        "type": "number"
                    },
                    "coat_roughness": {
                        "description": "Clear coat diffuse/specular reflection roughness",
                        "type": "number"
                    },
                    "coat_thickness": {
                        "description": "Clear coat thickness",
                        "type": "number"
                    },
                    "diffuse": {
                        "description": "Diffuse reflection weight",
                        "type": "number",
                        "minimum": 0,
                        "maximum": 1
                    },
                    "edge_color": {
                        "description": "Edge tint for metallic surfaces",
                        "type": "array",
                        "items": {
                            "type": "number"
                        },
                        "minItems": 3,
                        "maxItems": 3
                    },
                    "ior": {
                        "description": "Dielectric index of refraction",
                        "type": "number",
                        "minimum": 1
                    },
                    "metallic": {
                        "description": "Alpha parameter between dielectric and metallic",
                        "type": "number",
                        "minimum": 0,
                        "maximum": 1
                    },
                    "roughness": {
                        "description": "Diffuse and specular reflection roughness",
                        "type": "number"
                    },
                    "sheen": {
                        "description": "Sheen effect weight (fabric-like effect such as satin or velvet)",
                        "type": "number"
                    },
                    "sheen_color": {
                        "description": "Sheen color",
                        "type": "array",
                        "items": {
                            "type": "number"
                        },
                        "minItems": 3,
                        "maxItems": 3
                    },
                    "sheen_roughness": {
                        "description": "Sheen diffuse/specular reflection roughness",
                        "type": "number"
                    },
                    "sheen_tint": {
                        "description": "Strenght of sheen color (0 = white, 1 = sheen color)",
                        "type": "number"
                    },
                    "specular": {
                        "description": "Specular reflection/transmission weight",
                        "type": "number",
                        "minimum": 0,
                        "maximum": 1
                    },
                    "thickness": {
                        "description": "Thickness of the object if thin = true",
                        "type": "number"
                    },
                    "thin": {
                        "description": "Specified wether the object is solid or thin (hollow)",
                        "type": "boolean"
                    },
                    "transmission": {
                        "description": "Specular transmission weight",
                        "type": "number",
                        "minimum": 0,
                        "maximum": 1
                    },
                    "transmission_color": {
                        "description": "Transmission attenuation color",
                        "type": "array",
                        "items": {
                            "type": "number"
                        },
                        "minItems": 3,
                        "maxItems": 3
                    },
                    "transmission_depth": {
                        "description": "Distance from surface at which the color will equal transmission color",
                        "type": "number"
                    }
                },
                "additionalProperties": false
            },
            "model_id": {
                "description": "ID of the model to apply the material",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "material",
            "model_id"
        ],
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

set-renderer-interactive
~~~~~~~~~~~~~~~~~~~~~~~~

Sets the system renderer to the interactive one.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "ao_samples": {
                "description": "Ambient occlusion samples",
                "type": "integer",
                "minimum": 0
            },
            "background_color": {
                "description": "Background color",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 4,
                "maxItems": 4
            },
            "enable_shadows": {
                "description": "Enable casted shadows when rendering",
                "type": "boolean"
            },
            "max_ray_bounces": {
                "description": "Maximum ray bounces",
                "type": "integer",
                "minimum": 0
            },
            "samples_per_pixel": {
                "description": "Ray samples per pixel",
                "type": "integer",
                "minimum": 1
            }
        },
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

set-renderer-production
~~~~~~~~~~~~~~~~~~~~~~~

Sets the system renderer to the production one.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "background_color": {
                "description": "Background color",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 4,
                "maxItems": 4
            },
            "max_ray_bounces": {
                "description": "Maximum ray bounces",
                "type": "integer",
                "minimum": 0
            },
            "samples_per_pixel": {
                "description": "Ray samples per pixel",
                "type": "integer",
                "minimum": 1
            }
        },
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

set-simulation-parameters
~~~~~~~~~~~~~~~~~~~~~~~~~

Set the current state of the simulation parameters.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "current": {
                "description": "Current simulation frame index",
                "type": "integer",
                "minimum": 0
            },
            "dt": {
                "description": "Delta time between two frames",
                "type": "number",
                "readOnly": true
            },
            "end_frame": {
                "description": "Final simulation frame index",
                "type": "integer",
                "readOnly": true,
                "minimum": 0
            },
            "start_frame": {
                "description": "Initial simulation frame index",
                "type": "integer",
                "readOnly": true,
                "minimum": 0
            },
            "unit": {
                "description": "Time unit",
                "type": "string",
                "readOnly": true
            }
        },
        "required": [
            "dt",
            "end_frame",
            "start_frame",
            "unit"
        ],
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

snapshot
~~~~~~~~

Take a snapshot with given parameters.

This entrypoint is asynchronous, it means that it can take a long time and send
progress notifications.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "camera": {
                "title": "EngineObjectData",
                "description": "Camera definition",
                "type": "object",
                "properties": {
                    "name": {
                        "description": "Object type name",
                        "type": "string"
                    },
                    "params": {
                        "description": "Object parameters"
                    }
                },
                "additionalProperties": false
            },
            "camera_near_clip": {
                "description": "Camera near clipping distance",
                "type": "number"
            },
            "camera_view": {
                "title": "View",
                "description": "Camera view",
                "type": "object",
                "properties": {
                    "position": {
                        "description": "Camera position XYZ",
                        "type": "array",
                        "items": {
                            "type": "number"
                        },
                        "minItems": 3,
                        "maxItems": 3
                    },
                    "target": {
                        "description": "Camera target XYZ",
                        "type": "array",
                        "items": {
                            "type": "number"
                        },
                        "minItems": 3,
                        "maxItems": 3
                    },
                    "up": {
                        "description": "Camera up vector XYZ",
                        "type": "array",
                        "items": {
                            "type": "number"
                        },
                        "minItems": 3,
                        "maxItems": 3
                    }
                },
                "required": [
                    "position",
                    "target",
                    "up"
                ],
                "additionalProperties": false
            },
            "file_path": {
                "description": "Path to save image, raw encoded data will be returned if empty",
                "type": "string"
            },
            "image_settings": {
                "title": "ImageSettings",
                "description": "Image settings",
                "type": "object",
                "properties": {
                    "format": {
                        "description": "Image format (jpg or png)",
                        "type": "string",
                        "default": "png"
                    },
                    "quality": {
                        "description": "Image quality (0 = lowest quality, 100 = highest quality",
                        "type": "integer",
                        "default": 100,
                        "minimum": 0
                    },
                    "size": {
                        "description": "Image width and height",
                        "type": "array",
                        "items": {
                            "type": "integer",
                            "minimum": 0
                        },
                        "minItems": 2,
                        "maxItems": 2
                    }
                },
                "additionalProperties": false
            },
            "metadata": {
                "title": "ImageMetadata",
                "description": "Metadata information to embed into the image",
                "type": "object",
                "properties": {
                    "description": {
                        "description": "Image description",
                        "type": "string"
                    },
                    "keywords": {
                        "description": "List of keywords to describe the image contents",
                        "type": "array",
                        "items": {
                            "type": "string"
                        }
                    },
                    "title": {
                        "description": "Image title",
                        "type": "string"
                    },
                    "where_used": {
                        "description": "Event (publication, website, panel, etc.) for what the image was created",
                        "type": "string"
                    }
                },
                "required": [
                    "description",
                    "keywords",
                    "title",
                    "where_used"
                ],
                "additionalProperties": false
            },
            "renderer": {
                "title": "EngineObjectData",
                "description": "Renderer definition",
                "type": "object",
                "properties": {
                    "name": {
                        "description": "Object type name",
                        "type": "string"
                    },
                    "params": {
                        "description": "Object parameters"
                    }
                },
                "additionalProperties": false
            },
            "simulation_frame": {
                "description": "Simulation frame to render",
                "type": "integer",
                "minimum": 0
            }
        },
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "color_buffer": {
                "title": "ColorBufferMessage",
                "description": "Encoded snapshot color buffer",
                "type": "object",
                "readOnly": true,
                "properties": {
                    "offset": {
                        "description": "Data offset in attached binary",
                        "type": "integer",
                        "readOnly": true,
                        "minimum": 0
                    },
                    "size": {
                        "description": "Data size in attached binary",
                        "type": "integer",
                        "readOnly": true,
                        "minimum": 0
                    }
                },
                "required": [
                    "offset",
                    "size"
                ],
                "additionalProperties": false
            }
        },
        "required": [
            "color_buffer"
        ],
        "additionalProperties": false
    }

----

update-model
~~~~~~~~~~~~

Update the model with the given values and return its new state.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "model": {
                "title": "ModelInstance",
                "description": "Model data to update",
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
            },
            "model_id": {
                "description": "Model ID",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "model",
            "model_id"
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

----

upload-model
~~~~~~~~~~~~

Upload a model from binary request data and return model descriptors on success.

This entrypoint is asynchronous, it means that it can take a long time and send
progress notifications.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "loader_name": {
                "description": "Loader name",
                "type": "string"
            },
            "loader_properties": {
                "description": "Loader properties"
            },
            "type": {
                "description": "File extension",
                "type": "string"
            }
        },
        "required": [
            "loader_name",
            "loader_properties",
            "type"
        ],
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "array",
        "items": {
            "title": "ModelInstance",
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
    }
