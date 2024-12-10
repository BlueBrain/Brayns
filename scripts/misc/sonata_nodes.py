import libsonata

MORPHOLOGIES = "/gpfs/bbp.cscs.ch/project/proj142/entities/morphologies/20221112-mouse-trial/ascii "
PATH = "/gpfs/bbp.cscs.ch/project/proj142/circuits/tests/20221112-mouse-trial/circuit.morphologies.h5"

QX = "orientation_x"
QY = "orientation_y"
QZ = "orientation_z"
QW = "orientation_w"

Quaternion = tuple[float, float, float, float]

storage = libsonata.NodeStorage(PATH)
population = next(item for item in storage.population_names)
nodes = storage.open_population(population)

selection = nodes.select_all()

orientations = list[Quaternion]()

xs = nodes.get_attribute(QX, selection)
ys = nodes.get_attribute(QY, selection)
zs = nodes.get_attribute(QZ, selection)
ws = nodes.get_attribute(QW, selection)

for x, y, z, w in zip(xs, ys, zs, ws):
    orientations.append((x, y, z, w))

for orientation in orientations:
    print(orientation)
