import brayns

URI = "localhost:5000"
PATH = "/gpfs/bbp.cscs.ch/data/project/proj135/home/petkantc/axon/axonal-projection/axon_projection/validation/circuit-build/lite_multiple_regions/sonata/struct_circuit_config.json"

connector = brayns.Connector(URI)

with connector.connect() as instance:
    brayns.clear_models(instance)

    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name="neurons",
                nodes=brayns.SonataNodes.from_density(0.01),
                morphology=brayns.Morphology(
                    load_soma=True,
                    load_dendrites=True,
                    load_axon=True,
                ),
            )
        ]
    )

    model = loader.load_models(instance, PATH)[0]

    method = brayns.CircuitColorMethod.MORPHOLOGY_SECTION
    values = brayns.get_color_values(instance, model.id, method)
    colors = {
        "apical_dendrite": brayns.Color4(1, 1, 0),
        "dendrite": brayns.Color4.red,
        "axon": brayns.Color4.blue,
        "soma": brayns.Color4.green,
    }
    brayns.color_model(instance, model.id, method, colors)

    brayns.add_light(instance, brayns.AmbientLight())

    resolution = brayns.Resolution.ultra_hd

    renderer = brayns.InteractiveRenderer()

    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=resolution.aspect_ratio,
    )
    camera = controller.camera

    snapshot = brayns.Snapshot(resolution, camera, renderer)

    snapshot.save(instance, "test.png")
