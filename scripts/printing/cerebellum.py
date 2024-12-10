import json
from random import random

import brayns
import libsonata

REMOTE = "r2i0n5:5000"
LOCAL = "localhost:5000"

PATH = "/gpfs/bbp.cscs.ch/project/proj3/cloned_circuits/FullBrainSynthesizedWithAttributes/circuit_config.json"
POPULATION = "root__neurons"
NODESET = "CB"
OUTPUT = "/home/acfleury/source/test/scripts/printing/cerebellum.png"

DENSITY = 1.0
RESAMPLING = 0.9
SUBSAMPLING = 4

CAMERA_TRANSLATION = brayns.Vector3(0, -100, 0)
CAMERA_ROTATION = brayns.euler(0, 90, 0, degrees=True)
CAMERA_DISTANCE_FACTOR = 0.9

LIGHT_DIRECTION = brayns.Vector3(1, -1, -1)

EXPORT_CELLS = False
RADIUS_MULTIPLIER = 3
CELLS = "/home/acfleury/source/test/scripts/printing/cerebellum_cells.json"
INDICES = "/home/acfleury/source/repos/Brayns2/ids.json"
NULL = 4294967295

COLOR_METHOD = brayns.CircuitColorMethod.SUBREGION
EXPORT_COLORS = False
COLORS = "/home/acfleury/source/test/scripts/printing/cerebellum_colors.json"

RELOAD = True

BACKGOUND = brayns.Color4.bbp_background.transparent

PROD = True

if PROD:
    RESOLUTION = brayns.Resolution(6000, 4000)
    RENDERER = brayns.ProductionRenderer(background_color=BACKGOUND)
    MATERIAL = brayns.PrincipledMaterial(
        roughness=0.4,
        ior=1.52,
    )
else:
    RESOLUTION = brayns.Resolution(6000, 4000)
    RENDERER = brayns.InteractiveRenderer(background_color=BACKGOUND)
    MATERIAL = brayns.PhongMaterial()

MODEL_ROTATION = brayns.euler(180, 0, 0, degrees=True)


def load_all_cerebellum_cells() -> list[int]:
    circuit = libsonata.CircuitConfig.from_file(PATH)
    nodes = circuit.node_population(POPULATION)
    node_sets = libsonata.NodeSets.from_file(circuit.node_sets_path)

    selection = node_sets.materialize(NODESET, nodes)

    return [int(cell) for cell in selection.flatten()]


def export_colors() -> None:
    circuit = libsonata.CircuitConfig.from_file(PATH)
    nodes = circuit.node_population(POPULATION)

    keys = nodes.enumeration_values(COLOR_METHOD)

    colors = {key: [random(), random(), random()] for key in keys}

    with open(COLORS, "w") as file:
        json.dump(colors, file)


def load_colors() -> dict[str, brayns.Color4]:
    with open(COLORS) as file:
        colors: dict[str, list[float]] = json.load(file)
        return {key: brayns.Color4(r, g, b) for key, (r, g, b) in colors.items()}


def export_visible_cells() -> None:
    cells = load_all_cerebellum_cells()

    with open(INDICES) as file:
        primitives = json.load(file)

    indices = {primitive for primitive in primitives if primitive < NULL}

    visible_cells = sorted({cells[index] for index in indices})

    with open(CELLS, "w") as file:
        json.dump(visible_cells, file)


def load_visible_cells() -> list[int]:
    with open(CELLS) as file:
        return json.load(file)


def load_circuit(instance: brayns.Instance) -> brayns.Model:
    if EXPORT_CELLS:
        morphologies = False
        cells = load_all_cerebellum_cells()
    else:
        morphologies = True
        cells = load_visible_cells()

    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name=POPULATION,
                nodes=brayns.SonataNodes.from_ids(cells),
                morphology=brayns.Morphology(
                    radius_multiplier=RADIUS_MULTIPLIER if EXPORT_CELLS else 1,
                    load_soma=morphologies,
                    load_dendrites=morphologies,
                    resampling=RESAMPLING,
                    subsampling=SUBSAMPLING,
                ),
            )
        ]
    )

    model = loader.load_models(instance, PATH)[0]

    transform = brayns.Transform(rotation=MODEL_ROTATION)

    return brayns.update_model(instance, model.id, transform)


def color_circuit(instance: brayns.Instance, model: brayns.Model) -> None:
    brayns.set_material(instance, model.id, MATERIAL)
    colors = load_colors()
    brayns.color_model(instance, model.id, COLOR_METHOD, colors)


def add_lights(instance: brayns.Instance) -> None:
    lights = [
        brayns.AmbientLight(0.5),
        brayns.DirectionalLight(
            intensity=10, direction=CAMERA_ROTATION.apply(LIGHT_DIRECTION)
        ),
    ]
    for light in lights:
        brayns.add_light(instance, light)


def focus_camera(instance: brayns.Instance) -> brayns.Camera:
    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=RESOLUTION.aspect_ratio,
        translation=CAMERA_TRANSLATION,
        rotation=CAMERA_ROTATION,
    )
    camera = controller.camera

    camera.distance *= CAMERA_DISTANCE_FACTOR

    return camera


def snapshot(instance: brayns.Instance) -> None:
    camera = focus_camera(instance)
    snapshot = brayns.Snapshot(RESOLUTION, camera, RENDERER)
    snapshot.save(instance, OUTPUT)


def main() -> None:
    if EXPORT_COLORS:
        export_colors()

    uri = LOCAL if EXPORT_CELLS else REMOTE
    connector = brayns.Connector(uri)

    with connector.connect() as instance:
        models = brayns.get_models(instance)

        if not models or RELOAD:
            brayns.clear_models(instance)
            model = load_circuit(instance)
        else:
            model = next(model for model in models if model.type == "neurons")

        brayns.clear_lights(instance)
        add_lights(instance)

        color_circuit(instance, model)

        snapshot(instance)

        if EXPORT_CELLS:
            export_visible_cells()


if __name__ == "__main__":
    main()
