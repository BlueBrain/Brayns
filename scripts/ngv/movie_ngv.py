import csv
from pathlib import Path

import brayns

CIRCUIT = "/gpfs/bbp.cscs.ch/project/proj137/NGVCircuits/rat_sscx_S1HL/V10/build/ngv_config.json"
NEURON_TO_FOLLOW = 101720
NEURON_GIDS = list(range(0, 129348, 100)) + [NEURON_TO_FOLLOW]
ASTROCYTE_DENSITY = 0.1
GEOMETRY_TYPE = brayns.GeometryType.SMOOTH
LOAD_AXON = False
LOAD_DENDRITES = True
RADIUS_MULTIPLIER = 1
RELOAD = False

MODEL_ROTATION = brayns.Rotation.from_quaternion(
    brayns.Quaternion(-0.172173, 0.686326, 0.669124, -0.227124)
).inverse

LIGHTS = [
    brayns.AmbientLight(intensity=0.5),
    brayns.DirectionalLight(intensity=10, direction=brayns.Vector3(1, -1, -1)),
    # brayns.DirectionalLight(intensity=5, direction=brayns.Vector3(1, 0, -1)),
    # brayns.DirectionalLight(intensity=2, direction=brayns.Vector3(-1, 0, -1)),
    # brayns.DirectionalLight(intensity=5, direction=brayns.Vector3(0, 0, 1)),
]

FINAL_MATERIAL = brayns.PrincipledMaterial(
    roughness=0.4,
    ior=1.52,
)

NEURON_COLOR = brayns.Color4(1.0, 0.5, 0)
ASTROCYTE_COLOR = brayns.Color4(0, 0.25, 0.5)
VASCULATURE_COLOR = brayns.Color4(0.5, 0, 0)

FRAMES_PATTERN = "/home/acfleury/source/test/scripts/ngv/frames/%d.png"
FPS = 60
DURATION = 30

HIGHLIGHT_NEURON = False
RESOLUTION = brayns.Resolution.ultra_hd
# RESOLUTION = brayns.Resolution.full_hd
# RENDERER = brayns.InteractiveRenderer(background_color=brayns.Color4.black.transparent)
RENDERER = brayns.ProductionRenderer(background_color=brayns.Color4.black.transparent)
FRAMES = list(range(FPS * DURATION))
# FRAMES = FRAMES[::FPS]
# FRAMES = [1 * FPS, 5 * FPS, 10 * FPS, 20 * FPS]
# FRAMES = [20 * FPS]

if HIGHLIGHT_NEURON:
    LIGHTS = [brayns.AmbientLight(intensity=10)]

MOVIE = "/home/acfleury/source/test/movie.mp4"

if isinstance(RENDERER, brayns.ProductionRenderer):
    MATERIAL = FINAL_MATERIAL
else:
    MATERIAL = brayns.PhongMaterial()

NODES_TXT = "/gpfs/bbp.cscs.ch/home/acfleury/src/Test/test.txt"
with open(NODES_TXT) as file:
    NODES = file.read().split()

CAMERAS = "/home/acfleury/source/test/scripts/ngv/cameras.csv"

POSITIONS = list[brayns.Vector3]()
ROTATIONS = list[brayns.Rotation]()

with open(CAMERAS) as file:
    reader = csv.reader(file)
    for row in reader:
        position = [float(i) for i in row[:3]]
        POSITIONS.append(brayns.Vector3(*position))
        rotation = [float(i) for i in row[3:]]
        ROTATIONS.append(brayns.euler(*rotation, degrees=True))


def cleanup() -> None:
    output = Path(FRAMES_PATTERN).parent
    output.mkdir(parents=True, exist_ok=True)
    for item in output.glob("*"):
        item.unlink()


