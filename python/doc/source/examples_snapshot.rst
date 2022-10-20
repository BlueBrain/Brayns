Take a snapshot
===============

Now that we can load models, it is time to render something.

Target
------

First we have to choose which models to render and for that we need the bounds
of the area we want to see in the snapshot.

.. code-block:: python

    # Focus on a specific model.
    target = model.bounds

    # Or the entire scene.
    target = brayns.get_bounds(instance)

    # Or a list of models.
    target = brayns.merge_bounds([
        item.bounds
        for item in models
    ])

Camera
------

A camera is composed of a view (position, target and up direction) and a
projection (3D -> 2D transformation).

As the current camera of a brayns instance is not automatically moved to focus
on the current scene, a custom camera is needed to see something.

.. code-block:: python

    # Compute camera view and projection to focus on target.
    # The camera use the front view by default (X right, Y up and Z front).
    camera = brayns.look_at(target)

    # We can also specify the projection manually (Perspective by default).
    projection = brayns.OrthographicProjection()
    camera = brayns.look_at(target, projection)

    # The camera can be moved manually.
    # Here we rotate of -90 degrees around X using the camera target as center.
    top_view = brayns.Vector3(-90, 0, 0)
    camera.rotate_around_target(top_view)

Renderer
--------

Brayns has two renderers available, one for fast / interactive rendering and
another one for slow and precise rendering (production).

.. code-block:: python

    renderer = brayns.InteractiveRenderer()

    # Or

    renderer = brayns.ProductionRenderer()

The renderer can also be used to configure the number of samples per pixel (
antialiasing) and the ray bounces (reflection of light from a non emissive
surface to another).

Light
-----

By default, Brayns scene is empty, that is why we need to add a light to be able
to see the models we want to render.

.. code-block:: python

    light = brayns.DirectionalLight(
        intensity=4,
        direction=camera.direction,
    )

    light_model = brayns.add_light(instance, light)

Here we add a directional light oriented from the camera to the target. The model
returned can be used to remove or tranform it, but in this example we don't use it.

Lights can be selectively removed with `remove_models`, or cleared using
`clear_lights`.

Snapshot
--------

Now we have everything we need to take a snapshot.

.. code-block:: python

    snapshot = brayns.Snapshot(
        resolution=brayns.Resolution.full_hd,
        frame=3,
        camera=camera,
        renderer=renderer,
    )

    snapshot.save(instance, 'snapshot.png')

We can here specify also a resolution and a simulation frame. If any of the
parameter is None, then the current object of the instance is taken.

That's it, snapshots can also be saved on the backend machine using
`save_remotely` or retreived as raw bytes using `download`.
