Launch Brayns backend from Python
=================================

Launcher
--------

The `Launcher` class of the Python API can be used to start a braynsService
backend instance programmatically. It is basically a wrapper around the command
line, that is why an executable of braynsService is required to use it.

By default the executable taken by the launcher is `braynsService` so it must
be in the system PATH.

On BB5 running the following command line will add `braynsService` to the PATH.

.. code-block:: console

    $ module load unstable
    $ module load brayns

On a local machine or without loading brayns module, the executable can still
be specified manually using the `executable` parameter of the launcher.

User can specify the server URI, the log level and some SSL and stream settings
of the braynsService started by the launcher.

.. code-block:: python

    launcher = brayns.Launcher(
        uri='0.0.0.0:5000',
        log_level=brayns.LogLevel.DEBUG,
    )

Process
-------

Use `launch` to start a new braynsService process. Its behavior is the same
as a backend instance started manually except that it can be monitored from
Python.

A value of None for `max_attempts` should be chosen as you have no way to know
when the server will be ready. It will make the connector try in loop to connect
to the instance until it is available.

The current logs (stdout and stderr) of a `Process` can be retreived at any time
using the `logs` property of the object.

.. code-block:: python

    connector = brayns.Connector('localhost:5000')

    with launcher.start() as process:

        with connector.connect(max_attempts=None) as instance:

            print(brayns.get_version(instance))
            print(process.logs)

.. attention::

    Use the context manager to automatically call `terminate` on the process
    started by the launcher (or call it manually) otherwise it will never get
    terminated and will keep running after your script is finished (locking the
    server port).
