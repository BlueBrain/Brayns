import brayns

URI = "localhost:5000"

PATH = "/gpfs/bbp.cscs.ch/project/proj82/home/soplata/rep/build-atlas-based-thalamus-circuits/thv3/sonata//circuit_config.json"
RESOLUTION = brayns.Resolution.full_hd
RENDERER = brayns.InteractiveRenderer()
LIGHTS = [
    brayns.AmbientLight(0.1),
    brayns.DirectionalLight(intensity=5, direction=brayns.Vector3(-1, 0, -1)),
    brayns.DirectionalLight(intensity=2, direction=brayns.Vector3(1, 0, -1)),
]


def load_cells(instance: brayns.Instance) -> int:
    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name="thalamus_neurons",
                nodes=brayns.SonataNodes.from_density(0.01),
                morphology=brayns.Morphology(radius_multiplier=20),
            )
        ]
    )
    models = loader.load_models(instance, PATH)
    return models[0].id


def focus_camera(instance: brayns.Instance) -> brayns.Camera:
    target = brayns.get_bounds(instance)
    controller = brayns.CameraController(
        target=target,
        aspect_ratio=RESOLUTION.aspect_ratio,
    )
    return controller.camera


def add_lights(instance: brayns.Instance) -> None:
    for light in LIGHTS:
        brayns.add_light(instance, light)


def main() -> None:
    connector = brayns.Connector(URI)

    with connector.connect() as instance:
        brayns.clear_models(instance)

        model = load_cells(instance)

        brayns.set_model_color(instance, model, brayns.Color4(0.9, 0.1, 0.1))

        add_lights(instance)

        camera = focus_camera(instance)

        snapshot = brayns.Snapshot(RESOLUTION, camera, RENDERER)
        snapshot.save(instance, "test.png")


if __name__ == "__main__":
    main()
