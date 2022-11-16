.. _launchbraynsbackend-label:

Running a Brayns backend service
================================

For any kind of vizualization, a running instance of Brayns renderer backend
with a websocket server (braynsService) is required to perform the
computationally expensive tasks.

An instance can be started using its executable (braynsService binary) using
the terminal. A typical command line to start it is the following.

.. code-block:: console

    $ braynsService --uri 0.0.0.0:5000 --plugin braynsCircuitExplorer --plugin braynsAtlasExplorer

It will start a Brayns instance on port 5000 with two plugins to handle circuits
and atlases.

Websocket server
----------------

A brayns instance runs a websocket server if an URI is specified in the command
line and can be monitored from an external application using JSON-RPC messages.

The URI format is always ``<IP>:<PORT>`` where the IP specifies the addresses
which are authorized to connect to the server and PORT the communication port
used by the instance.

Here is an example of server running on port 5000 accepting connections from any
host (wildcard IP).

.. code-block:: console

    $ braynsService --uri 0.0.0.0:5000

Plugins
-------

Plugins can be loaded to perform more than the basic functionalities of Brayns
using the command line.

Here is how to start brayns with the CircuitExplorer plugin:

.. code-block:: console

    $ braynsService --plugin braynsCircuitExplorer

.. hint::

   Brayns, by default, does not understand science-specific models. These
   specific functionalities are loaded using plugins. The ``CircuitExplorer``
   plugin is the main plugin for neuroscientific visualization that you will
   want to load.

For further information on plugins, check :ref:`plugins-label`.

Log level
---------

The instance log level can be configured using --log-level followed by either
trace, debug, info, warn, error, critical or off. This is useful for debugging
and see the requests sent from and received by the instance.

.. code-block:: console

    $ braynsService --log-level debug

Help
----

The command line usage of braynsService can be displayed using:

.. code-block:: console

    $ braynsService --help

Running an instance on BB5
--------------------------

On BB5 the binary executable of braynsService is provided through spack and can
be run with the following steps.

* 1 - Allocate a node on BB5:

.. code-block:: console

    $ salloc --account=<your_project_account> -p interactive -t 8:00:00 --exclusive --constraint=cpu -c 72 --mem 0

Where ``your_project_account`` should be the project account you have access to.

After a successful allocation, an SSH connection with the node is opened
automatically. The node ID is composed of alpha-numeric characters, for example:
``r2i2n11``.

* 2 - Load ``brayns`` module:

To access Brayns binary executable, user must load brayns module using:

.. code-block:: console

    $ module load unstable
    $ module load brayns/latest

* 3 - Launch the ``braynsService`` backend instance:

Now a Brayns instance can be started as before:

.. code-block:: console
    
    $ braynsService --uri 0.0.0.0:5000 --plugin braynsCircuitExplorer 

Now we know the host (r2i2n11) and port (5000 in this case) where our Brayns
backend is running. We can connect to it through the Python client (here using
uri='r2i2n11:5000').

.. hint::

    If the websocket client (e.g. Python script) is running outside BB5, r2i2n11
    should be replaced by r2i2n11.bbp.epfl.ch.

Now you can interact with the backend instance by using the Python API
:ref:`usepythonapi-label`.
