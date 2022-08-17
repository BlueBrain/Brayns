Take a snapshot
===============

Now that we can load models, it is time to render something.

Bounds
------

First we have to choose which models to render and for that we need the bounds
of the area we want to see in the snapshot.

.. code-block:: python

    # Focus on a specific model.
    bounds = model.bounds

    # Or the entire scene.
    bounds = brayns.get_bounds(instance)

    # Or some models.
    bounds = brayns.merge_bounds([
        item.bounds
        for item in models
    ])

Camera
------

Brayns has a default camera it uses to render stream images
(BraynsCircuitStudio) but it is usally better to define a snapshot specific
camera.

.. code-block:: python

    camera = brayns.PerspectiveCamera(
        fovy=brayns.Fovy(45, degrees=True),
    )

    # Or

    camera = brayns.OrthographicCamera(
        height=1.5 * bounds.height,
    )

Here we can choose to use a perspective camera with a given field of view (zoom
level) or an orthographic one with given height (the width is computed using the
aspect ratio of the snapshot resolution).

View
----

Depending on the bounds and the camera, we need also to choose the point of view
from which we want to render our snapshot.

.. code-block:: python

    # For a perspective camera, the FOVY gives the full-screen distance.
    view = camera.fovy.get_full_screen_view(bounds)

    # For an orthographic camera, we just take some margin as the viewport is
    # already defined by the camera height.
    view = brayns.OrthographicCamera.get_front_view(bounds)

    # We can also rotate the camera position around the target.
    euler = brayns.Vector3(0, 90, 0)
    rotation = brayns.Rotation.from_euler(euler, degrees=True)
    view.position = rotation.apply(view.position, center=view.target)

    # Or move it closer to the target.
    vector = view.position - view.target
    vector *= 0.75
    view.position = view.target + vector

Renderer
--------

Brayns has two renderers available, one for fast, interactive rendering and
another one for slow and precise rendering (production).

.. code-block:: python

    renderer = brayns.InteractiveRenderer()

    # Or

    renderer = brayns.ProductionRenderer()

The renderer can also be used to configure the number of samples per pixel (
antialiasing) and the ray bounces (reflection of light from a non emissive
surface to another).

Snapshot
--------

Now we have everything we need to take a snapshot.

.. code-block:: python

    snapshot = brayns.Snapshot(
        resolution=brayns.Resolution.full_hd,
        frame=3,
        view=view,
        camera=camera,
        renderer=renderer,
    )

    snapshot.save(instance, 'snapshot.png')

We can here specify also a resolution and a simulation frame. If any of the
parameter is None, then the current object of the instance is taken.

That's it, snapshots can also be saved on the instance machine using
`save_remotely` or retreived as raw bytes using `download`.
