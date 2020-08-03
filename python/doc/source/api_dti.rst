.. _dtipluginapi-label:

DTI API
=======

This is the API exposed by the Diffuse Tensor Imaging plugin. This plugin allows to
load streamlines to be visualized, and attach spike reports to them to run simulations.
For this API to be available, the Brayns backend service must be started with the
following parameter:

.. code-block:: console

    --plugin braynsDTI

----

add_streamlines
~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    add_streamlines(gids, name, opacity, indices, radius, color_scheme, vertices)

Adds a streamline representation to the scene

Parameters:

* ``color_scheme``: ``integer``, Color scheme to draw the streamlines
* ``gids``: ``array``, List of cell GIDs
* ``indices``: ``array``, List of indices
* ``name``: ``string``, Model name
* ``opacity``: ``number``, Color opacity
* ``radius``: ``number``, Streamline tube radius
* ``vertices``: ``array``, List of vertices (3 components per vertex)

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

set_spike_simulation
~~~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    set_spike_simulation(gids, end_time, dt, time_scale, decay_speed, rest_intensity, model_id, spike_intensity, timestamps)

Adds a spike simulation to a model

Parameters:

* ``decay_speed``: ``number``, Speed of spike decay
* ``dt``: ``number``, Simulation time step
* ``end_time``: ``number``, Simulation normalized end time
* ``gids``: ``array``, List of cel GIDs
* ``model_id``: ``integer``, The ID of the loaded model
* ``rest_intensity``: ``number``, Rest intensity
* ``spike_intensity``: ``number``, Spike intensity
* ``time_scale``: ``number``, Time scale
* ``timestamps``: ``array``, List of spike timestamps

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.


----

set_spike_simulation_from_file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Parameters:

.. code-block:: python

    set_spike_simulation_from_file(time_scale, dt, decay_speed, rest_intensity, model_id, path, spike_intensity)

Adds a spike simulation loaded from a file to a model

Parameters:

* ``decay_speed``: ``number``, Speed of spike decay
* ``dt``: ``number``, Simulation time step
* ``model_id``: ``integer``, The ID of the loaded model
* ``path``: ``string``, Path to BlueConfig file
* ``rest_intensity``: ``number``, Rest intensity
* ``spike_intensity``: ``number``, Spike intensity
* ``time_scale``: ``number``, Simulation time scale

Error:

In the event of an error, the return value will be a ``dictionary``, contain 2 entries:

* ``error``: ``integer``, an error code that identifies the problem.
* ``messge``: ``str``, a description of the error.

