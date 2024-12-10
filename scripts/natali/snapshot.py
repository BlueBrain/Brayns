from dataclasses import replace

import brayns
import libsonata

RAT_CONFIG = "/gpfs/bbp.cscs.ch/project/proj83/circuits/Bio_M/20200805/CircuitConfig"
HUMAN_CONFIG = (
    "/gpfs/bbp.cscs.ch/project/proj159/circuit_building/23042024/run/CircuitConfig"
)

RAT = "/gpfs/bbp.cscs.ch/home/acfleury/src/Test/circuit_config_rat_natali.json"
RAT_POPULATION = "All"
RAT_TARGETS = [
    "L2_TPC:A",
    "L2_TPC:B",
    "L3_TPC:A",
    "L3_TPC:C",
    "L4_TPC",
    "L5_TPC:A",
    "L5_TPC:B",
    "L5_TPC:C",
    "L6_TPC:A",
    "L6_TPC:C",
]
RAT_NODESETS = (
    "/gpfs/bbp.cscs.ch/project/proj83/circuits/Bio_M/20200805/sonata/node_sets.json"
)
RAT_NODESET = "S1HL_Column"
RAT_SKIP_FACTOR = 10
RAT_COLOR = brayns.Color4.from_color3(0.8 * brayns.parse_hex_color("#8A2BE2"))
RAT_IMAGE = "/home/acfleury/source/test/scripts/natali/rat.png"

HUMAN = "/gpfs/bbp.cscs.ch/project/proj159/circuit_building/23042024/run/sonata/circuit_config.json"
HUMAN_POPULATION = "hncx_neurons"
HUMAN_LAYERS = [
    "Layer2",
    "Layer3a",
    "Layer3b",
    "Layer4",
    "Layer5",
    "Layer6",
]
HUMAN_NODESETS = "/gpfs/bbp.cscs.ch/project/proj159/circuit_building/23042024/run/sonata/node_sets.json"
HUMAN_NODESET = "mc0_Column"
HUMAN_SKIP_FACTOR = 10
HUMAN_COLOR = brayns.Color4.from_color3(0.8 * brayns.parse_hex_color("#008080"))
HUMAN_IMAGE = "/home/acfleury/source/test/scripts/natali/human.png"

URI = "r1i7n5:5000"

TEST = False

if TEST:
    RESOLUTION = brayns.Resolution.ultra_hd
    RENDERER = brayns.InteractiveRenderer()
    MATERIAL = brayns.PhongMaterial()
else:
    RESOLUTION = 2 * brayns.Resolution.ultra_hd
    RENDERER = brayns.ProductionRenderer()
    MATERIAL = brayns.PrincipledMaterial(
        roughness=0.35,
        specular=0.5,
    )

OUTPUT = "/home/acfleury/source/test/scripts/natali/both.png"
SPHERE = "/home/acfleury/source/test/scripts/natali/sphere.png"


def extract_orientation(
    cells: list[int], nodes: libsonata.NodePopulation
) -> brayns.Rotation:
    xs, ys, zs, ws = (
        nodes.get_attribute(attribute, cells)
        for attribute in [
            "orientation_x",
            "orientation_y",
            "orientation_z",
            "orientation_w",
        ]
    )

    x, y, z, w = (sum(items) / len(items) for items in [xs, ys, zs, ws])

    quaternion = brayns.Quaternion(x, y, z, w)
    rotation = brayns.Rotation.from_quaternion(quaternion)

    return rotation.inverse


def extract_rat_cells(nodes: libsonata.NodePopulation) -> list[int]:
    nodesets = libsonata.NodeSets.from_file(RAT_NODESETS)

    column = {int(cell) for cell in nodesets.materialize(RAT_NODESET, nodes).flatten()}

    layers = {
        int(cell)
        for layer in RAT_TARGETS
        for cell in nodesets.materialize(layer, nodes).flatten()
    }

    cells = column.intersection(layers)

    return sorted(cells)[::RAT_SKIP_FACTOR]


