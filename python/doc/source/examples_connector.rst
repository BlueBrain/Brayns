Connection to a running backend instance
========================================

Now that the Python package is installed and that we saw how to basically
connect to a braynsService instance (backend), we can see more advanced
connection settings.

Connector
---------

The connector is a simple dataclass object (struct) that holds the connection
settings. It is required to be able to connect to an instance and needs at least
the URI of the backend server as seen previously.

Here is an example of connector with more settings:

.. code-block:: python

    import logging

    import brayns

    connector = brayns.Connector(
        uri='localhost:5000',
        ssl_context=brayns.SslClientContext(
            cafile='certification_authority.pem',
        ),
        binary_handler=lambda data: print(len(data)),
        logger=brayns.Logger(logging.INFO),
        max_attempts=10,
        attempt_period=0.2,
    )

    with connector.connect() as instance:
        instance.request('schema', {'endpoint': 'get-version'})

URI
~~~

The URI parameter is used to find the backend instance as mentioned in the
usage section :ref:`usepythonapi-label`.

SSL context
~~~~~~~~~~~

The SSL context depends on the backend SSL settings. If left to None (the
default), the connection will not be secured. The `ssl_context` should be chosen
as follows:

- If the backend is not secure, it can be left to None.
- If the backend certificate is signed by a recognized certification authority
    (CA) installed on the client machine, it can be default constructed.
- If the backend certificate is signed by a custom CA (or self-signed), this CA
    can be set as trusted by setting it in the SSL context (like above).

Binary frames
~~~~~~~~~~~~~

A Brayns instance can stream binary frames (images) on demand. It is mainly used
for the Web client (BraynsCircuitStudio) but they can be handled by a callback
specified in the connector (here it just prints it).

Logger
~~~~~~

The connector and the instance (once connected) log messages that can be useful
for debugging. By default Brayns logs to stdout and only messages with severity
WARN or superior. This level can be changed by passing brayns.Logger as `logger`
to the connector, constructed with the level from logging built-in module.

User can specify a custom logger to redirect the logs or change the level as
long as the logger is a subclass of logging.Logger. Brayns default logger can
be constructed with brayns.Logger(<level>).

Usually for production it is better not to log anything but warnings (default).
However, logging.INFO can be used to trace JSON-RPC messages (useful for
debugging).

For more advanced debug, logging.DEBUG can be used but it prints the requests /
replies content (not only the method and ID) so it can be very verbose.

Attempts
~~~~~~~~

By default, if the connection to the backend fails, `Connector.connect` will
raise an exception and cancel the connection. A new attempts can then safely be
made.

However, it can be convenient to try multiple times (or forever) if the backend
if not available yet. For example if you started a braynsService instance
automatically, you have no way to know when it is ready from the Python and
might want to wait for it in the `connect()` call.

That's why the connector can be configured to try `max_attempts` times to reach
the backend before raising an exception with a delay of `attempt_period` between
each try. If `max_attempts` is set to None, the connector will try to connect
indefinitely until it manages to reach the backend (`attempt_period` is still
taken into account).

.. attention::

    If another connection error occurs than the service being unavailable (SSL
    error, protocol error), retrying is pointless, as the same error will be
    raised again and again. Therefore, these kind of errors are raised
    regardless of `max_attempts` and `attempt_period`.

Instance
--------

Once `connector.connect()` succeeds, a backend instance `brayns.Instance` is
returned. It can be used to send raw JSON-RPC requests to braynsService.

The instance must be disconnected using `instance.disconnect()` once user is
done with it. It can be done automatically using a context manager (like above).

However using the low level JSON-RPC API can be tedious and is subject to
changes, that is why it is better to use high level functionalities provided
as functions and classes by the package (see next sections).
