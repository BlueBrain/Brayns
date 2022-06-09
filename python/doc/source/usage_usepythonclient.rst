.. _usepythonclient-label:

Using Brayns python client
==========================

After launching the Brayns backend service, we will have a server to which we can connect
and manipulate using the python client.

We will assume the backend has been started with the command:

.. code-block:: console

    braynsService --uri 0.0.0.0:5000

The port 5000 is used in this documentation but if you want to use another one,
just make sure you use this one instead of 5000 it in all the following examples.

The IP mentioned here is from the **SERVER** point of view and specifies which host can connect to this server.

0.0.0.0 is a wildcard IP address which means you can connect from any machine.

Use localhost if you want to allow only local connections.

Initializing the client object
------------------------------

The first step will be always to create a connection to the backend.

To do so, we need the URI of the backend service we want to connect to (ip:port).

Here is the URI for typical use cases:

- If Brayns and the Python script are running on the same machine then the URI is always **localhost:5000**.
- If Brayns is running on a BB5 node and the Python script on another BB5 node, then the URI is **<node ID>:5000** (example r1i1n1:5000) where <node ID> is the ID of the node running the backend.
- If Brayns is running on a BB5 node and the Python script outside BB5, then the URI is **<node ID>.bbp.epfl.ch:5000**.

So for a renderer running on the node r1i1n1 started with URI 0.0.0.0:5000, the connection is made as follows:

.. code-block:: python

    import brayns

    with brayns.connect('r1i1n1.bbp.epfl.ch:5000') as instance:
        print(instance.request('registry'))

It will send a request to get all the existing entrypoint methods of this instance.

.. attention::

    If you are connecting to a backend running on BB5, it will be necessary to be working within the
    EPFL's network, either by working on-site or using a VPN connection.

Loading a model into Brayns
---------------------------

Now, we can start performing requests for example loading a circuit (with circuit explorer plugin loaded during the renderer startup):

.. code-block:: python

    # Path to the file to load
    path = 'path/to/BlueConfig'

    # Loader we want to use (each loader has specific params)
    loader = brayns.BbpLoader(
        cells=brayns.BbpCells.from_density(0.1),
        report=brayns.BbpReport.spikes(),
        radius_multiplier=20
    )

    # Load the model(s) into the instance (some files contain multiple models)
    models = loader.load(instance, path)

    # Use the list of brayns.Model returned
    for model in models:
        print(model.id)

.. hint::
   When loading files from disk, by specifying a path, the backend will be able to load
   only the files which it can find on its filesystem. This means that, if we are running
   the backend on BB5, we cannot specify a path in our local machine.

Adjusting the camera
--------------------

By default, the camera is not placed to look at a given model.

We can position the camera using its position and the target it is looking at.

However, it is not necessary to change the state of the global camera as we can provide our own view when rendering snapshots.

.. code-block:: python

    # Choose the camera type we want (perspective or orthographic)
    camera = brayns.PerspectiveCamera()

    # Use it to get the position and target to see the model entirely.
    view = camera.get_full_screen_view(model.bounds)    


Rendering a snapshot
--------------------

After we have loaded the model and chose the camera type and view, we can get an image of the current scene.

.. code-block:: python

    # Path to save the image
    path = 'snapshot.png'

    # We can choose a custom renderer
    renderer = brayns.InteractiveRenderer.default()

    # Then we can setup our snapshot settings using the previous results
    snapshot = brayns.Snapshot(
        resolution=brayns.Resolution.full_hd,
        view=view,
        camera=camera,
        renderer=renderer
    )

    # And then download and save it.
    snapshot.save(instance, path)

Further information
-------------------

For further information about the API, refer to the different API pages.
