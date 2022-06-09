.. _api-label:

JSON-RPC API
============

This page references the entrypoints of Brayns JSON-RPC API.

The available entrypoints depend on the plugins loaded in the renderer instance and are hence grouped by plugin.

The Core plugin is always loaded as long as the renderer instance is running a websocket server (--uri provided).

All entrypoints use a JSON-RPC protocol (see specifications `here <https://www.jsonrpc.org/specification>`_).

Here are examples of the messages used by Brayns

Request
-------

The request message includes the protocol version, an optional ID, a method name and optional params.

The protocol version must always be 2.0.

The Request ID can be ignored if you don't care about the reply.

The Request ID must be integer or string otherwise.

The method is the name of the entrypoint (for example "get-camera-look-at").

The params can be anything depending on the entrypoint params schema.

.. code-block:: json

    {
        "jsonrpc": "2.0",
        "id": 0,
        "method": "test",
        "params": {
            "field": 123
        }
    }

Reply
-----

The reply is sent only if the request ID is not null nor omitted.

The protocol version must always be 2.0.

The ID is the one of the corresponding request it is replying to.

The result field is always present but can be null if nothing is replied.

The result can be anything depending on the entrypoint result schema.

.. code-block:: json

    {
        "jsonrpc": "2.0",
        "id": 0,
        "result": {
            "another_field": 456
        }
    }

Error
-----

The error message is sent instead of the reply if an error occurs.

The protocol version must always be 2.0.

The ID is the same as it would have been for the reply.

The error field is always present and differentiate it from a reply.

The error field contains a code, a description and optional data.

The additional data is mainly used in case of invalid params to detail the schema error(s) with a list of strings.

.. code-block:: json

    {
        "jsonrpc": "2.0",
        "id": 0,
        "error": {
            "code": 12,
            "message": "Something happened",
            "data": [
                "The field a.b.c is missing."
            ]
        }
    }

Notification
------------

The notification message is used by asynchronous entrypoints to send progress messages.

The protocol version must always be 2.0.

Notifications have no ID by contrast with replies and errors.

Notification params are always progress messages.

Progress messages contain the ID of the request being processed, a description and a progress amount.

The progress amount is a number between 0 and 1.

.. code-block:: json

    {
        "jsonrpc": "2.0",
        "params": {
            "id": 0,
            "operation": "Processing stuff...",
            "amount": 0.5
        }
    }

.. toctree::
    :hidden:

    api_circuitinfo_methods
    api_circuitexplorer_methods
    api_core_methods
