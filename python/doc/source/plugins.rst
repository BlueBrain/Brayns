.. _plugins-label:

Brayns plugins
==============

Introduction
------------

Brayns architecture allows it to be used for almost any kind of visualization
purpose. For this reason, the core of Brayns knows nowthing about any
science-specific modeling.

To load specific functionality, the Brayns backend service must be launched with
the command to load specifiying plugins, which will add extra functionality.
Plugins command line arguments can be chained to load multiple plugins. Plugin
names are the name of the dynamic libraries (.so) implementing it.

.. code-block:: console

    $ braynsService --uri 0.0.0.0:5000 --plugin braynsCircuitExplorer --plugin braynsDTI

Brayns comes with built-in plugins that are always available and can be loaded
at runtime.

Circuit Explorer plugin
-----------------------

The Circuit Explorer plugin is the main Brayns plugin for neuroscientific
visualization. It provides Brayns with extra features, such as:

* Load neuronal circuits and simulations based on CircuitConfig / BlueConfig.
* Load SONATA multi-population networks and simulations.
* Load morphology files (Ascii, SWC or H5).
* Load BlueConfig/SONATA multi-population hybrid circuits.

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

Circuit Info plugin
-------------------

The Circuit Info plugin provides circuit-specific information. It is used by the
brayns web interface WebBrayns. It can be used within the JSON-RPC API to query
some circuit information, such as:

* List of GIDs (all, or from a list of target).
* List of targets.
* List of compartment reports, and report-specific information.
* Spike report information.
* List of projections.
* Efferent and afferent synapses for a set of GIDs.
* Projected efferent synapses for a set of GIDs and a projection.

For information on the Circuit Info JSON-RPC API, refer to
:ref:`apicircuitinfo-label`.

To load the Circuit Info plugin when launching the Brayns backend service, use the following argument
in the command line:

.. code-block:: console

    --plugin braynsCircuitInfo

.. attention::

    This plugin is deprecated and will be removed soon. Use bluepy to query
    information about circuits.

Diffusion Tensor Imaging plugin (DTI)
-------------------------------------

The DTI plugin adds functionality to load streamlines into Brayns, as well as to
attach a spike report simulation to them to be visualized.

To load the DTI plugin when launching the Brayns backend service, use the
following argument in the command line:

.. code-block:: console

    --plugin braynsDTI

Cylindric camera plugin
-----------------------

This is a simple plugin to add support for cylindric camera. This camera fixes
the distorsion of curved screen (i.e. OpenDeck).

.. code-block:: console

    --plugin braynsCylindricCamera
