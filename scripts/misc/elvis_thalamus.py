import brayns

URI = "localhost:5000"
PATH = "/gpfs/bbp.cscs.ch/project/proj55/iavarone/releases/circuits/O1/2019-11-19_sonata_Zenodo/circuit_sonata.json"

TOP = brayns.Quaternion(
    -0.7300733930355126,
    0.08553673092432125,
    -0.07324985902976434,
    0.6740257907502605,
)
SIDE = brayns.Quaternion(
    0,
    0.7071067811865476,
    0,
    -0.7071067811865476,
)


def load_thalamus(instance: brayns.Instance) -> brayns.Model:
    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name="thalamus_neurons",
                nodes=brayns.SonataNodes.from_names(["mc2_Column"], density=0.2),
                morphology=brayns.Morphology(
                    radius_multiplier=1,
                    load_soma=True,
                    load_dendrites=True,
                    resampling=1,
                ),
            )
        ]
    )
    models = loader.load_models(instance, PATH)
    assert len(models) == 1
    return models[0]


def align_model(
    instance: brayns.Instance,
    model: brayns.Model,
    orientation: brayns.Quaternion,
) -> brayns.Model:
    rotation = brayns.Rotation.from_quaternion(orientation)
    transform = brayns.Transform(rotation=rotation.inverse)
    return brayns.update_model(instance, model.id, transform)


def color_model(instance: brayns.Instance, model: brayns.Model) -> None:
    method = brayns.CircuitColorMethod.LAYER
    colors = {
        "Rt": brayns.Color4.from_color3(brayns.parse_hex_color("#477CA0")),
        "VPL": brayns.Color4.from_color3(brayns.parse_hex_color("#152361")),
    }
    brayns.color_model(instance, model.id, method, colors)


def add_lights(instance: brayns.Instance) -> None:
    intensity = 3
    lights = [
        brayns.DirectionalLight(
            intensity,
            direction=brayns.Vector3(1, -1, -1),
        ),
        brayns.DirectionalLight(
            intensity,
            direction=brayns.Vector3(-1, -1, -1),
        ),
        brayns.DirectionalLight(
            intensity,
            direction=brayns.Vector3(-1, -1, 1),
        ),
        brayns.DirectionalLight(
            intensity,
            direction=brayns.Vector3(1, -1, 1),
        ),
    ]
    for light in lights:
        brayns.add_light(instance, light)


def take_snapshot(instance: brayns.Instance, path: str) -> None:
    resolution = brayns.Resolution(7680, 4320)
    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=resolution.aspect_ratio,
        rotation=brayns.CameraRotation.front,
    )
    camera = controller.camera
    renderer = brayns.ProductionRenderer(
        background_color=brayns.Color4.black.transparent,
    )
    snapshot = brayns.Snapshot(resolution, camera, renderer)
    snapshot.save(instance, path)


def main() -> None:
    connector = brayns.Connector(URI)
    with connector.connect() as instance:
        brayns.clear_models(instance)
        model = load_thalamus(instance)
        color_model(instance, model)
        add_lights(instance)
        model = align_model(instance, model, TOP)
        take_snapshot(instance, "top.png")
        model = align_model(instance, model, SIDE)
        take_snapshot(instance, "side.png")


if __name__ == "__main__":
    main()
