Load a model
============

Everything that is rendered by Brayns is called `Model`. A model is usually an
assembly of primitives (spheres, boxes, planes, meshes, ...) and can have
a simulation attached to it.

A simulation can change the model color or radius depending on the global
`Simulation` state (current frame). The user is responsible to choose the
current simulation frame, the backend doesn't perform any update on this.

Models can be built from different file formats. The support to parse and load
different files (SONATA, BlueConfig, DTI, ...) is added by plugins
:ref:`plugins-label`. By default, Brayns only supports mesh loading without
plugins.

Loader
------

Loaders contain the settings to load specific file formats. They are mostly
contained in the `braynsCircuitExplorer` plugin (SONATA, BBP) for the backend
support and in `SonataLoader` and `BbpLoader` for the Python API.

Here in this example, we will load a file a BBP format (BlueConfig).

.. code-block:: python

    loader = brayns.BbpLoader(
        cells=brayns.BbpCells.from_density(0.01),
        report=brayns.BbpReport.compartment('soma'),
        morphology=brayns.Morphology(
            radius_multiplier=10,
            load_soma=True,
            load_axon=False,
            load_dendrites=True,
            geometry_type=brayns.GeometryType.SMOOTH,
        )
    )

Here we won't get too much in the details of the parameters as they depend on
the loader but basically user can specify:

- The density of cells to load, it can also be specified by target or by listing
    the GIDs.
- Which report to load (here compartment or spike) with the report parameters.
- How to load the morphologies of the circuit (radius multiplier, which parts
    to load, ...).

Geometries
----------

Raw geometries with custom colors can also be loaded using a list of objects. A
geometry is basically a primitive shape (sphere, box, ...) with a color. It
always create one single model per function call.

.. code-block:: python

    model = brayns.add_geometries(instance, [
        brayns.Sphere(0.1).with_color(brayns.Color4.red),
        brayns.Sphere(0.2, center=brayns.Vector3.one),
    ])

Models
------

Now we can use the loader to load models from a file and retreive them.

.. code-block:: python

    circuit_path = 'path/to/BlueConfig'

    models = loader.load(instance, circuit_path)

    # We will take the first one for the next examples.
    model = models[0]

It returns a list of `Model` created by the loader (usually it is a single
model). It is important to get their ID (`model.id`) if you need to update them.

Model can also be retreived with `get_model` and removed using `remove_models`,
to select the IDs to erase, or `clear_models` to remove them all.

Update a model
~~~~~~~~~~~~~~

Models can be moved and made invisible using the `update_model` function.

.. hint::

    The model transform is the identity by default and is relative to the
    original location of the object when loaded.

.. code-block:: python

    # We use the transform to move the model.
    transform = model.transform

    # Move the model upward.
    transform.translation += 3 * brayns.Vector3.up

    # Rotate the model of 90 degrees around Y.
    euler = brayns.Vector3(0, 90, 0)
    transform.rotation = brayns.Rotation.from_euler(euler, degrees=True)

    # Upload the model and retreive its new state.
    # It is important to use the model returned by update_model as the bounds
    # will be updated if we change the transform.
    model = brayns.update_model(
        instance,
        model.id,
        transform=transform,
        visible=False,
    )

Color a circuit
~~~~~~~~~~~~~~~

Models loaded from a circuit (not regular models) can be colored using special
methods.

Here is an example to color SSCX circuit by layer.

.. code-block:: python

    # Choose a coloring method.
    method = brayns.ColorMethod.LAYER

    # Map method value to color.
    color = brayns.color_circuit_by_method(instance, model.id, method, {
        '1': brayns.Color4(255, 242, 59, 255) / 255,
        '2': brayns.Color4(248, 148, 48, 255) / 255,
        '3': brayns.Color4(225, 45, 97, 255) / 255,
        '4': brayns.Color4(253, 156, 250, 255) / 255,
        '5': brayns.Color4(103, 168, 222, 255) / 255,
        '6': brayns.Color4(106, 230, 109, 255) / 255,
    })

Available color methods and method values for a given circuit (model) can be
retreived using `get_color_methods` and `get_color_method_values`.

Some alternatives exist with `color_circuit_by_id` and `color_circuit`.

Switch between original color and simulation color
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If a model has a simulation attached, its original color can be overriden by
the simulation color. To avoid this `enable_simulation` can be used to enable /
disable simulation colors for a given model.
