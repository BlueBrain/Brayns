.. _pbrvolumespluginapi-label:

PBR Volumes API
===============

This is the API exposed by the PBR Volumes plugin. This plugin, together with the
``pbrt-v3`` engine, allows for visualization of volumes and participating media
defined with physically based properties.
For this API to be available, the Brayns backend service must be started with the
following parameter:

.. code-block:: console

    --plugin braynsPBRVolumes

----

add_grid_volume
~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    add_grid_volume(scattering, ny, grid_type, g, name,
                    p1, nz, absorption, le, density,
                    nx, p0, scale)

Adds a grid based volume to the scene, in which each cell has its own density

Parameters:

* ``absorption``: ``array``, Absorption spectrum
* ``density``: ``array``, Volume grid density values
* ``g``: ``number``, Anisotropy parameter
* ``grid_type``: ``string``, Type of grid (PBRT-V2 only, values "grid" or "vsd"
* ``le``: ``array``, Radiance spectrum
* ``name``: ``string``, Volume scene name
* ``nx``: ``integer``, Grid X dimension
* ``ny``: ``integer``, Grid Y dimension
* ``nz``: ``integer``, Grid Z dimension
* ``p0``: ``array``, Volume minimum bound
* ``p1``: ``array``, Volume maximum bound
* ``scale``: ``number``, Spectrums scale
* ``scattering``: ``array``, Scattering spectrum

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``message``: ``str``, a description of the error.

----

add_heterogeneus_volume
~~~~~~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    add_heterogeneus_volume(scattering, g, name, p1,
                            absorption, le, max_density,
                            scale, p0, min_density)

Adds a non constant density volume to the scene

Parameters:

* ``absorption``: ``array``, Absorption spectrum
* ``g``: ``number``, Anisotropy parameter
* ``le``: ``array``, Radiance spectrum
* ``max_density``: ``number``, Density maximum value
* ``min_density``: ``number``, Density minimum value
* ``name``: ``string``, Volume scene name
* ``p0``: ``array``, Volume minimum bound
* ``p1``: ``array``, Volume maximum bound
* ``scale``: ``number``, Spectrums scale
* ``scattering``: ``array``, Scattering spectrum

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``message``: ``str``, a description of the error.

----

add_homogeneus_volume
~~~~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    add_homogeneus_volume(scattering, g, name, p1,
                          absorption, le, density, scale, p0)

Adds a constant density volume to the scene

Parameters:

* ``absorption``: ``array``, Absorption spectrum
* ``density``: ``number``, Constant volume density
* ``g``: ``number``, Anisotropy parameter
* ``le``: ``array``, Radiance spectrum
* ``name``: ``string``, Volume scene name
* ``p0``: ``array``, Volume minimum bound
* ``p1``: ``array``, Volume maximum bound
* ``scale``: ``number``, Spectrums scale
* ``scattering``: ``array``, Scattering spectrum

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``message``: ``str``, a description of the error.

----

add_homogeneus_volume_model
~~~~~~~~~~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    add_homogeneus_volume_model(scattering, g, name,
                                absorption, model_id,
                                le, density, scale)

Adds a constant density volume to an already loaded model in the scene

Parameters:

* ``absorption``: ``array``, Absorption spectrum
* ``density``: ``number``, Constant volume density
* ``g``: ``number``, Anisotropy parameter
* ``le``: ``array``, Radiance spectrum
* ``model_id``: ``integer``, Model to which to add the volume
* ``name``: ``string``, Volume scene name
* ``scale``: ``number``, Spectrums scale
* ``scattering``: ``array``, Scattering spectrum

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``message``: ``str``, a description of the error.
