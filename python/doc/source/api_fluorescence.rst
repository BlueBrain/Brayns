.. _fluorescencepluginapi-label:

Fluorescence API
================

This is the API exposed by the Fluorescence plugin. This plugin, together with the
``pbrt-v2`` engine, allows for visualization of fluorescent scattering and decying.
For this API to be available, the Brayns backend service must be started with the
following parameter:

.. code-block:: console

    --plugin braynsFluorescence

----

add_disk_sensor
~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    add_disk_sensor(phi, xpixels, inner_radius, height,
                    radius, rotation, ypixels, scale,
                    reference, fov, translation)

Adds a disk shaped sensor to the scene

Parameters:

* ``fov``: ``number``, Sensor capture field of view
* ``height``: ``number``, Sensor disk height
* ``inner_radius``: ``number``, Sensor disk inner radius
* ``phi``: ``number``, Sensor available surface (Max = 2*PI)
* ``radius``: ``number``, Sensor disk radius
* ``reference``: ``string``, Sensor name
* ``rotation``: ``array``, Sensor 3D Rotation
* ``scale``: ``array``, Sensor 3D Scale
* ``translation``: ``array``, Sensor 3D Translation
* ``xpixels``: ``integer``, Sensor pixel capture width
* ``ypixels``: ``integer``, Sensor pixel capture height

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

----

add_rectangle_sensor
~~~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    add_rectangle_sensor(xpixels, x, y, height, rotation,
                         ypixels, scale, reference,
                         fov, translation)

Adds a rectangle shaped sensor to the scene

Parameters:

* ``fov``: ``number``, Sensor capture field of view
* ``height``: ``number``, Sensor rectangle height
* ``reference``: ``string``, Sensor name
* ``rotation``: ``array``, Sensor 3D Rotation
* ``scale``: ``array``, Sensor 3D Scale
* ``translation``: ``array``, Sensor 3D Translation
* ``x``: ``number``, Sensor rectangle width
* ``xpixels``: ``integer``, Sensor pixel capture width
* ``y``: ``number``, Sensor rectangle height
* ``ypixels``: ``integer``, Sensor pixel capture height

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

----

add_fluorescent_ann_volume
~~~~~~~~~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    add_fluorescent_ann_volume(fexs, gfs, ntags, g, name,
                               p1, cs, fems, epsilons, p0,
                               prefix, yields)

Adds an annotated fluorescent volume to the scene

Parameters:

* ``cs``: ``array``, Fluorescence concentrations
* ``epsilons``: ``array``, Fluorescence epsilon tresholds
* ``fems``: ``array``, Fluorescence emissive spectrums
* ``fexs``: ``array``, Fluorescence exictatory spectrums
* ``g``: ``number``, Anisotropy parameter
* ``gfs``: ``array``, Fluorescence anisotropy parameters
* ``name``: ``string``, Volume scene name
* ``ntags``: ``integer``, Number of tags
* ``p0``: ``array``, Minimum axis-aligned volume bound
* ``p1``: ``array``, Maximum axis-aligned volume bound
* ``prefix``: ``string``, Path to volume density file
* ``yields``: ``array``, Fluorescence quantum yields

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

add_fluorescent_bin_volume
~~~~~~~~~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    add_fluorescent_bin_volume(epsilon, yield_v, g, name,
                               p1, fem, c, fex, prefix, p0,
                               gf)

Adds a constant density binary fluorescent volume to the scene

Parameters:

* ``c``: ``number``, Fluorescence concentration
* ``epsilon``: ``number``, Fluorescence epsilon treshold
* ``fem``: ``array``, Fluorescence emissive spectrum
* ``fex``: ``array``, Fluorescence exictatory spectrum
* ``g``: ``number``, Anisotropy parameter
* ``gf``: ``number``, Fluorescence anisotropy parameter
* ``name``: ``string``, Volume scene name
* ``p0``: ``array``, Minimum axis-aligned volume bound
* ``p1``: ``array``, Maximum axis-aligned volume bound
* ``prefix``: ``string``, Path to the volume density file
* ``yield_v``: ``number``, Fluorescence quantum yield

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

add_fluorescent_grid_file_volume
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    add_fluorescent_grid_file_volume(epsilon, yield_v, g,
                                     name, p1, fem, c, fex,
                                     prefix, p0, gf)

Adds a fluorescent grid volume, whose density values are loaded from a file on disk

Parameters:

* ``c``: ``number``, Fluorescence concentration
* ``epsilon``: ``number``, Fluorescence epsilon treshold
* ``fem``: ``array``, Fluorescence emissive spectrum
* ``fex``: ``array``, Fluorescence excitatory spectrum
* ``g``: ``number``, Anisotropy parameter
* ``gf``: ``number``, Fluorescence anysotropy parameter
* ``name``: ``string``, Volume scene name
* ``p0``: ``array``, Minimum axis-aligned volume bound
* ``p1``: ``array``, Maximum axis-aligned volume bound
* ``prefix``: ``string``, Path to the volume density file
* ``yield_v``: ``number``, Fluorescence quantum yield

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

add_fluorescent_grid_volume
~~~~~~~~~~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    add_fluorescent_grid_volume(epsilon, ny, yield_v, g,
                                name, p1, fem, nz, c, fex,
                                density, nx, p0, gf)

Adds a fluorescent grid volume, whose density values are specified in the request

Parameters:

