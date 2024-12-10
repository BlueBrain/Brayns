import brayns

URI = "localhost:5000"
PATH = "/gpfs/bbp.cscs.ch/project/proj82/circuits/Thal-SSCx/new_v2/circuit_config.json"

TOP = brayns.Quaternion(
    -0.10505541386523184,
    0.18769423983357672,
    -0.6267436780101047,
    0.7489503284098799,
)
SIDE = brayns.Quaternion(
    0.46058044221117983,
    -0.28411701180678944,
    -0.559823375357093,
    0.6274878231952602,
)


def load_sscx(instance: brayns.Instance) -> brayns.Model:
    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name="All",
                nodes=brayns.SonataNodes.from_names(["hex_ctc"], density=0.05),
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
        "1": brayns.Color4(1.0, 0.9490196078431372, 0.0),
        "2": brayns.Color4(0.9686274509803922, 0.5803921568627451, 0.11372549019607843),
        "3": brayns.Color4(0.8784313725490196, 0.1843137254901961, 0.3803921568627451),
        "4": brayns.Color4(0.9882352941176471, 0.6078431372549019, 0.9921568627450981),
        "5": brayns.Color4(0.40784313725490196, 0.6588235294117647, 0.8784313725490196),
        "6": brayns.Color4(0.4235294117647059, 0.9019607843137255, 0.3843137254901961),
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
        model = load_sscx(instance)
        color_model(instance, model)
        add_lights(instance)
        model = align_model(instance, model, TOP)
        take_snapshot(instance, "top.png")
        # model = align_model(instance, model, SIDE)
        # take_snapshot(instance, "side.png")


if __name__ == "__main__":
    main()