def load_cells(instance: brayns.Instance) -> brayns.Future[list[brayns.Model]]:
    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name="All",
                nodes=brayns.SonataNodes.from_ids(NEURON_GIDS),
                morphology=brayns.Morphology(
                    radius_multiplier=RADIUS_MULTIPLIER,
                    load_soma=True,
                    load_axon=LOAD_AXON,
                    load_dendrites=LOAD_DENDRITES,
                    geometry_type=GEOMETRY_TYPE,
                ),
            ),
            brayns.SonataNodePopulation(
                name="astrocytes",
                nodes=brayns.SonataNodes.from_density(ASTROCYTE_DENSITY),
                morphology=brayns.Morphology(
                    radius_multiplier=RADIUS_MULTIPLIER,
                    load_soma=True,
                    load_axon=LOAD_AXON,
                    load_dendrites=LOAD_DENDRITES,
                    geometry_type=GEOMETRY_TYPE,
                ),
            ),
            brayns.SonataNodePopulation(
                name="vasculature",
                nodes=brayns.SonataNodes.from_density(1.0),
                vasculature_radius_multiplier=1,
                morphology=brayns.Morphology(
                    geometry_type=GEOMETRY_TYPE,
                ),
            ),
        ]
    )
    return loader.load_models_task(instance, CIRCUIT)


def highlight_neuron(instance: brayns.Instance, model: brayns.Model) -> None:
    highlight = brayns.Color4.white
    rest = brayns.Color4.black
    by_id = brayns.CircuitColorMethod.ID
    colors = {str(gid): rest for gid in NEURON_GIDS[:-1]}
    if model.type == "neurons":
        brayns.set_model_color(instance, model.id, highlight)
        brayns.color_model(instance, model.id, by_id, colors)
        brayns.set_material(instance, model.id, MATERIAL)
    if model.type == "astrocytes":
        brayns.set_model_color(instance, model.id, rest)
        brayns.set_material(instance, model.id, MATERIAL)
    if model.type == "vasculature":
        brayns.set_model_color(instance, model.id, rest)
        brayns.set_material(instance, model.id, MATERIAL)


def color_model(instance: brayns.Instance, model: brayns.Model) -> None:
    if model.type == "neurons":
        brayns.set_model_color(instance, model.id, NEURON_COLOR)
        brayns.set_material(instance, model.id, MATERIAL)
    if model.type == "astrocytes":
        brayns.set_model_color(instance, model.id, ASTROCYTE_COLOR)
        brayns.set_material(instance, model.id, MATERIAL)
    if model.type == "vasculature":
        brayns.set_model_color(instance, model.id, VASCULATURE_COLOR)
        brayns.set_material(instance, model.id, MATERIAL)


def color_models(instance: brayns.Instance) -> None:
    models = brayns.get_models(instance)
    for model in models:
        if HIGHLIGHT_NEURON:
            highlight_neuron(instance, model)
            continue
        color_model(instance, model)


def transform_models(instance: brayns.Instance) -> None:
    models = brayns.get_models(instance)
    transform = brayns.Transform(
        rotation=MODEL_ROTATION,
    )
    for model in models:
        if model.type not in ["neurons", "astrocytes", "vasculature"]:
            continue
        brayns.update_model(instance, model.id, transform)


def add_lights(instance: brayns.Instance) -> None:
    for light in LIGHTS:
        brayns.add_light(instance, light)


