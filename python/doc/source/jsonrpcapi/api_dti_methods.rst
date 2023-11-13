.. _apidti-label:

DTI API
=======

This page references the loaders and entrypoints registered by the DTI plugin.

Loaders
-------

DTI loader
~~~~~~~~~~

Can load the following formats: **json**.

This loader does not support loading binary data using 'upload-model'.

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "radius": {
                "description": "Connectivity streamlines radius",
                "type": "number"
            },
            "spike_decay_time": {
                "description": "Time [ms], that a spikes takes to go from the beginning to the end of the streamline",
                "type": "number",
                "default": 1,
                "minimum": 0
            }
        },
        "required": [
            "radius"
        ],
        "additionalProperties": false
    }

Entrypoints
-----------

This plugin does not register any entrypoints.
