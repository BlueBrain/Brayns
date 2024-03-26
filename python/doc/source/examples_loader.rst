Load a model
============

Now that we are connected to a braynsService instance, it is time to load
something to render.

All entities in Brayns are models and refered as ``Model`` in the API. A model
can be something to render (circuit, mesh, morphology, etc...), a light or a
clipping geometry.

Everything that is going to be rendered is usually an assembly of primitives
(spheres, capsules, meshes, etc...) and can have a simulation attached.

A simulation can change the model color or radius depending on the global
``Simulation`` state (current frame). The user is responsible to choose the
current simulation frame, the backend doesn't perform any update on this.

Models can be built from different file formats. The support to parse and load
different files (Sonata, NRRD) is added by plugins
:ref:`plugins-label`. By default, Brayns only supports mesh loading without
plugins.

Loader
------

Loaders contain the settings to load specific file formats. They are mostly
contained in the ``braynsCircuitExplorer`` plugin for the backend
support and in ``SonataLoader`` and for the Python API.

Here in this example, we will load a file at Sonata format (JSON config file).

.. code-block:: python

    loader = brayns.SonataLoader(
        populations=[
            brayns.SonataNodePopulation(
                name="root__neurons",
                nodes=brayns.SonataNodes.from_density(0.01),
                report=brayns.SonataReport.compartment("soma"),
                morphology=brayns.Morphology(
                    radius_multiplier=2,
                    load_soma=True,
                    load_dendrites=True,
                ),
            )
        ]
    )

Here we won't get too much in the details of the parameters as they depend on
the loader but basically user can specify:

- The cells to load, can be specified using density, targets or GIDs.
- Which report to load (here compartment or spike) with the report parameters.
- The morphologies to load and how to load them.

Geometries
----------

Raw geometries with custom colors can also be loaded using a list of objects. A
geometry is basically a primitive shape (sphere, box, ...) with a color. It
always create one single model per function call and all geometries must be of
the same type.

Supported geometry types are:

* ``BoundedPlane``: plane limited in space.
* ``Box``: regular box.
* ``Capsule``: cylinder with spheres at its extemities.
* ``Plane``: infinite plane.
* ``Sphere``: regular sphere.

.. code-block:: python

    model = brayns.add_geometries(instance, [
        (brayns.Sphere(0.1), brayns.Color4.red),
        (brayns.Sphere(0.2, center=brayns.Vector3.one), brayns.Color4.white),
    ])

Clipping geometries
-------------------

Raw geometries can also be used to clip parts of the scene. Each set of added
clipping geometries is a single model like for regular geometries.

For planes, everything below the plane is not rendered and for the rest,
everything inside the shape is not rendered.

.. code-block:: python

    model = brayns.add_clipping_geometries(instance, [
        brayns.Sphere(0.1),
        brayns.Sphere(0.2, center=brayns.Vector3.one),
    ])

Circuits
--------

Now we can use the loader to load models (here circuits) from a file and
retreive them.

.. code-block:: python

    circuit_path = 'path/to/circuit_config.json'

    models = loader.load_models(instance, circuit_path)

    # We will take the first one for the next examples.
    model = models[0]

It returns a list of ``Model`` created by the loader (usually it is a single
model). It is important to get their ID (``model.id``) if you need to update them.

Model can also be retreived with ``get_model`` and removed using ``remove_models``
(from their IDs) or ``clear_models`` (remove all).

Update a model
~~~~~~~~~~~~~~

Models can be moved and made invisible using the ``update_model`` function.

.. hint::

    The model transform is the identity by default and is relative to the
    original location of the object when loaded.

.. code-block:: python

    # We use the transform to move the model.
    transform = model.transform

    # Move the model upward.
    transform.translation += 3 * brayns.Axis.up

    # Rotate the model of 90 degrees around Y.
    transform.rotation = brayns.euler(0, 90, 0, degrees=True)

    # Upload the model and retreive its new state.
    # It is important to use the model returned by update_model as the bounds
    # will be updated if we change the transform.
    model = brayns.update_model(
        instance,
        model.id,
        transform=transform,
        visible=False,
    )

Model coloring
~~~~~~~~~~~~~~

Models loaded in Brayns can be colored using different methods. These methods
depend on the model type and the plugins loaded and can be queried for a given
model.

A color method has a name and some available values that can be mapped to
user-defined colors.

The Python API provides the core coloring method as factory methods in the
``ColorMethod`` and ``CircuitColorMethod`` (plugin dependent) classes.

Here is an example to color an SSCX circuit by layer.

.. code-block:: python

    # Get available coloring methods on model (optional).
    # Gives ['solid', 'element id', 'layer', 'etype', ...] for SSCX.
    methods = brayns.get_color_methods(instance, model.id)

    # Pick a coloring method (here by circuit layer).
    method = brayns.CircuitColorMethod.LAYER

    # Get available coloring values (optional).
    # Gives ['1', '2', ..., '6'] for SSCX.
    values = brayns.get_color_values(instance, model.id, method)

    # Map coloring value (here layer) to their color.
    colors = {
        '1': brayns.Color4(255, 242, 59, 255) / 255,
        '2': brayns.Color4(248, 148, 48, 255) / 255,
        '3': brayns.Color4(225, 45, 97, 255) / 255,
        '4': brayns.Color4(253, 156, 250, 255) / 255,
        '5': brayns.Color4(103, 168, 222, 255) / 255,
        '6': brayns.Color4(106, 230, 109, 255) / 255,
    }

    # Apply color method on given model.
    brayns.color_model(instance, model.id, method, colors)

A shortcut to apply a uniform (solid) color on a model is also provided.

.. code-block:: python

    brayns.set_model_color(instance, model.id, brayns.Color4.red)

Switch between original color and simulation color
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If a model has a simulation attached, its original color can be overriden by
the simulation color. To avoid this ``enable_simulation`` can be used to enable /
disable simulation colors for a given model.
