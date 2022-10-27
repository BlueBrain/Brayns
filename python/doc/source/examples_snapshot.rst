Take a snapshot
===============

Now that we can load models, it is time to render something.

Target
------

First we have to choose which models to render and for that, we need the bounds
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

Camera positioning can be complex and depends on the target orientation, camera
projection and resolution aspect ratio. Therefore, Brayns provides a helper
function ``look_at`` to create a camera from these parameters.

.. code-block:: python

    # Minimal usage with only target bounds.
    camera = brayns.look_at(target)

    # Resolution of the final image
    resolution = brayns.Resolution.full_hd

    # More advanced / precise usage.
    camera = brayns.look_at(
        target,
        aspect_ratio=resolution.aspect_ratio,
        translation=brayns.Vector3(1, 2, 3),
        rotation=brayns.CameraRotation.left,
        projection=brayns.OrthographicProjection(),
    )

    # We can also specify the projection manually (Perspective by default).
    camera.projection = brayns.OrthographicProjection()

    # We can also move or rotate the camera manually.
    camera.position += brayns.Vector3(1, 2 ,3)

This function is just a helper, see ``Camera`` for available properties.

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
to see what we render.

.. code-block:: python

    light = brayns.DirectionalLight(
        intensity=4,
        direction=camera.direction,
    )

    model = brayns.add_light(instance, light)

Here we add a directional light oriented from the camera to the target. The model
returned can be used to remove or transform it, but in this example we don't use it.

Lights can be selectively removed with ``remove_models``, or cleared using
``clear_lights``.

Snapshot
--------

Now we have everything we need to take a snapshot.

.. code-block:: python

    # Snapshot settings.
    snapshot = brayns.Snapshot(
        resolution=resolution,
        frame=3,
        camera=camera,
        renderer=renderer,
    )

    # Download and save the snapshot on the script host.
    snapshot.save(instance, 'snapshot.png')

We can here specify also a resolution and a simulation frame. If any of the
parameter is None, then the current object of the instance is taken.

That's it, snapshots can also be saved on the backend machine using
``save_remotely`` or retreived as raw bytes using ``download``.

Snapshot vs Image
-----------------

An image of the current scene can be rendered either using ``Snapshot`` or
``Image``.

The ``Snapshot`` renders all accumulation frames in one call using a temporary
context (camera, renderer, framebuffer and simulation frame) so it can use
different settings for rendering without modifying the instance.

The ``Image`` can render one or all accumulation frame(s) using the current
state of an instance. It doesn't render anything if the max accumulation has
been reached and nothing has changed in the scene.

To summarize, use ``Image`` to make a quick render of the current state of a
Brayns instance and ``Snapshot`` to make a more complex rendering with many
samples per pixel without changing the instance state.

.. attention::

    ``Image`` is usually faster to render than ``Snapshot`` when using a
    renderer with few samples per pixel (1-3) but can be a lot slower with more
    samples (> 3).

    The reason is that image uses the current context so it doesn't have the
    overhead of the snapshot to create a temporary one, which makes it faster
    to render one sample.
    
    However, images render all samples individually using accumulation to allow
    retreiving intermediate results which is slower than the technique used by
    the snapshots.
