import brayns

URI = "localhost:5000"
PATH = "/gpfs/bbp.cscs.ch/project/proj3/cloned_circuits/FULL_BRAIN_WITH_SIM_15_06_2023//circuit_config.json"
RESOLUTION = brayns.Resolution.full_hd
LIGHTS = [
    brayns.AmbientLight(intensity=0.5),
    brayns.DirectionalLight(intensity=5, direction=brayns.Vector3(1, 0, -1)),
    brayns.DirectionalLight(intensity=2, direction=brayns.Vector3(-1, 0, -1)),
]

connector = brayns.Connector(URI)

with connector.connect() as instance:
    brayns.clear_models(instance)

    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name="root__neurons",
                nodes=brayns.SonataNodes.from_density(0.01),
                morphology=brayns.Morphology(radius_multiplier=20, load_soma=False),
            )
        ]
    )
    models = loader.load_models(instance, PATH)

    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=RESOLUTION.aspect_ratio,
    )
    camera = controller.camera

    for light in LIGHTS:
        brayns.add_light(instance, light)

    renderer = brayns.InteractiveRenderer()

    snapshot = brayns.Snapshot(RESOLUTION, camera, renderer)
    snapshot.save(instance, "test.png")
