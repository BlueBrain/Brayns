import brayns

URI = "localhost:5000"
PATH = "/gpfs/bbp.cscs.ch/data/scratch/proj134/home/king/BBPP134-917/gpfs_simulation_config2.json"
FRAME = 970

connector = brayns.Connector(URI)

with connector.connect() as instance:
    brayns.clear_models(instance)

    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name="root__neurons",
                nodes=brayns.SonataNodes.all(),
                report=brayns.SonataReport.compartment("soma"),
                morphology=brayns.Morphology(
                    radius_multiplier=10,
                ),
            )
        ]
    )

    models = loader.load_models(instance, PATH)

    model = models[0]

    transform = brayns.Transform(
        rotation=brayns.euler(180, 0, 0, degrees=True),
    )

    model = brayns.update_model(instance, model.id, transform)

    resolution = brayns.Resolution.full_hd

    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=resolution.aspect_ratio,
    )
    camera = controller.camera

    light = brayns.DirectionalLight(intensity=5, direction=brayns.Vector3(1, -1, -1))
    brayns.add_light(instance, light)

    light = brayns.AmbientLight(intensity=0.5)
    brayns.add_light(instance, light)

    renderer = brayns.InteractiveRenderer()

    snapshot = brayns.Snapshot(resolution, camera, renderer)

    brayns.enable_simulation(instance, model.id, True)

    ramp = brayns.ColorRamp(
        value_range=brayns.ValueRange(0, 1),
        colors=[
            brayns.Color4(0, 0.8, 0),
            brayns.Color4(0.8, 0, 0),
        ],
    )

    brayns.set_color_ramp(instance, model.id, ramp)

    simulation = brayns.get_simulation(instance)

    snapshot.frame = simulation.start_frame + FRAME

    snapshot.save(instance, "test.png")
