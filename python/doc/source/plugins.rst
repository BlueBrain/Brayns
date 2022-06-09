.. _plugins-label:

Brayns plugins
==============

Introduction
------------

Brayns architecture allows it to be used for almost any kind of visualization
purpose. For this reason, the core of Brayns knows nowthing about any science-specific
modeling.

To load specific functionality, the Brayns backend service must be launched by specifiying
plugins, which will add extra functionality.

Circuit Explorer plugin
-----------------------

The Circuit Explorer plugin is the main Brayns plugin for neuroscientific visualization. It
provides Brayns with extra features, such as, but not limited to:

* Loading neuronal circuits and simulations based on CircuitConfig / BlueConfig
* Loading SONATA multi-population networks and simulations
* Load morphology files (Ascii, SWC or H5)
* Load BlueConfig/SONATA multi-population hybrid circuits

For further information on the Circuit Explorer's JSON-RPC API, refer to :ref:`apicircuitexplorer-label`

To load the Circuit Explorer plugin when launching the Brayns backend service, use the following argument
in the command line:

.. code-block:: console

    --plugin braynsCircuitExplorer


Atlas Explorer plugin
---------------------

The Atlas Explorer plugin is used to visualize atlas data as volumes.

It also adds a loader for NRRD files.

For further information on the Atlas Explorer's JSON-RPC API, refer to :ref:`apiatlasexplorer-label`

To load the Atlas Explorer plugin when launching the Brayns backend service, use the following argument
in the command line:

.. code-block:: console

    --plugin braynsAtlasExplorer


Circuit Info plugin
-------------------

The Circuit Info plugin provides circuit-specific information. It is used by the brayns web interface
WebBrayns. It can be used within the JSON-RPC API to query some circuit information, such as:

* List of GIDs (all, or from a list of target)
* List of targets
* List of compartment reports, and report-specific information
* Spike report information
* List of projections
* Efferent and afferent synapses for a set of GIDs
* Projected efferent synapses for a set of GIDs and a projection

For information on the Circuit Info JSON-RPC API, refer to :ref:`apicircuitinfo-label`

To load the Circuit Info plugin when launching the Brayns backend service, use the following argument
in the command line:

.. code-block:: console

    --plugin braynsCircuitInfo


Diffusion Tensor Imaging plugin
-------------------------------

The DTI plugin adds functionality to load streamlines into Brayns, as well as to attach a spike
report simulation to them to be visualized.

This plugin accept a file with extension ``.dti``, with the following format:

.. code-block::

    streamlines=<path to streamlines file .row>
    gids_to_streamline_row=<path to GID to streamlines file row mapping .mapping>

The ``streamlines`` property points to a file with all the streamlines to load. Each row
corresponds to a streamline defined by a set of points, with the following format:

.. code-block::

    <number of points> <point 1 x> <point 1 y> <point 1 z> <point 2 x> <point 2 y> ...

The ``gids_to_streamline_row`` property points to a file that maps cell GIDs to rows to in the
``streamlines`` file, with the following format:

.. code-block::

    <GID> <row_index>

To load the DTI plugin when launching the Brayns backend service, use the following argument
in the command line:

.. code-block:: console

    --plugin braynsDTI

