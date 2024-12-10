import brayns

PATH = "/gpfs/bbp.cscs.ch/project/proj159/circuit_building/112024/run/sonata/circuit_config.json"
OUTPUT = "/home/acfleury/source/test/scripts/mopro711/snapshot.png"

URI = "r1i7n18:5000"

POPULATION = "hncx_neurons"
NODESET = "mc0_Column"
DENSITY = 0.2

# CAMERA_DISTANCE_FACTOR = 0.8
# CAMERA_TRANSLATION = brayns.Vector3(0, 600, 0)

CAMERA_DISTANCE_FACTOR = 0.7
CAMERA_TRANSLATION = brayns.Vector3(0, 400, 0)

RESOLUTION = brayns.Resolution.ultra_hd
RENDERER = brayns.ProductionRenderer()

MATERIAL = (
    brayns.PrincipledMaterial(
        roughness=0.4,
        ior=1.52,
    )
    if isinstance(RENDERER, brayns.ProductionRenderer)
    else brayns.PhongMaterial()
)

COLORS = {
    "L1": brayns.Color4(1.0, 0.949, 0.0),
    "L2": brayns.Color4(1.0, 0.301, 0.0),
    "L3a": brayns.Color4(0.878, 0.184, 0.380),
    "L3b": brayns.Color4(0.878, 0.184, 0.380),
    "L4": brayns.Color4(0.988, 0.607, 0.992),
    "L5": brayns.Color4(0.407, 0.658, 0.878),
    "L6": brayns.Color4(0.423, 0.901, 0.384),
}


def add_lights(instance: brayns.Instance) -> None:
    lights = [
        brayns.AmbientLight(intensity=0.5),
        brayns.DirectionalLight(intensity=10, direction=brayns.Vector3(1, -1, -1)),
    ]

    for light in lights:
        brayns.add_light(instance, light)


def load_circuit(instance: brayns.Instance) -> brayns.Model:
    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name=POPULATION,
                nodes=brayns.SonataNodes.from_names([NODESET], DENSITY),
                morphology=brayns.Morphology(
                    load_soma=True,
                    load_dendrites=True,
                ),
            )
        ]
    )

    return loader.load_models(instance, PATH)[0]


def color_circuit(instance: brayns.Instance, model: brayns.Model) -> None:
    brayns.set_material(instance, model.id, MATERIAL)
    method = brayns.CircuitColorMethod.LAYER
    brayns.color_model(instance, model.id, method, COLORS)


def take_snapshot(instance: brayns.Instance) -> None:
    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=RESOLUTION.aspect_ratio,
    )
    camera = controller.camera

    camera = camera.translate(CAMERA_TRANSLATION)

    camera.distance *= CAMERA_DISTANCE_FACTOR

    snapshot = brayns.Snapshot(RESOLUTION, camera, RENDERER)

    snapshot.save(instance, OUTPUT)


def run(instance: brayns.Instance) -> None:
    brayns.clear_models(instance)

    add_lights(instance)

    model = load_circuit(instance)
    color_circuit(instance, model)

    take_snapshot(instance)


def main() -> None:
    connector = brayns.Connector(URI)

    with connector.connect() as instance:
        run(instance)


if __name__ == "__main__":
    main()
