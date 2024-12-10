import libsonata

PATH = "/gpfs/bbp.cscs.ch/project/proj82/circuits/Thal-SSCx/new_v2/circuit_config.json"
QX = "orientation_x"
QY = "orientation_y"
QZ = "orientation_z"
QW = "orientation_w"

Quaternion = tuple[float, float, float, float]

config = libsonata.CircuitConfig.from_file(PATH)

name = next(item for item in config.node_populations)

population = config.node_population(name)

selection = population.select_all()

orientations = list[Quaternion]()

xs = population.get_attribute(QX, selection)
ys = population.get_attribute(QY, selection)
zs = population.get_attribute(QZ, selection)
ws = population.get_attribute(QW, selection)

for x, y, z, w in zip(xs, ys, zs, ws):
    orientations.append((x, y, z, w))

for orientation in orientations:
    print(orientation)
