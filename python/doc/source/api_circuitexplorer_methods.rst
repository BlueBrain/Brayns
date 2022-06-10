.. _apicircuitexplorer-label:

Circuit Explorer API methods
----------------------------

This page references the entrypoints of the Circuit Explorer plugin.

color-circuit-by-id
~~~~~~~~~~~~~~~~~~~

Colors a circuit model by element ID. Specific IDs can be targeted, otherwise, random colors per ID will be applied. Returns a list of IDs that were not colored (if any).

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "color_info": {
                "description": "List of IDs with their corresponding color. If empty, all the model will be colored with random colors per ID",
                "type": "array",
                "items": {
                    "title": "ColoringInformation",
                    "type": "object",
                    "properties": {
                        "color": {
                            "description": "Color applied to the given variable (Normalized RGBA)",
                            "type": "array",
                            "items": {
                                "type": "number"
                            },
                            "minItems": 4,
                            "maxItems": 4
                        },
                        "variable": {
                            "description": "Variable of the coloring method being used",
                            "type": "string"
                        }
                    },
                    "required": [
                        "variable",
                        "color"
                    ],
                    "additionalProperties": false
                }
            },
            "model_id": {
                "description": "ID of the model to color",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "model_id",
            "color_info"
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

color-circuit-by-method
~~~~~~~~~~~~~~~~~~~~~~~

Colors a circuit model by grouping its elements using the given method. Specific variables can be targetted, otherwise, random colors per variable group will be applied.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "color_info": {
                "description": "List of IDs with their corresponding color. If empty, all the model will be colored with random colors per ID",
                "type": "array",
                "items": {
                    "title": "ColoringInformation",
                    "type": "object",
                    "properties": {
                        "color": {
                            "description": "Color applied to the given variable (Normalized RGBA)",
                            "type": "array",
                            "items": {
                                "type": "number"
                            },
                            "minItems": 4,
                            "maxItems": 4
                        },
                        "variable": {
                            "description": "Variable of the coloring method being used",
                            "type": "string"
                        }
                    },
                    "required": [
                        "variable",
                        "color"
                    ],
                    "additionalProperties": false
                }
            },
            "method": {
                "description": "Method to use for coloring",
                "type": "string"
            },
            "model_id": {
                "description": "ID of the model to color",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "model_id",
            "method",
            "color_info"
        ],
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

----

color-circuit-by-single-color
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Colors a whole circuit model with a single color.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "color": {
                "description": "Color to use for the whole circuit (Normalized RGBA)",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 4,
                "maxItems": 4
            },
            "model_id": {
                "description": "ID of the model to color",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "model_id",
            "color"
        ],
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

----

get-circuit-color-method-variables
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Return the available variables which can be specified when coloring a circuit model by the given method.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "method": {
                "description": "Name of the method to query",
                "type": "string"
            },
            "model_id": {
                "description": "ID of the model to query",
                "type": "integer",
                "minimum": 0
            }
        },
        "required": [
            "model_id",
            "method"
        ],
        "additionalProperties": false
    }

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "variables": {
                "description": "Available variables for the given circuit model and method",
                "type": "array",
                "items": {
                    "type": "string"
                }
            }
        },
        "required": [
            "variables"
        ],
        "additionalProperties": false
    }

----

get-circuit-color-methods
~~~~~~~~~~~~~~~~~~~~~~~~~

Return the available extra coloring methods for a circuit model.

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
        "type": "object",
        "properties": {
            "methods": {
                "description": "Available coloring methods",
                "type": "array",
                "items": {
                    "type": "string"
                }
            }
        },
        "required": [
            "methods"
        ],
        "additionalProperties": false
    }

----

make-movie
~~~~~~~~~~

Builds a movie file from a set of frames stored on disk.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "dimensions": {
                "description": "Video dimensions (width,height)",
                "type": "array",
                "items": {
                    "type": "integer",
                    "minimum": 0
                },
                "minItems": 2,
                "maxItems": 2
            },
            "erase_frames": {
                "description": "Wether to clean up the frame image files after generating the video file",
                "type": "boolean"
            },
            "fps_rate": {
                "description": "The frames per second rate at which to create the video",
                "type": "integer",
                "minimum": 0
            },
            "frames_file_extension": {
                "description": "The extension of the frame files to fetch (ex: png, jpg)",
                "type": "string"
            },
            "frames_folder_path": {
                "description": "Path to where to fetch the frames to create the video",
                "type": "string"
            },
            "output_movie_path": {
                "description": "The path to where the movie will be created. Must include filename and extension",
                "type": "string"
            }
        },
        "required": [
            "dimensions",
            "frames_folder_path",
            "frames_file_extension",
            "fps_rate",
            "output_movie_path",
            "erase_frames"
        ],
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

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

This entrypoint has no result, the "result" field is still present but is always null.

----

trace-anterograde
~~~~~~~~~~~~~~~~~

Performs neuronal tracing showing efferent and afferent synapse relationship between cells (including projections).

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "cell_gids": {
                "description": "List of cell GIDs to use a source of the tracing",
                "type": "array",
                "items": {
                    "type": "integer",
                    "minimum": 0
                }
            },
            "connected_cells_color": {
                "description": "RGBA normalized color to apply to the target cells geometry",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 4,
                "maxItems": 4
            },
            "model_id": {
                "description": "Model where to perform the neuronal tracing",
                "type": "integer",
                "minimum": 0
            },
            "non_connected_cells_color": {
                "description": "RGBA normalized color to apply to the rest of cells",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 4,
                "maxItems": 4
            },
            "source_cell_color": {
                "description": "RGBA normalized color to apply to the source cell geometry",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 4,
                "maxItems": 4
            },
            "target_cell_gids": {
                "description": "List of cells GIDs which are the result of the given tracing mode",
                "type": "array",
                "items": {
                    "type": "integer",
                    "minimum": 0
                }
            }
        },
        "required": [
            "model_id",
            "cell_gids",
            "target_cell_gids",
            "source_cell_color",
            "connected_cells_color",
            "non_connected_cells_color"
        ],
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.
