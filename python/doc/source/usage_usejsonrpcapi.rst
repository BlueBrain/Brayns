.. _usejsonrpcapi-label:

Using Brayns JSON-RPC API
=========================

This section is useful only if you plan to use your own websocket client to
monitor your Brayns instance. The Python API hides the websocket communication
so if you are using it, you can skip this section.

The available entrypoints depend on the plugins loaded in the instance and are
hence grouped by plugin. The Core plugin is always loaded as long as the backend
instance is running a websocket server (--uri provided).

All entrypoints use a JSON-RPC protocol (see specifications
`here <https://www.jsonrpc.org/specification>`_).

The schemas of the messages used by Brayns have the following rules.

Websocket
---------

The JSON-RPC messages are send using websockets.

The connection can be made using the URL ``ws://HOST:PORT`` if the connection is
not secure or ``wss://HOST:PORT`` with SSL.

.. attention::
    Brayns doesn't implement ping messages so be careful if your websocket API
    uses a ping interval, it must be disabled.

Request
-------

The request message includes the protocol version, an optional ID, a method name
and optional params.

- The protocol version must always be 2.0.
- The Request ID can be ignored if you don't care about the reply.
- The Request ID must be integer or string if a reply is expected.
- The method is the name of the entrypoint (for example "snapshot").

The params can be anything depending on the entrypoint params schema.

.. code-block:: json

    {{
        "jsonrpc": "2.0",
        "id": 0,
        "method": "test",
        "params": {{
            "field": 123
        }}
    }}

Reply
-----

The reply is sent by the instance only if the request has a non-null ID.

- The protocol version must always be 2.0.
- The ID is the one of the corresponding request it is replying to.
- The result field is always present but can be null depending on the schema.
- The result can be anything depending on the entrypoint result schema.

.. code-block:: json

    {{
        "jsonrpc": "2.0",
        "id": 0,
        "result": {{
            "another_field": 456
        }}
    }}

Error
-----

The error message is sent instead of the reply if an error occurs.

- The protocol version must always be 2.0.
- The ID is the same as it would have been for the reply.
- The error field is always present and differentiate it from a reply.
- The error field contains a code, a description and optional data.
- The additional data are mainly used in case of invalid params to detail the
    schema error(s) with a list of strings.

.. code-block:: json

    {{
        "jsonrpc": "2.0",
        "id": 0,
        "error": {{
            "code": 12,
            "message": "Something happened",
            "data": [
                "The field a.b.c is missing."
            ]
        }}
    }}

Notification
------------

The notification message is used by asynchronous entrypoints to send progress
messages.

- The protocol version must always be 2.0.
- Notifications have no ID by contrast with replies and errors.
- Notification params are always progress messages.
- Progress messages contain the ID of the request being processed, a description
    and a progress amount between 0 and 1.

.. code-block:: json

    {{
        "jsonrpc": "2.0",
        "params": {{
            "id": 0,
            "operation": "Processing stuff...",
            "amount": 0.5
        }}
    }}

.. hint::

    JSON-RPC messages received from an instance are differentiated as follows:
    - If the message has a "result" field, then it is a reply.
    - If the message has an "error" field, then it is an error.
    - If the message has a "params" field, then it is a notification.
    - Otherwise the message is invalid and should be ignored.

Further information
-------------------

For further information about, please refer to the API reference
:ref:`jsonrpcapi-label`.
