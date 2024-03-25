.. _apicircuitexplorer-label:

Circuit Explorer API
====================

This page references the loaders and entrypoints registered by the Circuit Explorer plugin.

Loaders
-------

Cell placement loader
~~~~~~~~~~~~~~~~~~~~~

Can load the following formats: **circuit.morphologies.h5**.

This loader does not support loading binary data using 'upload-model'.

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "extension": {
                "description": "Morphology file extension",
                "type": "string"
            },
            "ids": {
                "description": "IDs of the nodes to load (overrides percentage)",
                "type": "array",
                "items": {
                    "type": "integer",
                    "minimum": 0
                }
            },
            "morphology_folder": {
                "description": "Path to morphology folder",
                "type": "string"
            },
            "morphology_parameters": {
                "title": "NeuronMorphologyLoaderParameters",
                "description": "Settings for morphology geometry loading",
                "type": "object",
                "default": {},
                "properties": {
                    "geometry_type": {
                        "description": "Geometry generation configuration",
                        "type": "string",
                        "default": "smooth",
                        "enum": [
                            "original",
                            "smooth",
                            "section_smooth",
                            "constant_radii",
                            "spheres"
                        ]
                    },
                    "load_axon": {
                        "description": "Load the axon section of the neuron",
                        "type": "boolean",
                        "default": false
                    },
                    "load_dendrites": {
                        "description": "Load the dendrites section of the neuron",
                        "type": "boolean",
                        "default": false
                    },
                    "load_soma": {
                        "description": "Load the soma section of the neuron",
                        "type": "boolean",
                        "default": false
                    },
                    "radius_multiplier": {
                        "description": "Parameter to multiply all morphology sample radii by",
                        "type": "number",
                        "default": 1,
                        "minimum": 0.1
                    },
                    "resampling": {
                        "description": "Minimum angle cosine between 2 segments to merge them (disabled if > 1)",
                        "type": "number",
                        "default": 2
                    },
                    "subsampling": {
                        "description": "Skip factor when converting samples into geometry (disabled if <= 1)",
                        "type": "integer",
                        "default": 1,
                        "minimum": 0
                    }
                },
                "additionalProperties": false
            },
            "percentage": {
                "description": "Percentage of cells to load",
                "type": "number",
                "default": 1,
                "minimum": 0,
                "maximum": 1
            }
        },
        "additionalProperties": false
    }

----

Neuron Morphology loader
~~~~~~~~~~~~~~~~~~~~~~~~

Can load the following formats: **swc**, **h5**, **asc**.

This loader does not support loading binary data using 'upload-model'.

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "geometry_type": {
                "description": "Geometry generation configuration",
                "type": "string",
                "default": "smooth",
                "enum": [
                    "original",
                    "smooth",
                    "section_smooth",
                    "constant_radii",
                    "spheres"
                ]
            },
            "load_axon": {
                "description": "Load the axon section of the neuron",
                "type": "boolean",
                "default": false
            },
            "load_dendrites": {
                "description": "Load the dendrites section of the neuron",
                "type": "boolean",
                "default": false
            },
            "load_soma": {
                "description": "Load the soma section of the neuron",
                "type": "boolean",
                "default": false
            },
            "radius_multiplier": {
                "description": "Parameter to multiply all morphology sample radii by",
                "type": "number",
                "default": 1,
                "minimum": 0.1
            },
            "resampling": {
                "description": "Minimum angle cosine between 2 segments to merge them (disabled if > 1)",
                "type": "number",
                "default": 2
            },
            "subsampling": {
                "description": "Skip factor when converting samples into geometry (disabled if <= 1)",
                "type": "integer",
                "default": 1,
                "minimum": 0
            }
        },
        "additionalProperties": false
    }

----

SONATA loader
~~~~~~~~~~~~~

Can load the following formats: **.json**.

