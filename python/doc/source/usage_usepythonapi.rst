.. _usepythonapi-label:

Using Brayns Python API
=======================

After launching the Brayns backend service, we have a renderer backend instance
running a websocket server and we can monitor it using the web API. The Python
package just wraps the websocket client and the JSON-RPC messages into a more
high-level API with classes and functions.

In the following section, we will assume the backend was started with:

.. code-block:: console

    braynsService --uri 0.0.0.0:5000

The port 5000 is used in this documentation but if you want to use another one,
just make sure it is available on the server machine and that you use this one
instead of 5000 it in all the following examples.

The IP mentioned here is from the **SERVER** point of view and specifies which
host can connect to this server. 0.0.0.0 is a wildcard IP address which means
you can connect from any machine as client. Use localhost if you want to allow
only local connections.

Connection to an instance
--------------------------

To create a connection to the backend from the Python API, we need the URI of
the braynsService instance we want to connect to (ip:port). The IP is always the
one of the machine the instance is running on, seen from the machine of the
Python client.

.. hint::

    The Python API is not doing any expensive tasks so it can be run outside
    BB5 on a local machine (useful for debugging).

Here is the URI for typical use cases:

- If Brayns and the Python script are running on the same machine then the URI
    is always **localhost:5000**.
- If Brayns is running on a BB5 node and the Python script is on another BB5
    node, then the URI is **<node ID>:5000** (example r1i1n1:5000) where
    <node ID> is the ID of the node running the backend.
- If Brayns is running on a BB5 node and the Python script outside BB5, then the
    URI is **<node ID>.bbp.epfl.ch:5000**.

So for a renderer running on the node r1i1n1 started with URI 0.0.0.0:5000, the
connection is made as follows from a remote machine:

.. code-block:: python

    # Import installed brayns Python package.
    import brayns

    # Create a connector with connection settings (URI, SSL, logging, etc).
    connector = brayns.Connector('r1i1n1.bbp.epfl.ch:5000')

    # Connect to the server instance, we use a context manager to close the
    # connection properly once we are done with it. It can be done manually
    # with instance.disconnect().
    with connector.connect() as instance:

        # Test by printing the version of the instance we are connected to.
        print(brayns.get_version(instance))

.. attention::

    If you are connecting to a backend running on BB5, it will be necessary to
    be working within EPFL's network, either on-site or through the VPN.

Loading a model into Brayns
---------------------------

Now, we can start performing requests, for example loading a circuit (requires
circuit explorer plugin loaded during the renderer startup).

.. code-block:: python

    # Path to the file to load.
    path = 'path/to/BlueConfig'

    # Loader we want to use (each loader has specific params).
    loader = brayns.BbpLoader(
        cells=brayns.BbpCells.from_density(0.1),
        report=brayns.BbpReport.spikes(),
        morphology=brayns.Morphology(radius_multiplier=20)
    )

    # Load the model(s) into the instance (some files contain multiple models).
    models = loader.load(instance, path)

    # Use the list of brayns.Model returned.
    for model in models:
        print(model)

.. hint::

    When loading files from disk, by specifying a path, the backend will be able
    to load only the files it can find on its filesystem. This means that, if we
    are running the backend on BB5, we cannot specify a local path.

Adjusting the camera
--------------------

By default, the camera is not placed to look at a given model, that's why we
have to move it manually. The camera view is specified using its position and
its target with an optional up vector. However, it is not necessary to change
the state of the renderer camera as we can provide our own view when rendering
snapshots.

.. code-block:: python

    # Choose the camera type we want (perspective or orthographic).
    camera = brayns.PerspectiveCamera()

    # Get the scene boundary (take all visible models into account).
    # If you want to focus on a single model, use model.bounds.
    bounds = brayns.get_bounds(instance)

    # Use it to get the position and target to see the model entirely.
    view = camera.fovy.get_full_screen_view(bounds)    

    # Camera view can optionally be rotated like this.
    euler = brayns.Vector3(0, 90, 0)
    rotation = brayns.Rotation.from_euler(euler, degrees=True)
    view.position = rotation.apply(view.position, center=view.target)

Adding a light source
---------------------

By default an instance doesn't have any light so we need to add one to see
something.

.. code-block:: python

    # Choose light type. Here directional light with same direction as the
    # camera view.
    light = brayns.DirectionalLight(
        direction=view.direction,
    )

    # Upload the light.
    brayns.add_light(instance, light)

Rendering a snapshot
--------------------

ONce we have loaded out models and chosen the camera type and view, we can get
render and save an image of the current scene.

.. code-block:: python

    # Path to save the image.
    path = 'snapshot.png'

    # We can choose a custom renderer (fast or slow).
    renderer = brayns.InteractiveRenderer()

    # Then we can setup our snapshot settings using the previous results.
    snapshot = brayns.Snapshot(
        resolution=brayns.Resolution.full_hd,
        view=view,
        camera=camera,
        renderer=renderer
    )

    # And then render, download and save our image.
    snapshot.save(instance, path)

Further information
-------------------

For further information about, please refer to the API reference
:ref:`pythonapi-label`.
