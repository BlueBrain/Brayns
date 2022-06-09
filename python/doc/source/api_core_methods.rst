.. _apicore-label:

Core API methods
----------------

This page references the entrypoints of the Core plugin.

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

This entrypoint has no result, the "result" field is still present but is always null.

----

add-capsules
~~~~~~~~~~~~

Adds a list of capsules to the scene.

**Params**:

.. jsonschema::

    {
        "type": "array",
        "items": {
            "title": "GeometryWithColor<Primitive>",
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
                    "title": "Primitive",
                    "description": "Geometry data",
                    "type": "object",
                    "properties": {
                        "p0": {
                            "description": "Starting point of the primitive",
                            "type": "array",
                            "items": {
                                "type": "number"
                            },
                            "minItems": 3,
                            "maxItems": 3
                        },
                        "p1": {
                            "description": "Ending point of the primitive",
                            "type": "array",
                            "items": {
                                "type": "number"
                            },
                            "minItems": 3,
                            "maxItems": 3
                        },
                        "r0": {
                            "description": "Primitive radius at p0",
                            "type": "number"
                        },
                        "r1": {
                            "description": "Primitive radius at p1",
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

This entrypoint has no result, the "result" field is still present but is always null.

----

add-clip-plane
~~~~~~~~~~~~~~

Add a clip plane and returns the clip plane ID.

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

This entrypoint has no result, the "result" field is still present but is always null.

----

add-light-ambient
~~~~~~~~~~~~~~~~~

Add an ambient light which iluminates all the scen from all directions.

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
            },
            "visible": {
                "description": "Sets wether the light should be visible on the scene",
                "type": "boolean"
            }
        },
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

----

add-light-directional
~~~~~~~~~~~~~~~~~~~~~

Add a directional light and return its ID.

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
            },
            "visible": {
                "description": "Sets wether the light should be visible on the scene",
                "type": "boolean"
            }
        },
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

----

add-light-quad
~~~~~~~~~~~~~~

