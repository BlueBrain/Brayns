import json
from random import random

import brayns

CIRCUIT = "/gpfs/bbp.cscs.ch/project/proj143/home/arnaudon/proj143/circuit-build-SIM/auxiliary/circuit_config_hpc.json"
MESHES = [
    # "/gpfs/bbp.cscs.ch/project/proj143/home/arnaudon/proj143/circuit-build-SIM/bioname/meshes/gr_mesh_viz.obj",
    # "/gpfs/bbp.cscs.ch/project/proj143/home/arnaudon/proj143/circuit-build-SIM/bioname/meshes/outer_mesh_viz.obj",
]
COLORS = "/home/acfleury/source/test/scripts/lastone/colors.json"
OUTPUT = "/home/acfleury/source/test/scripts/lastone/snapshot.png"

URI = "r1i7n24:5000"

POPULATION = "cerebellum_neurons"

DENSITY = 1.0
MORPHOLOGIES = True
# MORPHOLOGIES = False
AXONS = True
# AXONS = False
RADIUS_MULTIPLIER = 10

CAMERA_DISTANCE_FACTOR = 0.8
CAMERA_TRANSLATION = brayns.Vector3(0, 100, 0)
CAMERA_ROTATION = brayns.euler(0, 0, 0, degrees=True)

RESOLUTION = 2 * brayns.Resolution.ultra_hd
# RESOLUTION = brayns.Resolution.full_hd

# RENDERER = brayns.ProductionRenderer()
RENDERER = brayns.InteractiveRenderer()

MATERIAL = (
    brayns.PrincipledMaterial(
        roughness=0.4,
        ior=1.52,
    )
    if isinstance(RENDERER, brayns.ProductionRenderer)
    else brayns.PhongMaterial()
)

EXPORT_COLORS = False

MESH_MATERIAL = brayns.GlassMaterial()
MESH_COLOR = brayns.Color4.from_color3(brayns.parse_hex_color("#F0F080"))

LIGHTS = [
    brayns.AmbientLight(intensity=0.5),
    brayns.DirectionalLight(intensity=10, direction=brayns.Vector3(-1, -1, -1)),
]


def add_lights(instance: brayns.Instance) -> None:
    for light in LIGHTS:
        brayns.add_light(instance, light)


def load_somas_and_dendrites(instance: brayns.Instance) -> brayns.Model:
    population = brayns.SonataNodePopulation(
        name=POPULATION,
        nodes=brayns.SonataNodes.from_density(DENSITY),
        morphology=brayns.Morphology(
            radius_multiplier=1 if MORPHOLOGIES else RADIUS_MULTIPLIER,
            load_soma=MORPHOLOGIES,
            load_dendrites=MORPHOLOGIES,
            load_axon=AXONS,
        ),
    )

    loader = brayns.SonataLoader([population])
    return loader.load_models(instance, CIRCUIT)[0]


def get_circuit_colors(
    instance: brayns.Instance, model: brayns.Model, method: str
) -> dict[str, brayns.Color4]:
    if EXPORT_COLORS:
        keys = brayns.get_color_values(instance, model.id, method)
        colors = {key: brayns.Color4(random(), random(), random()) for key in keys}

        with open(COLORS, "w") as file:
            obj = {key: [color.r, color.g, color.b] for key, color in colors.items()}
            json.dump(obj, file)

        return colors

    with open(COLORS) as file:
        obj: dict[str, list] = json.load(file)

    return {key: brayns.Color4(*value) for key, value in obj.items()}


def color_circuit(instance: brayns.Instance, model: brayns.Model) -> None:
    brayns.set_material(instance, model.id, MATERIAL)
    method = brayns.CircuitColorMethod.MTYPE
    colors = get_circuit_colors(instance, model, method)
    brayns.color_model(instance, model.id, method, colors)


def load_circuit(instance: brayns.Instance) -> list[brayns.Model]:
    population = brayns.SonataNodePopulation(
        name=POPULATION,
        nodes=brayns.SonataNodes.from_density(DENSITY),
        morphology=brayns.Morphology(
            radius_multiplier=1 if MORPHOLOGIES else RADIUS_MULTIPLIER,
            load_soma=MORPHOLOGIES,
            load_dendrites=MORPHOLOGIES,
            load_axon=AXONS,
        ),
    )

    loader = brayns.SonataLoader([population])
    model = loader.load_models(instance, CIRCUIT)[0]

    color_circuit(instance, model)

    return [model]


def load_meshes(instance: brayns.Instance) -> list[brayns.Model]:
    loader = brayns.MeshLoader()

    models = [loader.load_models(instance, mesh)[0] for mesh in MESHES]

    for model in models:
        brayns.set_material(instance, model.id, MESH_MATERIAL)
        brayns.set_model_color(instance, model.id, MESH_COLOR)

    return models


def take_snapshot(instance: brayns.Instance) -> None:
    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=RESOLUTION.aspect_ratio,
        rotation=CAMERA_ROTATION,
        translation=CAMERA_TRANSLATION,
    )
    camera = controller.camera

    camera.distance *= CAMERA_DISTANCE_FACTOR

    snapshot = brayns.Snapshot(RESOLUTION, camera, RENDERER)
    snapshot.save(instance, OUTPUT)


def main() -> None:
    connector = brayns.Connector(URI)

    with connector.connect() as instance:
        brayns.clear_models(instance)

        add_lights(instance)

        load_circuit(instance)
        load_meshes(instance)

        take_snapshot(instance)


if __name__ == "__main__":
    main()
