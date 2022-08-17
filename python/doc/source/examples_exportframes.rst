Export frames
=============

When a model has a simulation attached, we might want to render multiple frames
to make a movie from them. To avoid recreating a snapshot context in the backend
for each image, a frame export allows to create the context once and use it
to take multiple snapshots with different parameters.

Simulation
----------

As previously mentioned, the simulation settings are global in Brayns, which
means we have one single state for the simulation in an instance.

.. code-block:: python

    simulation = brayns.get_simulation(instance)

    print(simulation.start_frame)
    print(simulation.end_frame)
    print(simulation.current_frame)
    print(simulation.delta_time)

This global state is automatically updated each time we load a model with a
simulation attached.

The time unit is always milliseconds but a convenience attribute `time_unit` is
in the simulation to convert it in seconds. All time values in simulation are
expressed in `time_unit` except when it is explicitely stated being in seconds.

`Simulation` also provides convenience method to convert frame indices to
timestamp and vice-versa.

Frame indices
-------------

If we use the parameters set when we took a snapshot, we will need the indices
of the frames we want to export.

Usually, we just know the start and end frame we want to render, the movie FPS
(frames per second) and possibly how much slower we want the movie. We can use a
helper class `MovieFrames` to generate the indices from these data.

.. code-block:: python

    frames = brayns.MovieFrames(
        fps=25,
        slowing_factor=1,
        start_frame=0,
        end_frame=-1,
    )

    indices = frames.get_indices(simulation)

.. attention::

    This class takes a regular time step between frames so if the movie FPS are
    not a multiple of simulation FPS, the movie time might not be accurate. The
    formula is the following:

    step = int(simulation_fps / movie_fps / slowing_factor)

View
----

As we render multiple frames, we can have one view per frame. This is specified
using a list `KeyFrame` objects, each having a frame index and a camera view.
Here we will suppose it is the same for all (static camera).

.. code-block:: python

    key_frames = brayns.KeyFrame.from_indices(indices, view)

Exporter
--------

The exporter class can render frames into a given folder. All export parameters
(resolution, camera, renderer, format, key frames) can be specified in its
constructor.

.. code-block:: python

    # Frame exporter specifications.
    exporter = brayns.FrameExporter(
        frames=key_frames,
        format=brayns.ImageFormat.PNG,
        resolution=brayns.Resolution.full_hd,
        camera=camera,
        renderer=renderer,
    )

    # Export to given folder.
    exporter.export_frames(instance, 'path/to/frames')

.. attention::

    You need to remember the movie FPS you set if you want to make a movie from
    the exported frames.
