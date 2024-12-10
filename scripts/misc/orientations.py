import brayns
import libsonata

URI = "localhost:5000"
PATH = "/gpfs/bbp.cscs.ch/project/proj142/circuits/tests/20221112-mouse-trial/circuit.morphologies.h5"
FOLDER = (
    "/gpfs/bbp.cscs.ch/project/proj142/entities/morphologies/20221112-mouse-trial/ascii"
)
EXTENSION = "asc"
CELL = 120


def snapshot(instance: brayns.Instance) -> None:
    resolution = brayns.Resolution.full_hd

    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=resolution.aspect_ratio,
    )
    camera = controller.camera

    renderer = brayns.InteractiveRenderer()

    snapshot = brayns.Snapshot(resolution, camera, renderer)

    snapshot.save(instance, "snapshot.jpg")


storage = libsonata.NodeStorage(PATH)

name = next(item for item in storage.population_names)

population = storage.open_population(name)

selection = libsonata.Selection([CELL])

filenames = population.get_attribute("morphology", selection)

path = f"{FOLDER}/{filenames[0]}.{EXTENSION}"

xs = population.get_attribute("orientation_x", selection)
ys = population.get_attribute("orientation_y", selection)
zs = population.get_attribute("orientation_z", selection)
ws = population.get_attribute("orientation_w", selection)

orientation = brayns.Quaternion(xs[0], ys[0], zs[0], ws[0])
rotation = brayns.Rotation.from_quaternion(orientation)

xs = population.get_attribute("x", selection)
ys = population.get_attribute("y", selection)
zs = population.get_attribute("z", selection)
position = brayns.Vector3(xs[0], ys[0], zs[0])

print(path)
print(orientation)
print(rotation.euler_degrees)
print(position)

connector = brayns.Connector(URI)

with connector.connect() as instance:
    brayns.clear_models(instance)

    morphology = brayns.Morphology(
        load_dendrites=True,
        load_axon=True,
    )
    loader = brayns.MorphologyLoader(morphology)

    rotated = loader.load_models(instance, path)[0]
    brayns.set_model_color(instance, rotated.id, brayns.Color4.green)

    transform = brayns.Transform(
        translation=position,
        rotation=brayns.Rotation.from_quaternion(orientation),
    )
    rotated = brayns.update_model(instance, rotated.id, transform)

    loader = brayns.CellPlacementLoader(
        FOLDER,
        extension=EXTENSION,
        ids=[CELL],
        morphology=morphology,
    )
    placement = loader.load_models(instance, PATH)[0]
    brayns.set_model_color(instance, placement.id, brayns.Color4.red)

    light = brayns.AmbientLight()
    brayns.add_light(instance, light)

    snapshot(instance)
