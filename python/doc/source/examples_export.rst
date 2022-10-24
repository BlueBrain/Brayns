Export frames
=============

When a model has a simulation attached, we might want to render multiple frames
to make a movie from them.

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

    # Movie duration settings.
    frames = brayns.MovieFrames(
        fps=25,
        slowing_factor=1,
        start_frame=0,
        end_frame=-1,
    )

    # Select frame indices from instance simulation settings.
    indices = frames.get_indices(simulation)

.. attention::

    The index step is computed with the following formula:

    step = simulation_fps / movie_fps / slowing_factor
    
    If the step is smaller than 1, some frames will be duplicated to match the
    target FPS.

.. attention::

    You need to remember the movie FPS you set if you want to make a movie from
    the exported frames.

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

Export
------

To make a movie from exported frames (snapshots or images), we will have to name
the filenames using a pattern which allows FFMPEG to extract the frame index
from the name (%d C-style printf integer formatting).

.. code-block:: python

    import pathlib

    # Choose a folder to export frames.
    folder = pathlib.Path('frames')

    # Create it if not exists.
    folder.mkdir(exist_ok=True)

    # Naming pattern: image-1.png, image-2.png, ...
    filename = 'image-%d.png'

    # Full path pattern.
    pattern = str(folder / 'image-%d.png')

Now we can render our frames using either ``Image`` or ``Snapshot``. 

.. code-block:: python

    # With Image (updating instance context).

    # Update instance state.
    brayns.set_resolution(instance, resolution)
    brayns.set_camera(instance, camera)
    brayns.set_renderer(instance, renderer)

    # Image settings (accumulate by default)
    image = brayns.Image()

    # Render images using pattern and updating simulation.
    for index, frame in enumerate(indices):
        brayns.set_simulation_frame(instance, frame)
        image.save(instance, pattern % index)
    
    # OR with Snapshot (using different camera, renderer, etc...).

    # Snapshot settings.
    snapshot = brayns.Snapshot(
        resolution=resolution,
        camera=camera,
        renderer=renderer,
    )
    
    # Render images using pattern and updating snapshot settings.
    for index, frame in enumerate(indices):
        snapshot.frame = frame
        snapshot.save(instance, pattern % index)
