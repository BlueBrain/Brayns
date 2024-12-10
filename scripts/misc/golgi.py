import brayns

URI = "localhost:5000"
PATH = "/home/acfleury/source/test/assets/test.swc"
PATH = "/home/acfleury/source/test/assets/GolgiCell.swc"

connector = brayns.Connector(URI)

with connector.connect() as instance:
    brayns.clear_models(instance)

    light = brayns.AmbientLight()
    brayns.add_light(instance, light)

    morphology = brayns.Morphology(
        load_soma=True,
        load_dendrites=True,
        load_axon=True,
        geometry_type=brayns.GeometryType.SMOOTH,
    )
    loader = brayns.MorphologyLoader(morphology)

    model = loader.load_models(instance, PATH)[0]

    method = brayns.CircuitColorMethod.MORPHOLOGY_SECTION
    colors = {
        "soma": brayns.Color4.red,
        "apical_dendrite": brayns.Color4.blue,
        "dendrite": brayns.Color4.blue,
        "axon": brayns.Color4.blue,
    }
    brayns.color_model(instance, model.id, method, colors)

    resolution = brayns.Resolution.ultra_hd

    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=resolution.aspect_ratio,
    )
    camera = controller.camera

    renderer = brayns.InteractiveRenderer(ambient_occlusion_samples=10)

    snapshot = brayns.Snapshot(resolution, camera, renderer)
    snapshot.save(instance, "snapshot.png")
