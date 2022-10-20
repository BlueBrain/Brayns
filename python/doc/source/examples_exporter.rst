Export frames
=============

When a model has a simulation attached, we might want to render multiple frames
to make a movie from them. To avoid recreating a snapshot context in the backend
for each image, a frame export allows us to create the context once and use it
to take multiple snapshots more efficiently.

Simulation
----------

The simulation settings are global in Brayns, which means we have one single
state for the simulation in an instance.

.. code-block:: python

    simulation = brayns.get_simulation(instance)

    print(simulation.start_frame)
    print(simulation.end_frame)
    print(simulation.current_frame)
    print(simulation.delta_time)

This global state is automatically updated each time we load a model with a
simulation attached.

The time unit is always milliseconds but a convenience attribute ``time_unit`` is
in the simulation to convert it in seconds. All time values in simulation are
expressed in ``time_unit`` except when it is explicitely stated being in seconds.

``Simulation`` also provides convenience method to convert frame indices to
timestamp and vice-versa.

Frame indices
-------------

First we have to specify the indices of the simulation frames we want to export.

Usually, we just know the start and end frame we want to render, the movie FPS
(frames per second) and possibly how much slower we want the movie.

To generate frame indices from these informations, We can use a helper class
named ``MovieFrames``.

.. code-block:: python

    frames = brayns.MovieFrames(
        fps=25,
        slowing_factor=1,
        start_frame=0,
        end_frame=-1,
    )

    indices = frames.get_indices(simulation)

.. attention::

    The index step is computed with the following formula:

    step = simulation_fps / movie_fps / slowing_factor
    
    If the step is smaller than 1, some frames will be duplicated to match the
    target FPS.

.. attention::

    You need to remember the movie FPS you set if you want to make a movie from
    the exported frames.

View
----

As we render multiple frames, we can have one view per frame. This is specified
using a list ``KeyFrame`` objects, each having a frame index and a camera view.

Here we will suppose the view is the same for all frames (static camera).

.. code-block:: python

    key_frames = brayns.KeyFrame.from_indices(indices, camera.view)

Color ramp
----------

The color ramp is an object attached to models with a simulation. It allows to
map a value range (usually voltages) to a color range (list of colors).

This part is optional as Brayns build a default color ramp for circuits.

.. code-block:: python

    # Get the color ramp of the model.
    ramp = brayns.get_color_ramp(instance, model.id)

    # Choose a range of simulation value (here -80mV to +10mV).
    ramp.value_range = brayns.ValueRange(-80, 10)

    # Choose colors:
    # Simulation values of -80 and below will be red.
    # Simulation values around -35 (middle of value range) will be green.
    # Simulation values of +10 and above will be blue.
    # Between these values, the color is linearly interpolated (mixed).
    colors = [
        brayns.Color3.red,
        brayns.Color3.green,
        brayns.Color3.blue,
    ]

    # Optional opacity curve for complex alpha channels based on the value range:
    # From 0% to 50% alpha is 0 (transparent). 
    # From 50% to 100% alpha is mixed from 0 to 1.
    # Mixing is done using a linear interpolation.
    # Here at 50% alpha is 0 and at 100% it is 1.
    curve = brayns.OpacityCurve([
        brayns.ControlPoint(0.0, 0.0),
        brayns.ControlPoint(0.5, 0.0),
        brayns.ControlPoint(1.0, 1.0),
    ])

    # Generate the colors with RGB and opacity curve.
    ramp.colors = curve.apply(colors)

    # Update the color ramp.
    brayns.set_color_ramp(instance, model.id, ramp)

Frame exporter
--------------

The exporter class can render frames into a given folder. All export parameters
(resolution, camera, renderer, format, key frames) can be specified in its
constructor.

.. code-block:: python

    # Frame export specifications (using some parameters from snapshot).
    exporter = brayns.Exporter(
        frames=key_frames,
        format=brayns.ImageFormat.PNG,
        resolution=resolution,
        projection=camera.projection,
        renderer=brayns.InteractiveRenderer(),
    )

    # Export to given folder.
    exporter.export_frames(instance, 'path/to/frames')
