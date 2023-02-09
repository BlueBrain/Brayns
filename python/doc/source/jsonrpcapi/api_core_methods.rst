.. _apicore-label:

Core API methods
----------------

This page references the entrypoints of the Core plugin.

add-bounded-planes
~~~~~~~~~~~~~~~~~~

Adds a list of axis-aligned bound limited planes.

**Params**:

.. jsonschema::

    {
        "type": "array",
        "items": {
            "title": "GeometryWithColor<BoundedPlane>",
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
                                    "description": "Maximum bound corner (top front right)",
                                    "type": "array",
                                    "items": {
                                        "type": "number"
                                    },
                                    "minItems": 3,
                                    "maxItems": 3
                                },
                                "min": {
                                    "description": "Minimum bound corner (bottom back left)",
                                    "type": "array",
                                    "items": {
                                        "type": "number"
                                    },
                                    "minItems": 3,
                                    "maxItems": 3
                                }
                            },
                            "required": [
                                "min",
                                "max"
                            ],
                            "additionalProperties": false
                        },
                        "coefficients": {
                            "description": "Plane equation coefficients (A, B, C, D from Ax + By + Cz + D = 0)",
                            "type": "array",
                            "items": {
                                "type": "number"
                            },
                            "minItems": 4,
                            "maxItems": 4
                        }
                    },
                    "required": [
                        "coefficients",
                        "bounds"
                    ],
                    "additionalProperties": false
                }
            },
            "required": [
                "geometry",
                "color"
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
            "info": {
                "title": "ModelInfo",
                "description": "Model-specific metadata",
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
                        "description": "Model load information",
                        "type": "object",
                        "readOnly": true,
                        "properties": {
                            "load_parameters": {
                                "description": "Loader configuration"
                            },
                            "loader_name": {
                                "description": "Loader used",
                                "type": "string"
                            },
                            "path": {
                                "description": "File path in case of file load type",
                                "type": "string"
                            },
                            "source": {
                                "description": "Model load source",
                                "type": "string",
                                "enum": [
                                    "from_file",
                                    "from_blob",
                                    "none"
                                ]
                            }
                        },
                        "required": [
                            "source",
                            "path",
                            "loader_name",
                            "load_parameters"
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
            "title": "GeometryWithColor<Box>",
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
                            "description": "Maximum bound corner (top front right)",
                            "type": "array",
                            "items": {
                                "type": "number"
                            },
                            "minItems": 3,
                            "maxItems": 3
                        },
                        "min": {
                            "description": "Minimum bound corner (bottom back left)",
                            "type": "array",
                            "items": {
                                "type": "number"
                            },
                            "minItems": 3,
                            "maxItems": 3
                        }
                    },
                    "required": [
                        "min",
                        "max"
                    ],
                    "additionalProperties": false
                }
            },
            "required": [
                "geometry",
                "color"
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
            "info": {
                "title": "ModelInfo",
                "description": "Model-specific metadata",
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
                        "description": "Model load information",
                        "type": "object",
                        "readOnly": true,
                        "properties": {
                            "load_parameters": {
                                "description": "Loader configuration"
                            },
                            "loader_name": {
                                "description": "Loader used",
                                "type": "string"
                            },
                            "path": {
                                "description": "File path in case of file load type",
                                "type": "string"
                            },
                            "source": {
                                "description": "Model load source",
                                "type": "string",
                                "enum": [
                                    "from_file",
                                    "from_blob",
                                    "none"
                                ]
                            }
                        },
                        "required": [
                            "source",
                            "path",
                            "loader_name",
                            "load_parameters"
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
            "title": "GeometryWithColor<Capsule>",
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
                            "description": "Starting point of the capsule",
                            "type": "array",
                            "items": {
                                "type": "number"
                            },
                            "minItems": 3,
                            "maxItems": 3
                        },
                        "p1": {
                            "description": "Ending point of the capsule",
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
                        "r0",
                        "p1",
                        "r1"
                    ],
                    "additionalProperties": false
                }
            },
            "required": [
                "geometry",
                "color"
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
            "info": {
                "title": "ModelInfo",
                "description": "Model-specific metadata",
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
                        "description": "Model load information",
                        "type": "object",
                        "readOnly": true,
                        "properties": {
                            "load_parameters": {
                                "description": "Loader configuration"
                            },
                            "loader_name": {
                                "description": "Loader used",
                                "type": "string"
                            },
                            "path": {
                                "description": "File path in case of file load type",
                                "type": "string"
                            },
                            "source": {
                                "description": "Model load source",
                                "type": "string",
                                "enum": [
                                    "from_file",
                                    "from_blob",
                                    "none"
                                ]
                            }
                        },
                        "required": [
                            "source",
                            "path",
                            "loader_name",
                            "load_parameters"
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
        "additionalProperties": false
    }

----

add-clip-plane
~~~~~~~~~~~~~~

Old way of adding clip plane, use 'add-clipping-planes' instead.

.. attention::

    This entrypoint is DEPRECATED, it will be removed or renamed in the next
    major release.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "coefficients": {
                "description": "Plane equation coefficients (A, B, C, D from Ax + By + Cz + D = 0)",
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
            "info": {
                "title": "ModelInfo",
                "description": "Model-specific metadata",
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
                        "description": "Model load information",
                        "type": "object",
                        "readOnly": true,
                        "properties": {
                            "load_parameters": {
                                "description": "Loader configuration"
                            },
                            "loader_name": {
                                "description": "Loader used",
                                "type": "string"
                            },
                            "path": {
                                "description": "File path in case of file load type",
                                "type": "string"
                            },
                            "source": {
                                "description": "Model load source",
                                "type": "string",
                                "enum": [
                                    "from_file",
                                    "from_blob",
                                    "none"
                                ]
                            }
                        },
                        "required": [
                            "source",
                            "path",
                            "loader_name",
                            "load_parameters"
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
                                    "description": "Maximum bound corner (top front right)",
                                    "type": "array",
                                    "items": {
                                        "type": "number"
                                    },
                                    "minItems": 3,
                                    "maxItems": 3
                                },
                                "min": {
                                    "description": "Minimum bound corner (bottom back left)",
                                    "type": "array",
                                    "items": {
                                        "type": "number"
                                    },
                                    "minItems": 3,
                                    "maxItems": 3
                                }
                            },
                            "required": [
                                "min",
                                "max"
                            ],
                            "additionalProperties": false
                        },
                        "coefficients": {
                            "description": "Plane equation coefficients (A, B, C, D from Ax + By + Cz + D = 0)",
                            "type": "array",
                            "items": {
                                "type": "number"
                            },
                            "minItems": 4,
                            "maxItems": 4
                        }
                    },
                    "required": [
                        "coefficients",
                        "bounds"
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
            "info": {
                "title": "ModelInfo",
                "description": "Model-specific metadata",
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
                        "description": "Model load information",
                        "type": "object",
                        "readOnly": true,
                        "properties": {
                            "load_parameters": {
                                "description": "Loader configuration"
                            },
                            "loader_name": {
                                "description": "Loader used",
                                "type": "string"
                            },
                            "path": {
                                "description": "File path in case of file load type",
                                "type": "string"
                            },
                            "source": {
                                "description": "Model load source",
                                "type": "string",
                                "enum": [
                                    "from_file",
                                    "from_blob",
                                    "none"
                                ]
                            }
                        },
                        "required": [
                            "source",
                            "path",
                            "loader_name",
                            "load_parameters"
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
                            "description": "Maximum bound corner (top front right)",
                            "type": "array",
                            "items": {
                                "type": "number"
                            },
                            "minItems": 3,
                            "maxItems": 3
                        },
                        "min": {
                            "description": "Minimum bound corner (bottom back left)",
                            "type": "array",
                            "items": {
                                "type": "number"
                            },
                            "minItems": 3,
                            "maxItems": 3
                        }
                    },
                    "required": [
                        "min",
                        "max"
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
            "info": {
                "title": "ModelInfo",
                "description": "Model-specific metadata",
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
                        "description": "Model load information",
                        "type": "object",
                        "readOnly": true,
                        "properties": {
                            "load_parameters": {
                                "description": "Loader configuration"
                            },
                            "loader_name": {
                                "description": "Loader used",
                                "type": "string"
                            },
                            "path": {
                                "description": "File path in case of file load type",
                                "type": "string"
                            },
                            "source": {
                                "description": "Model load source",
                                "type": "string",
                                "enum": [
                                    "from_file",
                                    "from_blob",
                                    "none"
                                ]
                            }
                        },
                        "required": [
                            "source",
                            "path",
                            "loader_name",
                            "load_parameters"
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
                            "description": "Starting point of the capsule",
                            "type": "array",
                            "items": {
                                "type": "number"
                            },
                            "minItems": 3,
                            "maxItems": 3
                        },
                        "p1": {
                            "description": "Ending point of the capsule",
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
                        "r0",
                        "p1",
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
            "info": {
                "title": "ModelInfo",
                "description": "Model-specific metadata",
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
                        "description": "Model load information",
                        "type": "object",
                        "readOnly": true,
                        "properties": {
                            "load_parameters": {
                                "description": "Loader configuration"
                            },
                            "loader_name": {
                                "description": "Loader used",
                                "type": "string"
                            },
                            "path": {
                                "description": "File path in case of file load type",
                                "type": "string"
                            },
                            "source": {
                                "description": "Model load source",
                                "type": "string",
                                "enum": [
                                    "from_file",
                                    "from_blob",
                                    "none"
                                ]
                            }
                        },
                        "required": [
                            "source",
                            "path",
                            "loader_name",
                            "load_parameters"
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
                            "description": "Plane equation coefficients (A, B, C, D from Ax + By + Cz + D = 0)",
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
            "info": {
                "title": "ModelInfo",
                "description": "Model-specific metadata",
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
                        "description": "Model load information",
                        "type": "object",
                        "readOnly": true,
                        "properties": {
                            "load_parameters": {
                                "description": "Loader configuration"
                            },
                            "loader_name": {
                                "description": "Loader used",
                                "type": "string"
                            },
                            "path": {
                                "description": "File path in case of file load type",
                                "type": "string"
                            },
                            "source": {
                                "description": "Model load source",
                                "type": "string",
                                "enum": [
                                    "from_file",
                                    "from_blob",
                                    "none"
                                ]
                            }
                        },
                        "required": [
                            "source",
                            "path",
                            "loader_name",
                            "load_parameters"
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
                            "description": "Sphere center point",
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
            "info": {
                "title": "ModelInfo",
                "description": "Model-specific metadata",
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
                        "description": "Model load information",
                        "type": "object",
                        "readOnly": true,
                        "properties": {
                            "load_parameters": {
                                "description": "Loader configuration"
                            },
                            "loader_name": {
                                "description": "Loader used",
                                "type": "string"
                            },
                            "path": {
                                "description": "File path in case of file load type",
                                "type": "string"
                            },
                            "source": {
                                "description": "Model load source",
                                "type": "string",
                                "enum": [
                                    "from_file",
                                    "from_blob",
                                    "none"
                                ]
                            }
                        },
                        "required": [
                            "source",
                            "path",
                            "loader_name",
                            "load_parameters"
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
                "description": "Light color (Normalized RGB)",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "intensity": {
                "description": "Light intensity",
                "type": "number",
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
            "info": {
                "title": "ModelInfo",
                "description": "Model-specific metadata",
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
                        "description": "Model load information",
                        "type": "object",
                        "readOnly": true,
                        "properties": {
                            "load_parameters": {
                                "description": "Loader configuration"
                            },
                            "loader_name": {
                                "description": "Loader used",
                                "type": "string"
                            },
                            "path": {
                                "description": "File path in case of file load type",
                                "type": "string"
                            },
                            "source": {
                                "description": "Model load source",
                                "type": "string",
                                "enum": [
                                    "from_file",
                                    "from_blob",
                                    "none"
                                ]
                            }
                        },
                        "required": [
                            "source",
                            "path",
                            "loader_name",
                            "load_parameters"
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
                "description": "Light color (Normalized RGB)",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "direction": {
                "description": "Light direction vector",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "intensity": {
                "description": "Light intensity",
                "type": "number",
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
            "info": {
                "title": "ModelInfo",
                "description": "Model-specific metadata",
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
                        "description": "Model load information",
                        "type": "object",
                        "readOnly": true,
                        "properties": {
                            "load_parameters": {
                                "description": "Loader configuration"
                            },
                            "loader_name": {
                                "description": "Loader used",
                                "type": "string"
                            },
                            "path": {
                                "description": "File path in case of file load type",
                                "type": "string"
                            },
                            "source": {
                                "description": "Model load source",
                                "type": "string",
                                "enum": [
                                    "from_file",
                                    "from_blob",
                                    "none"
                                ]
                            }
                        },
                        "required": [
                            "source",
                            "path",
                            "loader_name",
                            "load_parameters"
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
                "description": "Light color (Normalized RGB)",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "edge1": {
                "description": "Sets one of the quad light edges",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "edge2": {
                "description": "Sets one of the quad light edges",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "intensity": {
                "description": "Light intensity",
                "type": "number",
                "minimum": 0
            },
            "position": {
                "description": "Sets the corner position of the quad light",
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
            "info": {
                "title": "ModelInfo",
                "description": "Model-specific metadata",
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
                        "description": "Model load information",
                        "type": "object",
                        "readOnly": true,
                        "properties": {
                            "load_parameters": {
                                "description": "Loader configuration"
                            },
                            "loader_name": {
                                "description": "Loader used",
                                "type": "string"
                            },
                            "path": {
                                "description": "File path in case of file load type",
                                "type": "string"
                            },
                            "source": {
                                "description": "Model load source",
                                "type": "string",
                                "enum": [
                                    "from_file",
                                    "from_blob",
                                    "none"
                                ]
                            }
                        },
                        "required": [
                            "source",
                            "path",
                            "loader_name",
                            "load_parameters"
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
                "description": "Name of the loader to use",
                "type": "string"
            },
            "loader_properties": {
                "description": "Settings to configure the loading process"
            },
            "path": {
                "description": "Path to the file to load",
                "type": "string"
            }
        },
        "required": [
            "path",
            "loader_name",
            "loader_properties"
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
                "info": {
                    "title": "ModelInfo",
                    "description": "Model-specific metadata",
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
                            "description": "Model load information",
                            "type": "object",
                            "readOnly": true,
                            "properties": {
                                "load_parameters": {
                                    "description": "Loader configuration"
                                },
                                "loader_name": {
                                    "description": "Loader used",
                                    "type": "string"
                                },
                                "path": {
                                    "description": "File path in case of file load type",
                                    "type": "string"
                                },
                                "source": {
                                    "description": "Model load source",
                                    "type": "string",
                                    "enum": [
                                        "from_file",
                                        "from_blob",
                                        "none"
                                    ]
                                }
                            },
                            "required": [
                                "source",
                                "path",
                                "loader_name",
                                "load_parameters"
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
            "title": "GeometryWithColor<Plane>",
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
                            "description": "Plane equation coefficients (A, B, C, D from Ax + By + Cz + D = 0)",
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
                "geometry",
                "color"
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
            "info": {
                "title": "ModelInfo",
                "description": "Model-specific metadata",
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
                        "description": "Model load information",
                        "type": "object",
                        "readOnly": true,
                        "properties": {
                            "load_parameters": {
                                "description": "Loader configuration"
                            },
                            "loader_name": {
                                "description": "Loader used",
                                "type": "string"
                            },
                            "path": {
                                "description": "File path in case of file load type",
                                "type": "string"
                            },
                            "source": {
                                "description": "Model load source",
                                "type": "string",
                                "enum": [
                                    "from_file",
                                    "from_blob",
                                    "none"
                                ]
                            }
                        },
                        "required": [
                            "source",
                            "path",
                            "loader_name",
                            "load_parameters"
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
            "title": "GeometryWithColor<Sphere>",
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
                            "description": "Sphere center point",
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
                "geometry",
                "color"
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
            "info": {
                "title": "ModelInfo",
                "description": "Model-specific metadata",
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
                        "description": "Model load information",
                        "type": "object",
                        "readOnly": true,
                        "properties": {
                            "load_parameters": {
                                "description": "Loader configuration"
                            },
                            "loader_name": {
                                "description": "Loader used",
                                "type": "string"
                            },
                            "path": {
                                "description": "File path in case of file load type",
                                "type": "string"
                            },
                            "source": {
                                "description": "Model load source",
                                "type": "string",
                                "enum": [
                                    "from_file",
                                    "from_blob",
                                    "none"
                                ]
                            }
                        },
                        "required": [
                            "source",
                            "path",
                            "loader_name",
                            "load_parameters"
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
                "description": "The method to use for coloring",
                "type": "string"
            },
            "values": {
                "description": "Color input",
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
                "description": "Bool flag enabling or disabling the simulation",
                "type": "boolean"
            },
            "model_id": {
                "description": "ID of the model to enable or disable simulation",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "model_id",
            "enabled"
        ],
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.

----

exit-later
~~~~~~~~~~

Old monitoring, use 'quit' instead to stop the service.

.. attention::

    This entrypoint is DEPRECATED, it will be removed or renamed in the next
    major release.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "minutes": {
                "description": "Number of minutes after which Brayns will shut down",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "minutes"
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
            "camera_view": {
                "title": "View",
                "description": "Camera view settings",
                "type": "object",
                "properties": {
                    "position": {
                        "description": "Camera position",
                        "type": "array",
                        "items": {
                            "type": "number"
                        },
                        "minItems": 3,
                        "maxItems": 3
                    },
                    "target": {
                        "description": "Camera target",
                        "type": "array",
                        "items": {
                            "type": "number"
                        },
                        "minItems": 3,
                        "maxItems": 3
                    },
                    "up": {
                        "description": "Camera up vector",
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
                "description": "G buffer channels to export",
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
                "description": "Buffers will be saved at this path if specified, otherwise it will be returned as EXR encoded binary data",
                "type": "string"
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
                "description": "Loaded plugins",
                "type": "array",
                "readOnly": true,
                "items": {
                    "type": "string"
                }
            },
            "viewport": {
                "description": "Window size",
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
                "description": "Orthographic projection plane height",
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
                "description": "Camera position",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "target": {
                "description": "Camera target",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "up": {
                "description": "Camera up vector",
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
                "description": "List of colors (RGBA) to map",
                "type": "array",
                "items": {
                    "type": "array",
                    "items": {
                        "type": "number"
                    },
                    "minItems": 4,
                    "maxItems": 4
                }
            },
            "range": {
                "description": "Values range",
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
                "description": "ID of the model to check",
                "type": "integer",
                "minimum": 0
            },
            "method": {
                "description": "The method to query for color values",
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

Get all loaders.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

.. jsonschema::

    {
        "type": "array",
        "items": {
            "title": "LoaderInfo",
            "type": "object",
            "properties": {
                "extensions": {
                    "description": "Supported file extensions",
                    "type": "array",
                    "items": {
                        "type": "string"
                    }
                },
                "input_parameters_schema": {
                    "description": "Loader properties",
                    "type": "object"
                },
                "name": {
                    "description": "Loader name",
                    "type": "string"
                }
            },
            "required": [
                "name",
                "extensions",
                "input_parameters_schema"
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
                "description": "Metal flakes density",
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
                "description": "Base color of the material",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "intensity": {
                "description": "Emitted light intensity",
                "type": "number",
                "minimum": 0
            }
        },
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
                "description": "Opacity of the surface",
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
                "description": "Opacity of the surface",
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
                "description": "Opacity of the surface",
                "type": "number",
                "minimum": 0,
                "maximum": 1
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
            "info": {
                "title": "ModelInfo",
                "description": "Model-specific metadata",
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
                        "description": "Model load information",
                        "type": "object",
                        "readOnly": true,
                        "properties": {
                            "load_parameters": {
                                "description": "Loader configuration"
                            },
                            "loader_name": {
                                "description": "Loader used",
                                "type": "string"
                            },
                            "path": {
                                "description": "File path in case of file load type",
                                "type": "string"
                            },
                            "source": {
                                "description": "Model load source",
                                "type": "string",
                                "enum": [
                                    "from_file",
                                    "from_blob",
                                    "none"
                                ]
                            }
                        },
                        "required": [
                            "source",
                            "path",
                            "loader_name",
                            "load_parameters"
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
                        "info": {
                            "title": "ModelInfo",
                            "description": "Model-specific metadata",
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
                                    "description": "Model load information",
                                    "type": "object",
                                    "readOnly": true,
                                    "properties": {
                                        "load_parameters": {
                                            "description": "Loader configuration"
                                        },
                                        "loader_name": {
                                            "description": "Loader used",
                                            "type": "string"
                                        },
                                        "path": {
                                            "description": "File path in case of file load type",
                                            "type": "string"
                                        },
                                        "source": {
                                            "description": "Model load source",
                                            "type": "string",
                                            "enum": [
                                                "from_file",
                                                "from_blob",
                                                "none"
                                            ]
                                        }
                                    },
                                    "required": [
                                        "source",
                                        "path",
                                        "loader_name",
                                        "load_parameters"
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
                    "additionalProperties": false
                }
            }
        },
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
                "description": "Current frame index",
                "type": "integer",
                "minimum": 0
            },
            "dt": {
                "description": "Frame time",
                "type": "number",
                "readOnly": true
            },
            "end_frame": {
                "description": "Global final simulation frame index",
                "type": "integer",
                "minimum": 0
            },
            "start_frame": {
                "description": "Global initial simulation frame index",
                "type": "integer",
                "minimum": 0
            },
            "unit": {
                "description": "Time unit",
                "type": "string",
                "readOnly": true
            }
        },
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
                "type": "integer"
            },
            "minor": {
                "description": "Minor version",
                "type": "integer"
            },
            "patch": {
                "description": "Patch level",
                "type": "integer"
            },
            "revision": {
                "description": "SCM revision",
                "type": "string"
            }
        },
        "required": [
            "major",
            "minor",
            "patch",
            "revision"
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
                "description": "Position XY (normalized)",
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
                "description": "A boolean flag indicating wether there was a hit. If false, the rest of the fields must be ignored",
                "type": "boolean"
            },
            "metadata": {
                "description": "Extra attributes which vary depending on the type of model hitted"
            },
            "model_id": {
                "description": "ID of the model hitted",
                "type": "integer",
                "minimum": 0
            },
            "position": {
                "description": "3D hit position",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            }
        },
        "required": [
            "hit",
            "position",
            "model_id",
            "metadata"
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
                "description": "Model to instantiate",
                "type": "integer",
                "minimum": 0
            },
            "transforms": {
                "description": "New instances transforms",
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
                "info": {
                    "title": "ModelInfo",
                    "description": "Model-specific metadata",
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
                            "description": "Model load information",
                            "type": "object",
                            "readOnly": true,
                            "properties": {
                                "load_parameters": {
                                    "description": "Loader configuration"
                                },
                                "loader_name": {
                                    "description": "Loader used",
                                    "type": "string"
                                },
                                "path": {
                                    "description": "File path in case of file load type",
                                    "type": "string"
                                },
                                "source": {
                                    "description": "Model load source",
                                    "type": "string",
                                    "enum": [
                                        "from_file",
                                        "from_blob",
                                        "none"
                                    ]
                                }
                            },
                            "required": [
                                "source",
                                "path",
                                "loader_name",
                                "load_parameters"
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
                "description": "List of model ID to remove",
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
                "description": "Keep rendering until max accumulation",
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
                "minimum": 0
            },
            "max_accumulation": {
                "description": "Maximum frame accumulation",
                "type": "integer",
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
                "description": "Check if the entrypoint is asynchronous",
                "type": "boolean"
            },
            "deprecated": {
                "description": "If true, the entrypoint will be removed / renamed in the next release",
                "type": "boolean"
            },
            "description": {
                "description": "Description of the entrypoint",
                "type": "string"
            },
            "params": {
                "description": "Input schema",
                "type": "object"
            },
            "plugin": {
                "description": "Name of the plugin that loads the entrypoint",
                "type": "string"
            },
            "returns": {
                "description": "Output schema",
                "type": "object"
            },
            "title": {
                "description": "Name of the entrypoint",
                "type": "string"
            }
        },
        "required": [
            "plugin",
            "title",
            "description",
            "async",
            "deprecated"
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
                "description": "Loaded plugins",
                "type": "array",
                "readOnly": true,
                "items": {
                    "type": "string"
                }
            },
            "viewport": {
                "description": "Window size",
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
                "description": "Orthographic projection plane height",
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
                "description": "Camera position",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "target": {
                "description": "Camera target",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "up": {
                "description": "Camera up vector",
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
                        "description": "List of colors (RGBA) to map",
                        "type": "array",
                        "items": {
                            "type": "array",
                            "items": {
                                "type": "number"
                            },
                            "minItems": 4,
                            "maxItems": 4
                        }
                    },
                    "range": {
                        "description": "Values range",
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
            "id",
            "color_ramp"
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
                        "description": "Metal flakes density",
                        "type": "number",
                        "minimum": 0,
                        "maximum": 1
                    }
                },
                "additionalProperties": false
            },
            "model_id": {
                "description": "Model ID",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "model_id",
            "material"
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
                        "description": "Base color of the material",
                        "type": "array",
                        "items": {
                            "type": "number"
                        },
                        "minItems": 3,
                        "maxItems": 3
                    },
                    "intensity": {
                        "description": "Emitted light intensity",
                        "type": "number",
                        "minimum": 0
                    }
                },
                "additionalProperties": false
            },
            "model_id": {
                "description": "Model ID",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "model_id",
            "material"
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
                "description": "Model ID",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "model_id",
            "material"
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
                        "description": "Opacity of the surface",
                        "type": "number",
                        "minimum": 0,
                        "maximum": 1
                    }
                },
                "additionalProperties": false
            },
            "model_id": {
                "description": "Model ID",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "model_id",
            "material"
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
                "description": "Model ID",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "model_id",
            "material"
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
                        "description": "Opacity of the surface",
                        "type": "number",
                        "minimum": 0,
                        "maximum": 1
                    }
                },
                "additionalProperties": false
            },
            "model_id": {
                "description": "Model ID",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "model_id",
            "material"
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
                        "description": "Opacity of the surface",
                        "type": "number",
                        "minimum": 0,
                        "maximum": 1
                    }
                },
                "additionalProperties": false
            },
            "model_id": {
                "description": "Model ID",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "model_id",
            "material"
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
                "description": "Current frame index",
                "type": "integer",
                "minimum": 0
            },
            "dt": {
                "description": "Frame time",
                "type": "number",
                "readOnly": true
            },
            "end_frame": {
                "description": "Global final simulation frame index",
                "type": "integer",
                "minimum": 0
            },
            "start_frame": {
                "description": "Global initial simulation frame index",
                "type": "integer",
                "minimum": 0
            },
            "unit": {
                "description": "Time unit",
                "type": "string",
                "readOnly": true
            }
        },
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
            "camera_view": {
                "title": "View",
                "description": "Camera view settings",
                "type": "object",
                "properties": {
                    "position": {
                        "description": "Camera position",
                        "type": "array",
                        "items": {
                            "type": "number"
                        },
                        "minItems": 3,
                        "maxItems": 3
                    },
                    "target": {
                        "description": "Camera target",
                        "type": "array",
                        "items": {
                            "type": "number"
                        },
                        "minItems": 3,
                        "maxItems": 3
                    },
                    "up": {
                        "description": "Camera up vector",
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
                "description": "Snapshot will be saved at this path if specified, otherwise it will be returned as binary data with format from image_settings",
                "type": "string"
            },
            "image_settings": {
                "title": "ImageSettings",
                "description": "Image settings",
                "type": "object",
                "properties": {
                    "format": {
                        "description": "Image format (jpg or png)",
                        "type": "string"
                    },
                    "quality": {
                        "description": "Image quality (0 = lowest quality, 100 = highest quality",
                        "type": "integer",
                        "minimum": 0
                    },
                    "size": {
                        "description": "Image dimensions [width, height]",
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
                "description": "Snapshot color buffer encoded in params format",
                "type": "object",
                "properties": {
                    "offset": {
                        "description": "Buffer data offset in attached binary",
                        "type": "integer",
                        "minimum": 0
                    },
                    "size": {
                        "description": "Buffer data size in attached binary",
                        "type": "integer",
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
                    "info": {
                        "title": "ModelInfo",
                        "description": "Model-specific metadata",
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
                                "description": "Model load information",
                                "type": "object",
                                "readOnly": true,
                                "properties": {
                                    "load_parameters": {
                                        "description": "Loader configuration"
                                    },
                                    "loader_name": {
                                        "description": "Loader used",
                                        "type": "string"
                                    },
                                    "path": {
                                        "description": "File path in case of file load type",
                                        "type": "string"
                                    },
                                    "source": {
                                        "description": "Model load source",
                                        "type": "string",
                                        "enum": [
                                            "from_file",
                                            "from_blob",
                                            "none"
                                        ]
                                    }
                                },
                                "required": [
                                    "source",
                                    "path",
                                    "loader_name",
                                    "load_parameters"
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
                "additionalProperties": false
            },
            "model_id": {
                "description": "Model ID",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "model_id",
            "model"
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
            "info": {
                "title": "ModelInfo",
                "description": "Model-specific metadata",
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
                        "description": "Model load information",
                        "type": "object",
                        "readOnly": true,
                        "properties": {
                            "load_parameters": {
                                "description": "Loader configuration"
                            },
                            "loader_name": {
                                "description": "Loader used",
                                "type": "string"
                            },
                            "path": {
                                "description": "File path in case of file load type",
                                "type": "string"
                            },
                            "source": {
                                "description": "Model load source",
                                "type": "string",
                                "enum": [
                                    "from_file",
                                    "from_blob",
                                    "none"
                                ]
                            }
                        },
                        "required": [
                            "source",
                            "path",
                            "loader_name",
                            "load_parameters"
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
            "type",
            "loader_name",
            "loader_properties"
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
                "info": {
                    "title": "ModelInfo",
                    "description": "Model-specific metadata",
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
                            "description": "Model load information",
                            "type": "object",
                            "readOnly": true,
                            "properties": {
                                "load_parameters": {
                                    "description": "Loader configuration"
                                },
                                "loader_name": {
                                    "description": "Loader used",
                                    "type": "string"
                                },
                                "path": {
                                    "description": "File path in case of file load type",
                                    "type": "string"
                                },
                                "source": {
                                    "description": "Model load source",
                                    "type": "string",
                                    "enum": [
                                        "from_file",
                                        "from_blob",
                                        "none"
                                    ]
                                }
                            },
                            "required": [
                                "source",
                                "path",
                                "loader_name",
                                "load_parameters"
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
            "additionalProperties": false
        }
    }
