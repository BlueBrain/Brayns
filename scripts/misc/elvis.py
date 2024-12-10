import libsonata

CONFIG = (
    "/gpfs/bbp.cscs.ch/project/proj82/circuits/Thal-SSCx/new_v2/circuit_config.json"
)
POPULATION = "All"
SSCX_NODE_SET = "hex_ctc"
QX = "orientation_x"
QY = "orientation_y"
QZ = "orientation_z"
QW = "orientation_w"


Quaternion = tuple[float, float, float, float]

config = libsonata.CircuitConfig.from_file(CONFIG)
node_sets = libsonata.NodeSets.from_file(config.node_sets_path)

population = config.node_population("All")

selection = node_sets.materialize(SSCX_NODE_SET, population)

print(selection.flatten())

orientations = list[Quaternion]()

xs = population.get_attribute(QX, selection)
ys = population.get_attribute(QY, selection)
zs = population.get_attribute(QZ, selection)
ws = population.get_attribute(QW, selection)

for x, y, z, w in zip(xs, ys, zs, ws):
    orientations.append((x, y, z, w))

for orientation in orientations:
    print(orientation)
