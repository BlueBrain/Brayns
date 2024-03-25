.. _plugins-label:

Brayns plugins
==============

Introduction
------------

Brayns architecture allows it to be used for almost any kind of visualization
purpose. For this reason, the core of Brayns knows nothing about any
science-specific modeling.

To load specific functionalities, the Brayns backend service must be launched with
plugins (using the command line), which will add extra features.

Plugins command line arguments can be chained to load multiple plugins. Plugin
names are the name of the dynamic libraries (.so) implementing it.

.. code-block:: console

    $ braynsService --uri 0.0.0.0:5000 --plugin braynsCircuitExplorer --plugin braynsAtlasExplorer

Here we load plugins by linking dynamically against braynsCircuitExplorer.so
and braynsAtlasExplorer.so (always available).

Brayns comes with built-in plugins that are always available and can be loaded
at runtime.

Circuit Explorer plugin
-----------------------

The Circuit Explorer plugin is the main Brayns plugin for neuroscientific
visualization. It provides Brayns with extra features, such as:

* Load SONATA multi-population networks and simulations.
* Load morphology files (Ascii, SWC or H5).

For further information on the Circuit Explorer's JSON-RPC API, refer to
:ref:`apicircuitexplorer-label`.

To load the Circuit Explorer plugin when launching the Brayns backend service,
use the following argument in the command line:

.. code-block:: console

    --plugin braynsCircuitExplorer

Atlas Explorer plugin
---------------------

The Atlas Explorer plugin is used to visualize atlas data as volumes.

It also adds a loader for NRRD files. For further information on the Atlas
Explorer's JSON-RPC API, refer to :ref:`apiatlasexplorer-label`

To load the Atlas Explorer plugin when launching the Brayns backend service, use
the following argument in the command line:

.. code-block:: console

    --plugin braynsAtlasExplorer
