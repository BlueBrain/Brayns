.. _launchbraynsbackend-label:

.. |location_link| raw:: html

   <a href="http://webbrayns.ocp.bbp.epfl.ch" target="_blank">WebBrayns</a>

Running a Brayns backend service
================================

As a Brayns client, to run any script that uses the Brayns python client API you must have access to a
running Brayns server backend. To launch a Brayns backend:

* 1 - Get an allocation on BB5:

.. code-block:: console

    $ salloc --account=<your project account> --partition=prod --exclusive --constraint=cpu -c 72 --mem 0

Where ``your_project_account`` should be the project account you have access to on BB5.
After a successful allocation, you will be shown with the node ID or host of your allocation. It is composed of alpha-numeric
characters, for example: ``r2i2n11``. You will need it to connect to the ``braynsService`` from your python client.

* 2 - Launch the ``braynsService`` server:

.. code-block:: console

    $ module load unstable
    $ module load brayns/latest
    $ ./braynsService --uri 0.0.0.0:5000 --plugin braynsCircuitExplorer --plugin braynsCircuitInfo --plugin braynsDTI

The last command will launch a Brayns backend service with remote access needed for the python client (``--uri``)
on port 5000, and enabling the Circuit Explorer, Circuit Info and DTI plugins

.. hint::

   Brayns, by default, does not understand science-specific models. The specific functionality is loaded using plugins.
   The ``CircuitExplorer`` plugin is the main plugin for neuroscientific visualization that you will want to load.
   For further information on plugins, check :ref:`plugins-label`.

Now we know the host (node ID) and port (5000 in this case) where our Brayns backend is running. We will need both to
connect to it through the Python client.
