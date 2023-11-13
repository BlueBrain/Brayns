.. _apimoleculeexplorer-label:

Molecule Explorer API
=====================

This page references the loaders and entrypoints registered by the Molecule Explorer plugin.

Loaders
-------

protein
~~~~~~~

Can load the following formats: **pdb**, **pdb1**.

This loader does not support loading binary data using 'upload-model'.

.. jsonschema::

    {
        "type": "object",
        "properties": {
            "color_scheme": {
                "description": "Proteins coloring scheme",
                "type": "string",
                "default": "none",
                "enum": [
                    "none",
                    "by_id",
                    "protein_atoms",
                    "protein_chains",
                    "protein_residues"
                ]
            },
            "radius_multiplier": {
                "description": "Protein sample radius multiplier",
                "type": "number",
                "default": 1
            }
        },
        "additionalProperties": false
    }

----

xyz
~~~

Can load the following formats: **xyz**.

This loader supports loading binary data using 'upload-model'.

.. jsonschema::

    {}

Entrypoints
-----------

This plugin does not register any entrypoints.
