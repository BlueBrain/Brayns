.. _launchbraynsbackend-label:

Running a Brayns backend service
================================

For any kind of vizualization, a running instance of Brayns renderer backend
with a websocket server (braynsService) is required to perform the
computationally expensive tasks.

An instance can be started using its executable (braynsService binary) with the
following command line:

.. code-block:: console

    $ braynsService --uri 0.0.0.0:5000

Websocket server
----------------

A brayns instance runs a websocket server if an URI is specified in the command
line and can be monitored from an external application using JSON-RPC messages.

Plugins
-------

Science-specific tasks (loading circuits, etc...) are performed by plugins.
Plugins can be loaded as command line arguments (see :ref:`plugins-label`).

Log level
---------

The log level can also be configured using --log-level followed by either trace,
debug, info, warn, error, critical or off. This is useful for debugging and see
the requests sent from and received by the instance.

.. code-block:: console

    $ braynsService --uri 0.0.0.0:5000 --log-level debug

Examples
--------

Example of commonly used instance:

.. code-block:: console

    $ braynsService --uri 0.0.0.0:5000 --log-level debug --plugin braynsCircuitExplorer

The command line usage of an instance can also be retreived with:

.. code-block:: console

    $ braynsService --help

Running an instance on BB5
--------------------------

On BB5 the binary executable of braynsService is provided through spack.

An instance can be run on a BB5 node with the following steps:

* 1 - Allocate a node on BB5:

.. code-block:: console

    $ salloc --account=<your_project_account> -p interactive -t 8:00:00 --exclusive --constraint=cpu -c 72 --mem 0

Where ``your_project_account`` should be the project account you have access to.

After a successful allocation, an SSH connection with the node is opened
automatically. The node ID is composed of alpha-numeric characters, for example:
``r2i2n11``.

* 2 - Launch the ``braynsService`` backend instance:

To access Brayns binary executable, user must load brayns module using:

.. code-block:: console

    $ module load unstable
    $ module load brayns/latest

And then start Brayns instance as before:

.. code-block:: console
    
    $ braynsService --uri 0.0.0.0:5000 --plugin braynsCircuitExplorer 

The last command will launch a Brayns backend service on port 5000 and any
client from any host can connect to it.

.. hint::

   Brayns, by default, does not understand science-specific models. These
   specific functionalities are loaded using plugins. The ``CircuitExplorer``
   plugin is the main plugin for neuroscientific visualization that you will
   want to load. For further information on plugins, check :ref:`plugins-label`.

Now we know the host (r2i2n11) and port (5000 in this case) where our Brayns
backend is running. We can connect to it through the Python client (here using
uri='r2i2n11:5000').

.. hint::

    If the websocket client (e.g. Python) is running outside BB5, r2i2n11 should
    be replaced by r2i2n11.bbp.epfl.ch.

Now you can interact with the backend instance by using the Python API
:ref:`usepythonapi-label`.
