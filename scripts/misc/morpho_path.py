import brayns

URI = "localhost:5000"
PATH = "/gpfs/bbp.cscs.ch/project/proj82/scratch/home/arnaudon/O1_synthesis_reconstructed_axons/sonata/struct_circuit_config.json"

connector = brayns.Connector(URI)

with connector.connect() as instance:
    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name="O1_neurons",
                nodes=brayns.SonataNodes.from_density(0.01),
                morphology=brayns.Morphology(load_dendrites=True),
            )
        ]
    )
    print(loader.load_models(instance, PATH))

    brayns.add_light(instance, brayns.AmbientLight())

    resolution = brayns.Resolution.full_hd

    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=resolution.aspect_ratio,
    )
    camera = controller.camera

    renderer = brayns.InteractiveRenderer()

    snapshot = brayns.Snapshot(resolution, camera, renderer)
    snapshot.save(instance, "snapshot.jpg")