* ``c``: ``number``, Fluorescence concentration
* ``density``: ``array``, Density grid value
* ``epsilon``: ``number``, Fluorescence epsilon treshold
* ``fem``: ``array``, Fluorescence emissive spectrum
* ``fex``: ``array``, Fluorescence exictatory spectrum
* ``g``: ``number``, Anisotropy parameter
* ``gf``: ``number``, Fluorescence anisotropy parameter
* ``name``: ``string``, Volume scene name
* ``nx``: ``integer``, Gird x dimension
* ``ny``: ``integer``, Grid y dimension
* ``nz``: ``integer``, Grid z dimension
* ``p0``: ``array``, Minimum axis-aligned volume bound
* ``p1``: ``array``, Maximum axis-aligned volume bound
* ``yield_v``: ``number``, Fluorescence quantum yield

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

add_fluorescent_scatter_volume
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    add_fluorescent_scatter_volume(mweight, scattering, epsilon, yield_v,
                                   g, name, fscale, fem, p1, absorption,
                                   c, sscale, fex, le, density, p0, gf)

Adds a constant density fluorescent scattering volume to the scene

Parameters:

* ``absorption``: ``array``, Absoption spectrum
* ``c``: ``number``, Fluorescence concentration
* ``density``: ``number``, Constant volume density
* ``epsilon``: ``number``, Fluorescence epsilon treshold
* ``fem``: ``array``, Fluorescence emissive spectrum
* ``fex``: ``array``, Fluorescence excitatory spectrum
* ``fscale``: ``number``, Fluorescence scaling
* ``g``: ``number``, Anisotropy parameter
* ``gf``: ``number``, Fluorescence anisotropy parameter
* ``le``: ``array``, Radiance spectrum
* ``mweight``: ``number``, Weight
* ``name``: ``string``, Volume scene name
* ``p0``: ``array``, Minimum axis-aligned volume bound
* ``p1``: ``array``, Maximum axis-aligned volume bound
* ``scattering``: ``array``, Scattering spectrum
* ``sscale``: ``number``, Scattering scaling
* ``yield_v``: ``number``, Fluorescence quantum yield

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

add_fluorescent_scattergrid_volume
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    add_fluorescent_scattergrid_volume(mweight, scattering, epsilon,
                                       ny, yield_v, g, name, fscale,
                                       fem, nz, absorption, c, p1,
                                       sscale, fex, le, density, nx,
                                       p0, gf)

Adds a fluorescent grid scattering volume to the scene

Parameters:

* ``absorption``: ``array``, Absoption spectrum
* ``c``: ``number``, Fluorescence concentration
* ``density``: ``array``, Grid density values
* ``epsilon``: ``number``, Fluorescence epsilon treshold
* ``fem``: ``array``, Fluorescence emissive spectrum
* ``fex``: ``array``, Fluorescence excitatory spectrum
* ``fscale``: ``number``, Fluorescence scaling
* ``g``: ``number``, Anisotropy parameter
* ``gf``: ``number``, Fluorescence anisotropy parameter
* ``le``: ``array``, Radiance spectrum
* ``mweight``: ``number``, Weight
* ``name``: ``string``, Volume scene name
* ``nx``: ``integer``, Gird x dimension
* ``ny``: ``integer``, Grid y dimension
* ``nz``: ``integer``, Grid z dimension
* ``p0``: ``array``, Minimum axis-aligned volume bound
* ``p1``: ``array``, Maximum axis-aligned volume bound
* ``scattering``: ``array``, Scattering spectrum
* ``sscale``: ``number``, Scattering scaling
* ``yield_v``: ``number``, Fluorescence quantum yield

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

add_fluorescent_scattergridfile_volume
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    add_fluorescent_scattergridfile_volume(mweight, scattering, epsilon,
                                           yield_v, g, name, fscale, fem,
                                           p1, absorption, c, sscale, fex,
                                           le, prefix, p0, gf)

Adds a fluorescent scattering volume to the scene, whose values are loaded from a file on disk

Parameters:

* ``absorption``: ``array``, Absoption spectrum
* ``c``: ``number``, Fluorescence concentration
* ``epsilon``: ``number``, Fluorescence epsilon treshold
* ``fem``: ``array``, Fluorescence emissive spectrum
* ``fex``: ``array``, Fluorescence excitatory spectrum
* ``fscale``: ``number``, Fluorescence scaling
* ``g``: ``number``, Anisotropy parameter
* ``gf``: ``number``, Fluorescence anisotropy parameter
* ``le``: ``array``, Radiance spectrum
* ``mweight``: ``number``, Weight
* ``name``: ``string``, Volume scene name
* ``p0``: ``array``, Minimum axis-aligned volume bound
* ``p1``: ``array``, Maximum axis-aligned volume bound
* ``prefix``: ``string``, Path to volume density file
* ``scattering``: ``array``, Scattering spectrum
* ``sscale``: ``number``, Scattering scaling
* ``yield_v``: ``number``, Fluorescence quantum yield

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

add_fluorescent_volume
~~~~~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    add_fluorescent_volume(epsilon, yield_v, name, p1, fem, c, fex, p0, gf)

Adds a constant density fluorescent volume to the scene

Parameters:

* ``c``: ``number``, Fluorescence concentration
* ``epsilon``: ``number``, Fluorescence epsilon treshold
* ``fem``: ``array``, Fluorescence emissive spectrum
* ``fex``: ``array``, Fluorescence excitatory spectrum
* ``gf``: ``number``, Fluorescence anisotropy parameter
* ``name``: ``string``, Volume scene name
* ``p0``: ``array``, Minimum axis-aligned volume bound
* ``p1``: ``array``, Maximum axis-aligned volume bound
* ``yield_v``: ``number``, Fluorescence quantum yield

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

