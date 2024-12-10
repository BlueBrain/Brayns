import brayns

URI = "r2i2n0:5000"
PATH = "/gpfs/bbp.cscs.ch/home/acfleury/src/Test/circuit_config.json"
COLOR_SCALE = 0.8
COLORS_BY_LAYER = {
    "1": brayns.Color3(1.0, 0.949, 0.0),
    "2": brayns.Color3(0.968, 0.580, 0.113),
    "3": brayns.Color3(0.878, 0.184, 0.380),
    "4": brayns.Color3(0.988, 0.607, 0.992),
    "5": brayns.Color3(0.407, 0.658, 0.878),
    "6": brayns.Color3(0.423, 0.901, 0.384),
}

COLORS_BY_SYNAPSE_CLASS = {
    "EXC": brayns.parse_hex_color("#FFAC1C"),
    "INH": brayns.parse_hex_color("#FBCEB1"),
}
ORIENTATION = (
    brayns.euler(0, 0, 90, degrees=True)
    .then(brayns.euler(0, 180, 0, degrees=True))
    .then(brayns.euler(90, 0, 0, degrees=True))
)

OUTPUT = "/gpfs/bbp.cscs.ch/home/acfleury/src/Test/sscx_3_5"
BY_LAYER_PNG = f"{OUTPUT}/by_layer.png"
BY_LAYER_EXR = f"{OUTPUT}/by_layer.exr"
BY_SYNAPSE_CLASS_PNG = f"{OUTPUT}/by_synapse_class.png"
BY_SYNAPSE_CLASS_EXR = f"{OUTPUT}/by_synapse_class.exr"

TEST = False

if TEST:
    DENSITY = 1
    MORPHOLOGY = False
    RADIUS_MULTIPLIER = 10
else:
    DENSITY = 0.04
    MORPHOLOGY = True
    RADIUS_MULTIPLIER = 1


def rescale(colors: dict[str, brayns.Color3]) -> dict[str, brayns.Color4]:
    return {
        key: brayns.Color4.from_color3(value * COLOR_SCALE)
        for key, value in colors.items()
    }


connector = brayns.Connector(URI)

with connector.connect() as instance:
    brayns.clear_models(instance)

    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name="S1nonbarrel_neurons",
                nodes=brayns.SonataNodes.from_density(DENSITY),
                morphology=brayns.Morphology(
                    radius_multiplier=RADIUS_MULTIPLIER,
                    load_soma=MORPHOLOGY,
                    load_dendrites=MORPHOLOGY,
                ),
            )
        ]
    )
    loader.load_models(instance, PATH)

    model = brayns.get_model(instance, 0)

    transform = brayns.Transform(rotation=ORIENTATION)
    model = brayns.update_model(instance, model.id, transform)

    material = brayns.PrincipledMaterial(
        roughness=0.35,
        specular=0.5,
    )
    brayns.set_material(instance, model.id, material)

    resolution = 4 * brayns.Resolution.full_hd

    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=resolution.aspect_ratio,
    )
    camera = controller.camera

    renderer = brayns.ProductionRenderer()

    snapshot = brayns.Snapshot(resolution, camera, renderer)

    brayns.clear_lights(instance)

    light = brayns.AmbientLight(intensity=0.5)
    brayns.add_light(instance, light)

    light = brayns.DirectionalLight(intensity=5, direction=brayns.Vector3(1, -1, -1))
    brayns.add_light(instance, light)

    light = brayns.DirectionalLight(intensity=2, direction=brayns.Vector3(-1, -1, -1))
    brayns.add_light(instance, light)

    method = brayns.CircuitColorMethod.LAYER
    colors = rescale(COLORS_BY_LAYER)
    brayns.color_model(instance, model.id, method, colors)
    snapshot.save(instance, BY_LAYER_PNG)

    method = brayns.CircuitColorMethod.SYNAPSE_CLASS
    colors = rescale(COLORS_BY_SYNAPSE_CLASS)
    brayns.color_model(instance, model.id, method, colors)
    snapshot.save(instance, BY_SYNAPSE_CLASS_PNG)
