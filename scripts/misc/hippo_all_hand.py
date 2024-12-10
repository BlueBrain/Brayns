import brayns

URI = "r1i5n14:5000"
PATH = "/gpfs/bbp.cscs.ch/project/proj112/circuits/CA1/20211110-BioM/sonata/circuit_config.json"
POPULATION = "hippocampus_neurons"

COLORS = {
    "SLM": brayns.Color4(1.0, 0.9490196078431372, 0.0),
    "SO": brayns.Color4(0.9686274509803922, 0.5803921568627451, 0.11372549019607843),
    "SP": brayns.Color4(0.40784313725490196, 0.6588235294117647, 0.8784313725490196),
    "SR": brayns.Color4(0.9882352941176471, 0.6078431372549019, 0.9921568627450981),
}

LIGHT_DIRECTION = brayns.Vector3(1, -1, -1)
LIGHTS = [
    brayns.AmbientLight(intensity=0.25),
    brayns.DirectionalLight(intensity=9, direction=LIGHT_DIRECTION),
]

DENSITY = 0.5

MODEL_ROTATION = brayns.euler(180, -90, 0, degrees=True)

TEST = False

if TEST:
    PIXELS = 2000
    RENDERER = brayns.InteractiveRenderer(
        background_color=brayns.Color4.white.transparent,
    )
else:
    PIXELS = 30000
    RENDERER = brayns.ProductionRenderer(
        samples_per_pixel=128,
        max_ray_bounces=5,
        background_color=brayns.Color4.white.transparent,
    )

OUTPUT = "/home/acfleury/source/test/scripts/hippo/30k.png"


def prepare_scene(instance: brayns.Instance) -> None:
    brayns.clear_models(instance)

    for light in LIGHTS:
        brayns.add_light(instance, light)


def load_model(instance: brayns.Instance) -> brayns.Model:
    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name=POPULATION,
                nodes=brayns.SonataNodes.from_density(DENSITY),
                morphology=brayns.Morphology(
                    radius_multiplier=2,
                    load_soma=True,
                    load_dendrites=True,
                ),
            )
        ]
    )

    return loader.load_models(instance, PATH)[0]


def rotate_model(instance: brayns.Instance, model: brayns.Model) -> brayns.Model:
    transform = brayns.Transform(rotation=MODEL_ROTATION)
    return brayns.update_model(instance, model.id, transform)


def color_model(instance: brayns.Instance, model: brayns.Model) -> None:
    method = brayns.CircuitColorMethod.LAYER
    brayns.color_model(instance, model.id, method, COLORS)


def load_scene(instance: brayns.Instance) -> brayns.Model:
    prepare_scene(instance)
    model = load_model(instance)
    model = rotate_model(instance, model)
    color_model(instance, model)
    return model


def snapshot(instance: brayns.Instance, model: brayns.Model) -> None:
    bounds = model.bounds

    aspect = bounds.width / bounds.height
    height = PIXELS
    width = int(aspect * height)

    resolution = brayns.Resolution(width, height)

    controller = brayns.CameraController(bounds, aspect)
    camera = controller.camera

    snapshot = brayns.Snapshot(resolution, camera, RENDERER)

    snapshot.save(instance, OUTPUT)


def main() -> None:
    connector = brayns.Connector(URI)

    with connector.connect() as instance:
        models = brayns.get_models(instance)

        if not models:
            model = load_scene(instance)
        else:
            model = next(model for model in models if model.type == "neurons")

        color_model(instance, model)

        snapshot(instance, model)


if __name__ == "__main__":
    main()
