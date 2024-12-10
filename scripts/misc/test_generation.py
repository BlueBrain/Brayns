import brayns

PATH = "/gpfs/bbp.cscs.ch/project/proj3/cloned_circuits/GeneratedTest/simulation_config.json"
URI = "localhost:5000"

connector = brayns.Connector(URI)

with connector.connect() as instance:
    brayns.clear_models(instance)

    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name="cerebellum_neurons",
                nodes=brayns.SonataNodes.all(),
                report=brayns.SonataReport.compartment("test"),
                morphology=brayns.Morphology(load_dendrites=True),
            )
        ]
    )
    model = loader.load_models(instance, PATH)[0]

    ramp = brayns.ColorRamp(
        value_range=brayns.ValueRange(0, 3),
        colors=[
            brayns.Color4.red,
            brayns.Color4.green,
            brayns.Color4.blue,
        ],
    )
    brayns.set_color_ramp(instance, model.id, ramp)

    light = brayns.AmbientLight()
    brayns.add_light(instance, light)

    resolution = brayns.Resolution.full_hd

    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=resolution.aspect_ratio,
    )
    camera = controller.camera

    renderer = brayns.InteractiveRenderer()

    snapshot = brayns.Snapshot(resolution, camera, renderer)

    for i in range(3):
        snapshot.frame = i
        snapshot.save(instance, f"snapshot{i}.png")
