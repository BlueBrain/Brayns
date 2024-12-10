import brayns

URI = "localhost:5000"

PATH = "/home/acfleury/source/test/assets/GolgiCell.asc"
RESOLUTION = brayns.Resolution.full_hd
LIGHTS = [
    brayns.AmbientLight(intensity=0.5),
    brayns.DirectionalLight(intensity=5, direction=brayns.Vector3(1, 0, -1)),
    brayns.DirectionalLight(intensity=2, direction=brayns.Vector3(-1, 0, -1)),
]

connector = brayns.Connector(URI)

with connector.connect() as instance:
    brayns.clear_models(instance)

    morphology = brayns.Morphology.full()
    morphology.radius_multiplier = 3
    loader = brayns.MorphologyLoader(morphology)

    model = loader.load_models(instance, PATH)[0]

    brayns.set_model_color(instance, model.id, brayns.Color4(0.9, 0.1, 0.1))

    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=RESOLUTION.aspect_ratio,
    )
    camera = controller.camera

    for light in LIGHTS:
        brayns.add_light(instance, light)

    snapshot = brayns.Snapshot(RESOLUTION, camera)
    snapshot.save(instance, "test.png")
