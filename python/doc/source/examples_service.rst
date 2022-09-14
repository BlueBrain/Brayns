Launch Brayns backend from Python
=================================

Service
--------

The ``Service`` class of the Python API can be used to start a braynsService
backend instance programmatically. It is basically a wrapper around the command
line, that is why an executable of braynsService is required to use it.

By default the executable taken by the service is ``braynsService`` so it must
be in the system PATH.

On BB5 running this command line will add ``braynsService`` to the PATH.

.. code-block:: console

    $ module load unstable
    $ module load brayns

On a local machine or without loading brayns module, the executable can still
be specified manually using the ``executable`` parameter of the service.

User can specify the server URI, the log level and some SSL and stream settings
of the braynsService started by the service.

.. code-block:: python

    service = brayns.Service(
        uri='0.0.0.0:5000',
        log_level=brayns.LogLevel.DEBUG,
    )

Process
-------

Use ``start`` to start a new braynsService process. Its behavior is the same
as a backend instance started manually except that it can be monitored from
Python.

Here a value of 100 for ``max_attempts`` is chosen to let some time to the
process to start propertly.

The timeout is approximately ``max_attempts`` * ``attempt_period`` (in seconds)
but it can be disabled with ``max_attempts=None``.

The current logs (stdout and stderr) of a `Process` can be retreived at any
time using the `logs` property of the object.

.. code-block:: python

    connector = brayns.Connector('localhost:5000', max_attempts=None)

    with service.start() as process:

        with connector.connect() as instance:

            print(brayns.get_version(instance))
            print(process.logs)

.. attention::

    Use the context manager to automatically call ``stop`` on the process
    started by the service (or call it manually) otherwise it will never get
    terminated and keep running after your script is finished.

Start a service and connect to it in the same script
----------------------------------------------------

Brayns also provides a helper class to manage both the service and its
connector named ``Manager``. It can be obtained using a ``Service`` and a
``Connector`` with the ``start`` function.

.. code-block:: python

    service = brayns.Service('localhost:5000')
    connector = brayns.Connector('localhost:5000')

    with brayns.start(service, connector) as (process, instance):

        print(brayns.get_version(instance))
        print(process.logs)

.. attention::

    As for the process, use the context manager to automatically call ``stop``
    on the manager or do it manually.
