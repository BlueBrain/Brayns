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

* Loading neuronal circuits based on CircuitConfig / BlueConfig
* Load morphology files (Ascii, SWC or H5)
* Loading simulations in the form of compartment or spike reports
* Load circuit-specific features, such as synapses
* Provides multiple renderers for different kind of visualizations that highlight circuit specific features

The Circuit Explorer plugin also provides a python class within the Brayns python client that
encapsulates its functionality and eases the usage of the API exposed by this plugin. For further information
on the Circuit Explorer's python API, refer to :ref:`circuitexplorerapi-label`

To load the Circuit Explorer plugin when launching the Brayns backend service, use the following argument
in the command line:

.. code-block:: console

    --plugin braynsCircuitExplorer


Circuit Info plugin
-------------------

The Circuit Info plugin provides circuit-specific information. It is used by the brayns web interface
WebBrayns. Nonetheless, it can be used within the python API as well, if desired, to query some circuit
information, such as:

* List of GIDs (all, or from a list of target)
* List of targets
* List of compartment reports, and report-specific information
* Spike report information
* List of projections
* Efferent and afferent synapses for a set of GIDs
* Projected efferent synapses for a set of GIDs and a projection

For information on the Circuit Info python API, refer to :ref:`circuitinfoapi-label`

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

For information on the DTI python API, refer to :ref:`dtipluginapi-label`

To load the DTI plugin when launching the Brayns backend service, use the following argument
in the command line:

.. code-block:: console

    --plugin braynsDTI


Fluorescence plugin
-------------------

The Fluorescence plugin allows for fluorescent staining visualization. It works by sending the Brayns
backend service a specific generated volume that will use to render fluorescent scattering and decaying.

.. attention::

    This plugin requires a specific rendering engine to allow for its usage, the pbrt-v2 engine.

For information on the Fluorescence python API, refer to :ref:`fluorescencepluginapi-label`

To load the Fluorescence plugin when launching the Brayns backend service, use the following arguments in
the command line, which will load the appropiate render engine along the plugin:

.. code-block:: console

    --engine pbrtv2 --plugin braynsFluorescence


Physically based volumes plugin
-------------------------------

The PBR Volumes plugin allows to load volumes into Brayns with physically based properties.

.. attention::

    This plugin requires a specific rendering engine to allow for its usage, the pbrt-v3 engine.

For information on the PBR Volumes python API, refer to :ref:`pbrvolumespluginapi-label`

To load the PBR Volumes plugin when launching the Brayns backend service, use the following arguments in
the command line, which will load the appropiate render engine along the plugin:

.. code-block:: console

    --engine pbrt --plugin braynsPBRVolumes