Add a quad light and return its ID.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "bottom_left_corner": {
                "description": "Sets the bottom left corner position of the light (in world space coordinates)",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "color": {
                "description": "Light color (Normalized RGB)",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "horizontal_displacement": {
                "description": "Sets the horizontal displacement vector used to compute the bottom right corner",
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
            "vertical_displacement": {
                "description": "Sets the vertical displacement vector used to compute the top left corner",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "visible": {
                "description": "Sets wether the light should be visible on the scene",
                "type": "boolean"
            }
        },
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

----

add-model
~~~~~~~~~

Add model from path and return model descriptor on success.

This entrypoint is asynchronous, it means that it can take a long time and send progress notifications.

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

This entrypoint has no result, the "result" field is still present but is always null.

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

This entrypoint has no result, the "result" field is still present but is always null.

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

This entrypoint has no result, the "result" field is still present but is always null.

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

This entrypoint has no result, the "result" field is still present but is always null.

----

clear-clip-planes
~~~~~~~~~~~~~~~~~

Clear all clip planes in the scene.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

----

clear-lights
~~~~~~~~~~~~

Clear all lights in the scene.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

----

clear-models
~~~~~~~~~~~~

Clear all models in the scene.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

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

This entrypoint has no result, the "result" field is still present but is always null.

----

exit-later
~~~~~~~~~~

Schedules Brayns to shutdown after a given amount of minutes.

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

This entrypoint has no result, the "result" field is still present but is always null.

----

export-frames
~~~~~~~~~~~~~

Export a list of keyframes as images to disk.

This entrypoint is asynchronous, it means that it can take a long time and send progress notifications.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "camera": {
                "title": "GenericObject<Camera>",
                "description": "Camera definition",
                "type": "object",
                "properties": {
                    "name": {
                        "description": "Type name",
                        "type": "string",
                        "writeOnly": true
                    },
                    "params": {
                        "description": "Type parameters",
                        "writeOnly": true
                    }
                },
                "additionalProperties": false
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
            "key_frames": {
                "description": "List of keyframes to export",
                "type": "array",
                "items": {
                    "title": "ExportFramesKeyFrame",
                    "type": "object",
                    "properties": {
                        "camera_view": {
                            "title": "LookAt",
                            "description": "Camera view settings",
                            "type": "object",
                            "properties": {
                                "position": {
                                    "description": "Position of the camera",
                                    "type": "array",
                                    "items": {
                                        "type": "number"
                                    },
                                    "minItems": 3,
                                    "maxItems": 3
                                },
                                "target": {
                                    "description": "Target position at which the camera is looking",
                                    "type": "array",
                                    "items": {
                                        "type": "number"
                                    },
                                    "minItems": 3,
                                    "maxItems": 3
                                },
                                "up": {
                                    "description": "Up vector to compute the camera orthonormal basis",
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
                        "frame_index": {
                            "description": "Integer index of the simulation frame",
                            "type": "integer",
                            "minimum": 0
                        }
                    },
                    "required": [
                        "frame_index"
                    ],
                    "additionalProperties": false
                }
            },
            "path": {
                "description": "Path where the frames will be stored",
                "type": "string"
            },
            "renderer": {
                "title": "GenericObject<Renderer>",
                "description": "Renderer definition",
                "type": "object",
                "properties": {
                    "name": {
                        "description": "Type name",
                        "type": "string",
                        "writeOnly": true
                    },
                    "params": {
                        "description": "Type parameters",
                        "writeOnly": true
                    }
                },
                "additionalProperties": false
            },
            "sequential_naming": {
                "description": "Name the image file after the frame index",
                "type": "boolean",
                "default": true
            }
        },
        "required": [
            "path",
            "key_frames"
        ],
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

----

get-application-parameters
~~~~~~~~~~~~~~~~~~~~~~~~~~

Get the current state of the application parameters.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

----

get-camera-look-at
~~~~~~~~~~~~~~~~~~

Returns the camera view settings.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

----

get-camera-orthographic
~~~~~~~~~~~~~~~~~~~~~~~

Returns the current camera as orthographic.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

----

get-camera-perspective
~~~~~~~~~~~~~~~~~~~~~~

Returns the current camera as perspective.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

----

get-camera-type
~~~~~~~~~~~~~~~

Returns the type of the current camera.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

----

get-loaders
~~~~~~~~~~~

Get all loaders.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

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

This entrypoint has no result, the "result" field is still present but is always null.

----

get-material-default
~~~~~~~~~~~~~~~~~~~~

Returns the material of the given model as a default material, if possible.

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

This entrypoint has no result, the "result" field is still present but is always null.

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

This entrypoint has no result, the "result" field is still present but is always null.

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

This entrypoint has no result, the "result" field is still present but is always null.

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

This entrypoint has no result, the "result" field is still present but is always null.

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

This entrypoint has no result, the "result" field is still present but is always null.

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

This entrypoint has no result, the "result" field is still present but is always null.

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

This entrypoint has no result, the "result" field is still present but is always null.

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

This entrypoint has no result, the "result" field is still present but is always null.

----

get-model-transfer-function
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Get the transfer function of the given model.

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

This entrypoint has no result, the "result" field is still present but is always null.

----

get-renderer-interactive
~~~~~~~~~~~~~~~~~~~~~~~~

Returns the current renderer as interactive renderer, if possible.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

----

get-renderer-production
~~~~~~~~~~~~~~~~~~~~~~~

Returns the current renderer as production renderer, if possible.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

----

get-renderer-type
~~~~~~~~~~~~~~~~~

Returns the type of the renderer currently being used.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

----

get-scene
~~~~~~~~~

Get the current state of the scene.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

----

get-simulation-parameters
~~~~~~~~~~~~~~~~~~~~~~~~~

Get the current state of the simulation parameters.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

----

get-version
~~~~~~~~~~~

Get Brayns instance version.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

----

image-jpeg
~~~~~~~~~~

Take a snapshot at JPEG format.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

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

This entrypoint has no result, the "result" field is still present but is always null.

----

quit
~~~~

Quit the application.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

----

registry
~~~~~~~~

Retreive the names of all registered entrypoints.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

----

remove-clip-planes
~~~~~~~~~~~~~~~~~~

Remove clip planes from the scene given their ids.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "ids": {
                "description": "Clip planes ID list",
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

This entrypoint has no result, the "result" field is still present but is always null.

----

remove-lights
~~~~~~~~~~~~~

Remove the model(s) from the ID list from the scene.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "ids": {
                "description": "List of light ID to remove",
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

This entrypoint has no result, the "result" field is still present but is always null.

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

This entrypoint has no result, the "result" field is still present but is always null.

----

request-model-upload
~~~~~~~~~~~~~~~~~~~~

Request model upload from next binary frame received and return model descriptors on success.

This entrypoint is asynchronous, it means that it can take a long time and send progress notifications.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "chunks_id": {
                "description": "Chunk ID",
                "type": "string"
            },
            "loader_name": {
                "description": "Loader name",
                "type": "string"
            },
            "loader_properties": {
                "description": "Loader properties"
            },
            "size": {
                "description": "File size in bytes",
                "type": "integer",
                "minimum": 0
            },
            "type": {
                "description": "File extension",
                "type": "string"
            }
        },
        "required": [
            "chunks_id",
            "size",
            "type",
            "loader_name",
            "loader_properties"
        ],
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

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

This entrypoint has no result, the "result" field is still present but is always null.

----

set-application-parameters
~~~~~~~~~~~~~~~~~~~~~~~~~~

Set the current state of the application parameters.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "jpeg_quality": {
                "description": "JPEG quality",
                "type": "integer",
                "minimum": 0,
                "maximum": 100
            },
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

This entrypoint has no result, the "result" field is still present but is always null.

----

set-camera-look-at
~~~~~~~~~~~~~~~~~~

Sets the camera view settings.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "position": {
                "description": "Position of the camera",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "target": {
                "description": "Target position at which the camera is looking",
                "type": "array",
                "items": {
                    "type": "number"
                },
                "minItems": 3,
                "maxItems": 3
            },
            "up": {
                "description": "Up vector to compute the camera orthonormal basis",
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

This entrypoint has no result, the "result" field is still present but is always null.

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
                "description": "Camera orthographic projection height",
                "type": "number"
            }
        },
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

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
                "description": "Lens aperture radius (Use for depth of field effect. A value of 0.0 disables it",
                "type": "number",
                "default": 0,
                "minimum": 0
            },
            "focus_distance": {
                "description": "Distance at which to focus (for depth of field effect). A value of 1.0 disables it.",
                "type": "number",
                "default": 1,
                "minimum": 1
            },
            "fovy": {
                "description": "Camera vertical field of view (in degrees)",
                "type": "number",
                "default": 45,
                "minimum": 1
            }
        },
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

----

set-material-carpaint
~~~~~~~~~~~~~~~~~~~~~

Updates the material of the given model to a Car paint material. This material is only usable with the production renderer.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "material": {
                "title": "CarPaintMaterial",
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
                    "flake_density": {
                        "description": "Normalized percentage of flakes on the surface. Will be clampled to the range [0.0, 1.0]",
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

This entrypoint has no result, the "result" field is still present but is always null.

----

set-material-default
~~~~~~~~~~~~~~~~~~~~

Updates the material of the given model to the Default material. This material works with all renderers. It has a matte appearance..

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "material": {
                "title": "DefaultMaterial",
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
                    "opacity": {
                        "description": "Base opacity of the material. Will be clampled to the range [0.0, 1.0]",
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

This entrypoint has no result, the "result" field is still present but is always null.

----

set-material-emissive
~~~~~~~~~~~~~~~~~~~~~

Updates the material of the given model to an Emisive material. This material is only usable with the production renderer.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "material": {
                "title": "EmissiveMaterial",
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
                        "description": "Intensity of the light emitted. Will be clampled to the range [0.0, +infinite]",
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

This entrypoint has no result, the "result" field is still present but is always null.

----

set-material-glass
~~~~~~~~~~~~~~~~~~

Updates the material of the given model to a Glass material. This material is only usable with the production renderer.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "material": {
                "title": "GlassMaterial",
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

This entrypoint has no result, the "result" field is still present but is always null.

----

set-material-matte
~~~~~~~~~~~~~~~~~~

Updates the material of the given model to a Matte material. This material is only usable with the production renderer.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "material": {
                "title": "MatteMaterial",
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
                    "opacity": {
                        "description": "Base opacity of the material. Will be clampled to the range [0.0, 1.0]",
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

This entrypoint has no result, the "result" field is still present but is always null.

----

set-material-metal
~~~~~~~~~~~~~~~~~~

Updates the material of the given model to a Metal material. This material is only usable with the production renderer.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "material": {
                "title": "MetalMaterial",
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
                    "roughness": {
                        "description": "Surface roughness. Will be clamped on the range [0-1]",
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

This entrypoint has no result, the "result" field is still present but is always null.

----

set-material-plastic
~~~~~~~~~~~~~~~~~~~~

Updates the material of the given model to a Plastic material. This material is only usable with the production renderer.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "material": {
                "title": "PlasticMaterial",
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
                    "opacity": {
                        "description": "Base opacity of the material. Will be clampled to the range [0.0, 1.0]",
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

This entrypoint has no result, the "result" field is still present but is always null.

----

set-model-transfer-function
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Set the transfer function of the given model.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "id": {
                "description": "Model ID",
                "type": "integer",
                "minimum": 0
            },
            "transfer_function": {
                "title": "TransferFunction",
                "description": "Transfer function",
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
        },
        "required": [
            "id",
            "transfer_function"
        ],
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

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
                "description": "Sets number of samples to compute ambient occlusion",
                "type": "integer"
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
                "description": "Render casted shadows",
                "type": "boolean"
            },
            "max_ray_bounces": {
                "description": "Max ray bounces per sample",
                "type": "integer"
            },
            "samples_per_pixel": {
                "description": "Number of samples per pixel",
                "type": "integer"
            }
        },
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

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
                "description": "Max ray bounces per sample",
                "type": "integer"
            },
            "samples_per_pixel": {
                "description": "Number of samples per pixel",
                "type": "integer"
            }
        },
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

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

This entrypoint has no result, the "result" field is still present but is always null.

----

snapshot
~~~~~~~~

Take a snapshot with given parameters.

This entrypoint is asynchronous, it means that it can take a long time and send progress notifications.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "camera": {
                "title": "GenericObject<Camera>",
                "description": "Camera definition",
                "type": "object",
                "properties": {
                    "name": {
                        "description": "Type name",
                        "type": "string",
                        "writeOnly": true
                    },
                    "params": {
                        "description": "Type parameters",
                        "writeOnly": true
                    }
                },
                "additionalProperties": false
            },
            "camera_view": {
                "title": "LookAt",
                "description": "Camera 'look at' view settings",
                "type": "object",
                "properties": {
                    "position": {
                        "description": "Position of the camera",
                        "type": "array",
                        "items": {
                            "type": "number"
                        },
                        "minItems": 3,
                        "maxItems": 3
                    },
                    "target": {
                        "description": "Target position at which the camera is looking",
                        "type": "array",
                        "items": {
                            "type": "number"
                        },
                        "minItems": 3,
                        "maxItems": 3
                    },
                    "up": {
                        "description": "Up vector to compute the camera orthonormal basis",
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
                "description": "Path if saved on disk. If empty, image will be sent to the client as a base64 encoded image",
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
                "title": "GenericObject<Renderer>",
                "description": "Renderer definition",
                "type": "object",
                "properties": {
                    "name": {
                        "description": "Type name",
                        "type": "string",
                        "writeOnly": true
                    },
                    "params": {
                        "description": "Type parameters",
                        "writeOnly": true
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

This entrypoint has no result, the "result" field is still present but is always null.

----

trigger-jpeg-stream
~~~~~~~~~~~~~~~~~~~

Triggers the engine to stream a frame to the clients.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

This entrypoint has no result, the "result" field is still present but is always null.

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

This entrypoint has no result, the "result" field is still present but is always null.
