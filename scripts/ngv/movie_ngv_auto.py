import math
from pathlib import Path

import brayns

CIRCUIT = "/gpfs/bbp.cscs.ch/project/proj137/NGVCircuits/rat_sscx_S1HL/V10/build/ngv_config.json"
NEURON_DENSITY = 0.01
ASTROCYTE_DENSITY = 0.1
GEOMETRY_TYPE = brayns.GeometryType.SMOOTH
LOAD_AXON = False
LOAD_DENDRITES = True
RADIUS_MULTIPLIER = 1
RELOAD = False

CAMERA_DISTANCE = 1.0
CAMERA_TRANSLATION = brayns.Vector3(0, 300, 0)

MODEL_ROTATION = brayns.Rotation.from_quaternion(
    brayns.Quaternion(-0.172173, 0.686326, 0.669124, -0.227124)
).inverse

LIGHTS = [
    brayns.AmbientLight(intensity=0.5),
    brayns.DirectionalLight(intensity=5, direction=brayns.Vector3(1, 0, -1)),
    brayns.DirectionalLight(intensity=2, direction=brayns.Vector3(-1, 0, -1)),
    brayns.DirectionalLight(intensity=5, direction=brayns.Vector3(0, 0, 1)),
]

FINAL_MATERIAL = brayns.PrincipledMaterial(
    roughness=0.35,
    specular=0.5,
)

NEURONS = brayns.Color4(0.9, 0.9, 0.1)
ASTROCYTES = brayns.Color4(0.1, 0.1, 0.9)
VASCULATURE = brayns.Color4(0.9, 0.1, 0.1)

FRAMES_PATTERN = "/gpfs/bbp.cscs.ch/home/acfleury/src/Test/frames/%d.png"

# Prod
RESOLUTION = brayns.Resolution.ultra_hd
RENDERER = brayns.ProductionRenderer()
FPS = 60

# Test
RESOLUTION = brayns.Resolution.full_hd
RENDERER = brayns.ProductionRenderer()
FPS = 60

DURATION = 30
BASE_SPEED = 150
SPEED_FAR = 2 * BASE_SPEED
SPEED_CLOSE = 0.3 * BASE_SPEED
TRANSITION_START = 6
TRANSITION_END = 15

MOVIE = "/home/acfleury/source/test/movie.mp4"

if isinstance(RENDERER, brayns.ProductionRenderer):
    MATERIAL = FINAL_MATERIAL
else:
    MATERIAL = brayns.PhongMaterial()

NODES_TXT = "/gpfs/bbp.cscs.ch/home/acfleury/src/Test/test.txt"
with open(NODES_TXT) as file:
    NODES = file.read().split()


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
                nodes=brayns.SonataNodes.from_density(NEURON_DENSITY),
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


def color_models(instance: brayns.Instance) -> None:
    models = brayns.get_models(instance)
    for model in models:
        if model.type == "neurons":
            brayns.set_model_color(instance, model.id, NEURONS)
            brayns.set_material(instance, model.id, MATERIAL)
        if model.type == "astrocytes":
            brayns.set_model_color(instance, model.id, ASTROCYTES)
            brayns.set_material(instance, model.id, MATERIAL)
        if model.type == "vasculature":
            brayns.set_model_color(instance, model.id, VASCULATURE)
            brayns.set_material(instance, model.id, MATERIAL)


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


def add_camera_light(instance: brayns.Instance, camera: brayns.Camera) -> list[int]:
    lights = [
        brayns.SphereLight(
            intensity=2000,
            position=camera.position + 0 * camera.direction,
            radius=0,
        ),
    ]
    models = [brayns.add_light(instance, light) for light in lights]
    return [model.id for model in models]


def take_snapshot(
    instance: brayns.Instance, camera: brayns.Camera, frame: int
) -> brayns.Future[None]:
    filename = FRAMES_PATTERN % frame
    snapshot = brayns.Snapshot(RESOLUTION, camera, RENDERER)
    return snapshot.save_remotely_task(instance, filename)


def focus_camera(instance: brayns.Instance) -> brayns.Camera:
    target = brayns.get_bounds(instance)

    controller = brayns.CameraController(
        target=target,
        aspect_ratio=RESOLUTION.aspect_ratio,
    )
    camera = controller.camera

    camera.distance *= CAMERA_DISTANCE

    x, y, z = CAMERA_TRANSLATION
    translation = x * camera.right + y * camera.up + z * camera.direction

    return camera.translate(translation)


def ease(ratio: float) -> float:
    return -(math.cos(math.pi * ratio) - 1) / 2


def clamp(value: float, min_value: float, max_value: float) -> float:
    return max(min_value, min(max_value, value))


def get_progress(frame: int) -> float:
    start = TRANSITION_START * FPS
    end = TRANSITION_END * FPS

    progress = (frame - start) / (end - start)
    progress = clamp(progress, 0, 1)
    progress = ease(progress)

    return progress


def compute_speed(frame: int) -> float:
    progress = get_progress(frame)
    return SPEED_FAR * (1 - progress) + SPEED_CLOSE * progress


def get_frame_camera(instance: brayns.Instance, frame: int) -> brayns.Camera:
    camera = focus_camera(instance)

    translation = sum(compute_speed(i) for i in range(frame + 1)) / FPS

    start_direction = camera.direction

    camera.position += translation * start_direction
    camera.target = camera.position + start_direction

    return camera


def export_frames(instances: list[brayns.Instance], frames: list[int]) -> None:
    cameras = [
        get_frame_camera(instance, frame) for instance, frame in zip(instances, frames)
    ]

    lights = [
        add_camera_light(instance, camera)
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

    frame_count = DURATION * FPS
    frames = list(range(frame_count))
    batches = split_in_batches(frames, len(NODES))

    for batch in batches:
        export_frames(instances, batch)

    for instance in instances:
        instance.disconnect()

    make_movie()


if __name__ == "__main__":
    main()
