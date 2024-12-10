import brayns

URI = "r1i7n23:5000"
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
ORIENTATION = brayns.Quaternion(
    0.6354477129851502,
    0.25587353743607094,
    -0.20300281686616914,
    0.6996604842681159,
)

NODE_SETS = ["hex0"]
OUTPUT = "hex0.png"

NODE_SETS = ["hex_O1"]
OUTPUT = "hex_O1.png"

DENSITY = 0.1
MORPHOLOGY = True
RADIUS_MULTIPLIER = 1
RESOLUTION = 4 * brayns.Resolution.full_hd
RENDERER = brayns.InteractiveRenderer(ambient_occlusion_samples=16)


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
                nodes=brayns.SonataNodes.from_names(NODE_SETS, DENSITY),
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

    rotation = brayns.Rotation.from_quaternion(ORIENTATION)
    transform = brayns.Transform(rotation=rotation.inverse)
    brayns.update_model(instance, model.id, transform)

    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=RESOLUTION.aspect_ratio,
    )
    camera = controller.camera

    brayns.clear_lights(instance)

    light = brayns.AmbientLight(intensity=0.5)
    brayns.add_light(instance, light)

    light = brayns.DirectionalLight(intensity=5, direction=brayns.Vector3(1, -1, -1))
    light.direction = rotation.apply(light.direction)
    brayns.add_light(instance, light)

    light = brayns.DirectionalLight(intensity=2, direction=brayns.Vector3(-1, -1, -1))
    light.direction = rotation.apply(light.direction)
    brayns.add_light(instance, light)

    method = brayns.CircuitColorMethod.LAYER
    colors = rescale(COLORS_BY_LAYER)
    brayns.color_model(instance, model.id, method, colors)

    snapshot = brayns.Snapshot(RESOLUTION, camera, RENDERER)
    # snapshot.save(instance, OUTPUT)

    sphere = brayns.Sphere(radius=1000, center=controller.target.center)
    ref = brayns.add_geometries(instance, [(sphere, brayns.Color4.red)])

    # brayns.update_model(instance, model.id, visible=False)
    snapshot.save(instance, f"ref_{OUTPUT}")

    brayns.remove_models(instance, [ref.id])
