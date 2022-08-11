.. _launchbraynsbackend-label:

.. |location_link| raw:: html

   <a href="http://webbrayns.ocp.bbp.epfl.ch" target="_blank">WebBrayns</a>

Running a Brayns backend service
================================

For any kind of vizualization, a running instance of Brayns renderer backend is required.

This one has to run a websocket server on an URI chosen by use (ie ip:port) to be able to interact with the user.

An instance can be started using its executable (braynsService) with the following command line:

.. code-block:: console

    $ braynsService --uri 0.0.0.0:5000

To be able to load circuits, additional plugins must be loaded using keyword arguments (see :ref:`plugins-label`).

The log level can also be configured using --log-level followed by either trace, debug, info, warn, error, critical or off.

Example of commonly used instance:

.. code-block:: console

    $ braynsService --uri 0.0.0.0:5000 -log-level debug --plugin braynsCircuitExplorer --plugin braynsDTI

Running an instance on BB5
--------------------------

On BB5 the binary executable of Brayns is provided through spack and can be started like follows:

* 1 - Allocate a node on BB5:

.. code-block:: console

    $ salloc --account=proj3 -p interactive -t 8:00:00 --exclusive --constraint=cpu -c 72 --mem 0

Where ``your_project_account`` should be the project account you have access to on BB5.

After a successful allocation, you will be shown with the node ID or host of your allocation. It is composed of alpha-numeric
characters, for example: ``r2i2n11``.

You will need it to connect to the ``braynsService`` (here URI from client side will be r2i2n11:5000).

Note: 0.0.0.0 is a wildcard IP address, meaning that clients from any IP address will be able to connect to the instance.

Use localhost to restrict it to the local node / machine.

* 2 - Launch the ``braynsService`` server:

To access Brayns binary executable, user must load brayns module using:

.. code-block:: console

    $ module load unstable
    $ module load brayns/latest

And then start Brayns instance as before:

.. code-block:: console
    
    $ braynsService --uri 0.0.0.0:5000 --plugin braynsCircuitExplorer 

The last command will launch a Brayns backend service with remote access needed for the client (``--uri``)
on port 5000, and enabling the Circuit Explorer plugin.

.. hint::

   Brayns, by default, does not understand science-specific models. The specific functionality is loaded using plugins.
   The ``CircuitExplorer`` plugin is the main plugin for neuroscientific visualization that you will want to load.
   For further information on plugins, check :ref:`plugins-label`.

Now we know the host (r2i2n11) and port (5000 in this case) where our Brayns backend is running. We will need both to
connect to it through the Python client (here using uri='r2i2n11:5000').

Note: if the websocket client (e.g. Python) is running outside BB5, r2i2n11 should be replaced by r2i2n11.bbp.epfl.ch.

Now you can interact with the backend instance by using the Python API :ref:`_usepythonapi-label`.