def add_camera_lights(instance: brayns.Instance, camera: brayns.Camera) -> list[int]:
    if HIGHLIGHT_NEURON:
        return []

    size = 400
    backward = 50
    intensity = 15

    bottom_left = camera.position - size / 2 * (camera.real_up + camera.right)
    # bottom_right = bottom_left + size * camera.right
    # top_right = bottom_right + size * camera.real_up

    rotation = brayns.Rotation.from_axis_angle(camera.real_up, -10, degrees=True)
    # rotation2 = brayns.Rotation.from_axis_angle(camera.right, 40, degrees=True)

    back = brayns.QuadLight(
        intensity=intensity,
        bottom_left=bottom_left - backward * camera.direction,
        edge1=size * camera.real_up,
        edge2=rotation.apply(size * camera.right),
    )

    # right = brayns.QuadLight(
    #     intensity=intensity / 2,
    #     bottom_left=bottom_right,
    #     edge1=size * camera.real_up,
    #     edge2=rotation1.apply(size * camera.right),
    # )

    # left = brayns.QuadLight(
    #     intensity=intensity / 2,
    #     bottom_left=bottom_left,
    #     edge1=rotation1.inverse.apply(size * -camera.right),
    #     edge2=size * camera.real_up,
    # )

    # top = brayns.QuadLight(
    #     intensity=intensity / 2,
    #     bottom_left=top_right,
    #     edge1=size * -camera.right,
    #     edge2=rotation2.inverse.apply(size * camera.real_up),
    # )

    # bottom = brayns.QuadLight(
    #     intensity=intensity / 2,
    #     bottom_left=bottom_right,
    #     edge1=rotation2.apply(size * -camera.real_up),
    #     edge2=size * -camera.right,
    # )

    lights = [back]

    models = [brayns.add_light(instance, light) for light in lights]
    return [model.id for model in models]


def take_snapshot(
    instance: brayns.Instance, camera: brayns.Camera, frame: int
) -> brayns.Future[None]:
    filename = FRAMES_PATTERN % frame
    snapshot = brayns.Snapshot(RESOLUTION, camera, RENDERER)
    return snapshot.save_task(instance, filename)


def get_frame_camera(frame: int) -> brayns.Camera:
    position = POSITIONS[frame]
    rotation = ROTATIONS[frame]
    direction = rotation.apply(brayns.Axis.back)
    up = rotation.apply(brayns.Axis.up)
    view = brayns.View(position, position + direction, up)
    return brayns.Camera(view)


def export_frames(instances: list[brayns.Instance], frames: list[int]) -> None:
    cameras = [get_frame_camera(frame) for frame in frames]

    lights = [
        add_camera_lights(instance, camera)
        for instance, camera in zip(instances, cameras)
    ]

    tasks = [
        take_snapshot(instance, camera, frame)
        for instance, camera, frame in zip(instances, cameras, frames)
    ]

    for task in tasks:
        task.wait_for_result()

    for instance, ids in zip(instances, lights):
        brayns.remove_models(instance, ids)


def prepare_scene(instances: list[brayns.Instance]) -> None:
    cells_loaded = all(brayns.get_models(instance) for instance in instances)

    for instance in instances:
        brayns.clear_clipping_geometries(instance)
        brayns.clear_lights(instance)
        add_lights(instance)
        color_models(instance)
        transform_models(instance)

    if cells_loaded and not RELOAD:
        return

    for instance in instances:
        brayns.clear_renderables(instance)

    tasks = [load_cells(instance) for instance in instances]
    for task in tasks:
        task.wait_for_result()

    for instance in instances:
        color_models(instance)
        transform_models(instance)


def connect_to_nodes() -> list[brayns.Instance]:
    instances = list[brayns.Instance]()
    for node in NODES:
        connector = brayns.Connector(f"{node}:5000", max_attempts=None)
        instance = connector.connect()
        instances.append(instance)
    return instances


def split_in_batches(frames: list[int], size: int) -> list[list[int]]:
    count, remainder = divmod(len(frames), size)
    result = [frames[i * size : (i + 1) * size] for i in range(count)]
    if remainder != 0:
        result.append(frames[-remainder:])
    return result


def make_movie() -> None:
    movie = brayns.Movie(FRAMES_PATTERN, FPS)
    movie.save(MOVIE)


def main() -> None:
    cleanup()

    instances = connect_to_nodes()

    prepare_scene(instances)

    batches = split_in_batches(FRAMES, len(NODES))

    for batch in batches:
        export_frames(instances, batch)

    for instance in instances:
        instance.disconnect()

    make_movie()


if __name__ == "__main__":
    main()
