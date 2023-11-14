.. _apicylindriccamera-label:

Cylindric Camera API
====================

This page references the loaders and entrypoints registered by the Cylindric Camera plugin.

Loaders
-------

This plugin does not register any loaders.

Entrypoints
-----------

get-camera-cylindric
~~~~~~~~~~~~~~~~~~~~

Returns the current camera as cylindric.

**Params**:

This entrypoint has no params, the "params" field can hence be omitted or null.

**Result**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "fovy": {
                "description": "Vertical field of view (in degrees)",
                "type": "number"
            }
        },
        "additionalProperties": false
    }

----

set-camera-cylindric
~~~~~~~~~~~~~~~~~~~~

Sets the current camera to a cylindric one, with the specified parameters.

**Params**:

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "fovy": {
                "description": "Vertical field of view (in degrees)",
                "type": "number"
            }
        },
        "additionalProperties": false
    }

**Result**:

This entrypoint has no result, the "result" field is still present but is always
null.