def load_rat_model(instance: brayns.Instance) -> brayns.Model:
    circuit = libsonata.CircuitConfig.from_file(RAT)
    nodes = circuit.node_population(RAT_POPULATION)

    cells = extract_rat_cells(nodes)

    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name=RAT_POPULATION,
                nodes=brayns.SonataNodes.from_ids(cells),
                morphology=brayns.Morphology(
                    load_soma=True,
                    load_dendrites=True,
                ),
            )
        ]
    )
    model = loader.load_models(instance, RAT)[0]

    brayns.set_material(instance, model.id, MATERIAL)

    orientation = extract_orientation(cells, nodes)
    transform = brayns.Transform(rotation=orientation)

    model = brayns.update_model(instance, model.id, transform)

    brayns.set_model_color(instance, model.id, RAT_COLOR)

    return model


def extract_human_cells(nodes: libsonata.NodePopulation) -> list[int]:
    nodesets = libsonata.NodeSets.from_file(HUMAN_NODESETS)

    column = {
        int(cell) for cell in nodesets.materialize(HUMAN_NODESET, nodes).flatten()
    }

    layers = {
        int(cell)
        for layer in HUMAN_LAYERS
        for cell in nodesets.materialize(layer, nodes).flatten()
    }

    cells = column.intersection(layers)

    return sorted(cells)[::HUMAN_SKIP_FACTOR]


def load_human_model(instance: brayns.Instance) -> brayns.Model:
    circuit = libsonata.CircuitConfig.from_file(HUMAN)
    nodes = circuit.node_population(HUMAN_POPULATION)

    cells = extract_human_cells(nodes)

    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name=HUMAN_POPULATION,
                nodes=brayns.SonataNodes.from_ids(cells),
                morphology=brayns.Morphology(
                    load_soma=True,
                    load_dendrites=True,
                ),
            )
        ]
    )
    model = loader.load_models(instance, HUMAN)[0]

    brayns.set_material(instance, model.id, MATERIAL)

    orientation = extract_orientation(cells, nodes)
    transform = brayns.Transform(rotation=orientation)

    model = brayns.update_model(instance, model.id, transform)

    brayns.set_model_color(instance, model.id, HUMAN_COLOR)

    return model


def focus_camera(instance: brayns.Instance) -> brayns.Camera:
    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=RESOLUTION.aspect_ratio,
    )
    camera = controller.camera

    camera.distance *= 0.9

    camera = camera.translate(brayns.Vector3(200, 200, 0))

    return camera


def snapshot(instance: brayns.Instance, camera: brayns.Camera, output: str) -> None:
    snapshot = brayns.Snapshot(RESOLUTION, camera, RENDERER)
    snapshot.save(instance, output)


def main() -> None:
    connector = brayns.Connector(URI)

    with connector.connect() as instance:
        brayns.clear_models(instance)

        brayns.add_light(instance, brayns.AmbientLight(intensity=0.5))
        brayns.add_light(
            instance,
            brayns.DirectionalLight(
                intensity=10,
                direction=brayns.Vector3(1, -1, -1),
            ),
        )

        rat = load_rat_model(instance)
        human = load_human_model(instance)

        translation = rat.bounds.center - human.bounds.center
        translation += human.bounds.width * brayns.Axis.left

        transform = replace(human.transform, translation=translation)
        human = brayns.update_model(instance, human.id, transform)

        camera = focus_camera(instance)

        snapshot(instance, camera, OUTPUT)

        center = human.bounds.center + 0.5 * human.bounds.width * brayns.Axis.x
        sphere = brayns.Sphere(500, center)
        brayns.add_geometries(instance, [(sphere, brayns.Color4.red)])

        # print(human.bounds.center, rat.bounds.center, ref.bounds.center)

        snapshot(instance, camera, SPHERE)


if __name__ == "__main__":
    main()
