import brayns

URI = "r1i7n0:5000"
PATH = "/gpfs/bbp.cscs.ch/project/proj112/circuits/CA1/20211110-BioM/sonata/circuit_config.json"
POPULATION = "hippocampus_neurons"

COLORS = {
    "SLM": brayns.Color4(0.8, 0.8, 0.0),
    "SO": brayns.Color4(0.8, 0.5, 0.1),
    "SP": brayns.Color4(0.3, 0.5, 0.7),
    "SR": brayns.Color4(0.8, 0.5, 0.8),
}

LIGHTS = [
    brayns.AmbientLight(intensity=0.25),
    brayns.DirectionalLight(intensity=9, direction=brayns.Vector3(1, -1, -1)),
]

DENSITY = 0.5

MODEL_ROTATION = brayns.euler(180, -90, 0, degrees=True)

ASPECT = 1
DISTANCE_FACTOR = 0.83
TRANSLATION_X = -130
TRANSLATION_Y = 100

TEST = False

BACKGROUND = brayns.Color4.black.transparent

if TEST:
    PIXELS = 2000
    RENDERER = brayns.InteractiveRenderer(background_color=BACKGROUND)
else:
    PIXELS = 23_000
    RENDERER = brayns.ProductionRenderer(
        samples_per_pixel=128,
        max_ray_bounces=5,
        background_color=BACKGROUND,
    )

OUTPUT = "/home/acfleury/source/test/scripts/hippo/23k.png"


def add_lights(instance: brayns.Instance) -> None:
    brayns.clear_lights(instance)

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
    brayns.clear_models(instance)
    model = load_model(instance)
    model = rotate_model(instance, model)
    color_model(instance, model)
    return model


def snapshot(instance: brayns.Instance, model: brayns.Model) -> None:
    bounds = model.bounds

    width = PIXELS
    height = int(width / ASPECT)

    resolution = brayns.Resolution(width, height)

    controller = brayns.CameraController(bounds, ASPECT)
    camera = controller.camera

    camera.distance *= DISTANCE_FACTOR

    translation = camera.right * TRANSLATION_X + camera.real_up * TRANSLATION_Y
    camera = camera.translate(translation)

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

        add_lights(instance)

        color_model(instance, model)

        snapshot(instance, model)


if __name__ == "__main__":
    main()