This loader does not support loading binary data using 'upload-model'.

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "node_population_settings": {
                "description": "List of node populations to load and their settings",
                "type": "array",
                "items": {
                    "title": "SonataNodePopulationParameters",
                    "type": "object",
                    "properties": {
                        "edge_populations": {
                            "description": "List of edge populations to load and their settings",
                            "type": "array",
                            "items": {
                                "title": "SonataEdgePopulationParameters",
                                "type": "object",
                                "properties": {
                                    "edge_percentage": {
                                        "description": "Percentage of edges to load from all available",
                                        "type": "number",
                                        "default": 1,
                                        "minimum": 0,
                                        "maximum": 1
                                    },
                                    "edge_population": {
                                        "description": "Name of the edge population to load",
                                        "type": "string"
                                    },
                                    "edge_report_name": {
                                        "description": "Name of a synapse report to load along the edge population",
                                        "type": "string"
                                    },
                                    "load_afferent": {
                                        "description": "Wether to load afferent or efferent edges",
                                        "type": "boolean"
                                    },
                                    "radius": {
                                        "description": "Radius used for the synapse sphere geometry (Ignored for endfeet)",
                                        "type": "number",
                                        "default": 2,
                                        "minimum": 0.1
                                    }
                                },
                                "required": [
                                    "edge_population",
                                    "load_afferent"
                                ],
                                "additionalProperties": false
                            }
                        },
                        "neuron_morphology_parameters": {
                            "title": "NeuronMorphologyLoaderParameters",
                            "description": "Settings for morphology geometry loading (ignored for vasculature populations)",
                            "type": "object",
                            "default": {},
                            "properties": {
                                "geometry_type": {
                                    "description": "Geometry generation configuration",
                                    "type": "string",
                                    "default": "smooth",
                                    "enum": [
                                        "original",
                                        "smooth",
                                        "section_smooth",
                                        "constant_radii",
                                        "spheres"
                                    ]
                                },
                                "load_axon": {
                                    "description": "Load the axon section of the neuron",
                                    "type": "boolean",
                                    "default": false
                                },
                                "load_dendrites": {
                                    "description": "Load the dendrites section of the neuron",
                                    "type": "boolean",
                                    "default": false
                                },
                                "load_soma": {
                                    "description": "Load the soma section of the neuron",
                                    "type": "boolean",
                                    "default": false
                                },
                                "radius_multiplier": {
                                    "description": "Parameter to multiply all morphology sample radii by",
                                    "type": "number",
                                    "default": 1,
                                    "minimum": 0.1
                                },
                                "resampling": {
                                    "description": "Minimum angle cosine between 2 segments to merge them (disabled if > 1)",
                                    "type": "number",
                                    "default": 2
                                },
                                "subsampling": {
                                    "description": "Skip factor when converting samples into geometry (disabled if <= 1)",
                                    "type": "integer",
                                    "default": 1,
                                    "minimum": 0
                                }
                            },
                            "additionalProperties": false
                        },
                        "node_count_limit": {
                            "description": "Maximum number of nodes to load",
                            "type": "integer",
                            "minimum": 0
                        },
                        "node_ids": {
                            "description": "List of node IDs to load (invalidates 'node_percentage' and 'node_sets')",
                            "type": "array",
                            "items": {
                                "type": "integer",
                                "minimum": 0
                            }
                        },
                        "node_percentage": {
                            "description": "Percentage of nodes to load (after nodeset filter) (ignored if node_ids is provided)",
                            "type": "number",
                            "default": 0.01,
                            "minimum": 0,
                            "maximum": 1
                        },
                        "node_population": {
                            "description": "Name of the node population to load",
                            "type": "string"
                        },
                        "node_sets": {
                            "description": "List of node set names/regex to filter the node population (ignored if node_ids is provided)",
                            "type": "array",
                            "items": {
                                "type": "string"
                            }
                        },
                        "report_name": {
                            "description": "Name of the report file to load (ignored if report_type is 'none' or 'spikes')",
                            "type": "string"
                        },
                        "report_type": {
                            "description": "Type of report to load for the given node population",
                            "type": "string",
                            "default": "none",
                            "enum": [
                                "none",
                                "spikes",
                                "compartment",
                                "summation",
                                "synapse",
                                "bloodflow_pressure",
                                "bloodflow_speed",
                                "bloodflow_radii"
                            ]
                        },
                        "spike_transition_time": {
                            "description": "When loading a spike report, fade-in/out time [ms], from resting to spike state.",
                            "type": "number",
                            "default": 1,
                            "minimum": 0
                        },
                        "vasculature_geometry_parameters": {
                            "title": "VasculatureGeometrySettings",
                            "description": "Settings for vasculature geometry load (ignored for any non-vasculature population",
                            "type": "object",
                            "default": {},
                            "properties": {
                                "radius_multiplier": {
                                    "description": "Factor to multiply all vasculature sample radii",
                                    "type": "number",
                                    "default": 1,
                                    "minimum": 0.1
                                }
                            },
                            "additionalProperties": false
                        }
                    },
                    "required": [
                        "node_population"
                    ],
                    "additionalProperties": false
                }
            }
        },
        "required": [
            "node_population_settings"
        ],
        "additionalProperties": false
    }

Entrypoints
-----------

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
