.. _usepythonapi-label:

Using Brayns Python API
=======================

After launching the Brayns backend service, we have a renderer backend instance
running a websocket server and we can monitor it using the web API. The Python
package just wraps the websocket client and the JSON-RPC messages into a more
high-level API with classes and functions.

In the following section, we will assume the backend was started with:

.. code-block:: console

    braynsService --uri 0.0.0.0:5000

The port 5000 is used in this documentation but if you want to use another one,
just make sure it is available on the server machine and that you use this one
instead of 5000 it in all the following examples.

The IP mentioned here is from the **SERVER** point of view and specifies which
host can connect to this server. 0.0.0.0 is a wildcard IP address which means
you can connect from any machine as client. Use localhost if you want to allow
only local connections.

Connection to an instance
--------------------------

To create a connection to the backend from the Python API, we need the URI of
the braynsService instance we want to connect to (ip:port). The IP is always the
machine the instance is running on seen from the Python client.

.. hint::

    The Python API is not doing any expensive tasks so it can be run outside
    BB5 on a local machine (useful for debugging).

Here is the URI for typical use cases:

- If Brayns and the Python script are running on the same machine then the URI
    is always **localhost:5000**.
- If Brayns is running on a BB5 node and the Python script is on another BB5
    node, then the URI is **<node ID>:5000** (example r1i1n1:5000) where
    <node ID> is the ID of the node running the backend.
- If Brayns is running on a BB5 node and the Python script outside BB5, then the
    URI is **<node ID>.bbp.epfl.ch:5000**.

So for a renderer running on the node r1i1n1 started with URI 0.0.0.0:5000, the
connection is made as follows from a remote machine:

.. code-block:: python

    # Import installed brayns Python package.
    import brayns

    # Create a connector with connection settings (URI, SSL, logging, etc).
    connector = brayns.Connector('r1i1n1.bbp.epfl.ch:5000')

    # Connect to the server instance, we use a context manager to close the
    # connection properly once we are done with it. It can be done manually
    # with instance.disconnect().
    with connector.connect() as instance:

        # Test by printing the version of the instance we are connected to.
        print(brayns.get_version(instance))

.. attention::

    If you are connecting to a backend running on BB5, it will be necessary to
    be working within EPFL's network, either on-site or through the VPN.

Further information
-------------------

For further information about, please refer to the API reference
:ref:`pythonapi-label` and typical use cases examples :ref:`examples-label`.
