DTI API methods
---------------

The Brayns python client API is automatically generated when connecting to a
running backend service as shown in :ref:`usepythonclient-label`.

The Core plugin is always loaded so the related entrypoints are always
available.

Other plugins might register additional entrypoints but must be loaded in the
renderer to be available.

The functions below are generated using the entrypoints of the DTI plugin.

All arguments are keyword arguments extracted from the entrypoint params.

Renderer errors will be raised with an instance of brayns.ReplyError. This one
can be used to extract error code, description and some optional additional data
(mainly used to store JSON errors).

Example usage of some generated methods:

.. code-block: python
    import brayns

    with brayns.connect(uri='localhost:5000') as client:
        print(client.get_camera())
        client.set_camera(current='orthographic')
        print(client.get_camera())

add_streamlines
~~~~~~~~~~~~~~~

Add a streamline representation to the scene.

Parameters:

* ``color_scheme``: ``int``. Color scheme to draw the streamlines.
* ``gids``: ``list``. List of cell GIDs.

  * ``items``: ``int``

* ``indices``: ``list``. List of indices.

  * ``items``: ``int``

* ``name``: ``str``. Model name.
* ``opacity``: ``float``. Color opacity.
* ``radius``: ``float``. Streamline tube radius.
* ``vertices``: ``list``. List of vertices (3 components per vertex).

  * ``items``: ``float``

Return value:

This method has no return values.

----

set_spike_simulation
~~~~~~~~~~~~~~~~~~~~

Add a spike simulation to a model.

Parameters:

* ``decay_speed``: ``float``. Speed of spike decay.
* ``dt``: ``float``. Simulation time step.
* ``end_time``: ``float``. Simulation normalized end time.
* ``gids``: ``list``. List of cel GIDs.

  * ``items``: ``int``

* ``model_id``: ``int``. The ID of the loaded model.
* ``rest_intensity``: ``float``. Rest intensity.
* ``spike_intensity``: ``float``. Spike intensity.
* ``time_scale``: ``float``. Time scale.
* ``timestamps``: ``list``. List of spike timestamps.

  * ``items``: ``float``

Return value:

This method has no return values.

----

set_spike_simulation_from_file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Add a spike simulation loaded from a file to a model.

Parameters:

* ``decay_speed``: ``float``. Speed of spike decay.
* ``dt``: ``float``. Simulation time step.
* ``model_id``: ``int``. The ID of the loaded model.
* ``path``: ``str``. Path to BlueConfig file.
* ``rest_intensity``: ``float``. Rest intensity.
* ``spike_intensity``: ``float``. Spike intensity.
* ``time_scale``: ``float``. Simulation time scale.

Return value:

This method has no return values.

