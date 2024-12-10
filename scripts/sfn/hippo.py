import math
from dataclasses import dataclass
from pathlib import Path
from threading import RLock, Thread

import brayns
import libsonata

from colors import ALL

MESH = "/home/acfleury/source/test/hippo.obj"
PATH = "/gpfs/bbp.cscs.ch/project/proj112/circuits/CA1/20211110-BioM/sonata/circuit_config.json"
PATH = "/gpfs/bbp.cscs.ch/home/acfleury/src/Test/circuit_config_hippo.json"
POPULATION = "hippocampus_neurons"

COLORS = {
    "SLM": ALL[0],
    "SO": ALL[1],
    "SP": ALL[2],
    "SR": ALL[3],
}

LIGHTS = [
    brayns.AmbientLight(intensity=0.3),
    brayns.DirectionalLight(intensity=10, direction=brayns.Vector3(1, -1, -1)),
]

CAMERA_POSITION = brayns.Vector3(-910.239, 1621.451, -5344.393)
CAMERA_ROTATION = brayns.euler(171.6, 42, 0, degrees=True)
FOVY = brayns.Fovy(29, degrees=True)

SKIP_FACTOR = 5
RADIUS_MULTIPLIER = 1

MODEL_ROTATION = brayns.euler(180, -90, 0, degrees=True)

CENTER = brayns.Vector3(3503, -3560, 2812)
LOWER = brayns.Vector3(2693, -6966, 2273)

SPAWN_DURATION = 5
GROWTH_DURATION = 10

TEST = True

BACKGROUND = brayns.Color4.black.transparent

if TEST:
    FPS = 25
    RESOLUTION = brayns.Resolution.ultra_hd
    RENDERER = brayns.InteractiveRenderer(background_color=BACKGROUND)
else:
    FPS = 25
    RESOLUTION = brayns.Resolution.ultra_hd
    RENDERER = brayns.ProductionRenderer(
        samples_per_pixel=128,
        max_ray_bounces=5,
        background_color=BACKGROUND,
    )

SPAWN_FRAME_COUNT = SPAWN_DURATION * FPS
GROWTH_FRAME_COUNT = GROWTH_DURATION * FPS

FRAMES = list(range(SPAWN_FRAME_COUNT + GROWTH_FRAME_COUNT + 1))

OUTPUT = "/home/acfleury/source/test/scripts/sfn/frames_hippo/%d.png"

with open("/gpfs/bbp.cscs.ch/home/acfleury/src/Test/test.txt") as file:
    NODES = file.read().split()

# ELVIS
NODES = ["r1i7n0"]
MODEL_ROTATION = brayns.Rotation.identity
FRAMES = [SPAWN_FRAME_COUNT + GROWTH_FRAME_COUNT]
FRAMES = [SPAWN_FRAME_COUNT]


class Frames:
    def __init__(self, frames: list[int]) -> None:
        self._frames = frames
        self._lock = RLock()
        self._index = 0

    def get(self) -> int | None:
        with self._lock:
            index = self._index

            if index == len(self._frames):
                return None

            frame = self._frames[index]

            self._index += 1

            return frame


@dataclass
class Cell:
    id: int
    position: brayns.Vector3


@dataclass
class Circuit:
    cells: list[Cell]


def upload_mesh(instance: brayns.Instance) -> brayns.Model:
    loader = brayns.MeshLoader()

    with open(MESH, "rb") as file:
        data = file.read()

    model = loader.upload_models(instance, loader.OBJ, data)[0]

    return model


def get_angle(position: brayns.Vector3) -> float:
    ref1 = (LOWER - CENTER).normalized
    vector = (position - CENTER).normalized

    rotation = brayns.euler(0, 0, 90, degrees=True)
    ref2 = rotation.apply(ref1, CENTER)

    x = vector.dot(ref1)
    y = vector.dot(ref2)

    return -math.atan2(y, x)


def parse_circuit() -> Circuit:
    config = libsonata.CircuitConfig.from_file(PATH)
    nodes = config.node_population(POPULATION)

    selection = nodes.select_all()
    cells = selection.flatten()
    cells = [int(cell) for cell in cells[::SKIP_FACTOR]]

    xs, ys, zs = (
        nodes.get_attribute(attribute, cells) for attribute in ["x", "y", "z"]
    )

    positions = [
        MODEL_ROTATION.apply(brayns.Vector3(x, y, z)) for x, y, z in zip(xs, ys, zs)
    ]

    assert len(positions) == len(cells)

    cells = [Cell(gid, position) for gid, position in zip(cells, positions)]
    cells.sort(key=lambda cell: get_angle(cell.position))

    return Circuit(cells)


def cleanup() -> None:
    frames = Path(OUTPUT).parent

    if not frames.exists():
        frames.mkdir(parents=True, exist_ok=True)
        return

    for frame in frames.glob("*.png"):
        frame.unlink()


def add_lights(instance: brayns.Instance) -> None:
    brayns.clear_lights(instance)

    for light in LIGHTS:
        brayns.add_light(instance, light)


def load_model(
    instance: brayns.Instance,
    cells: list[int],
    growth: float = 1.0,
    morphologies: bool = False,
) -> None:
    if not cells:
        return

    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name=POPULATION,
                nodes=brayns.SonataNodes.from_ids(cells),
                morphology=brayns.Morphology(
                    radius_multiplier=RADIUS_MULTIPLIER,
                    load_soma=morphologies,
                    load_dendrites=morphologies,
                    growth=growth,
                    resampling=1,
                ),
            )
        ]
    )

    model = loader.load_models(instance, PATH)[0]

    transform = brayns.Transform(rotation=MODEL_ROTATION)
    model = brayns.update_model(instance, model.id, transform)

    method = brayns.CircuitColorMethod.LAYER
    brayns.color_model(instance, model.id, method, COLORS)


def focus_camera(instance: brayns.Instance) -> brayns.Camera:
    position = CAMERA_POSITION
    direction = CAMERA_ROTATION.apply(-brayns.Axis.z)
    up = CAMERA_ROTATION.apply(brayns.Axis.y)

    view = brayns.View(position, position + direction, up)

    projection = brayns.PerspectiveProjection(FOVY)

    return brayns.Camera(view, projection)

    # target = brayns.get_bounds(instance)
    # view = brayns.View(position, target.center)

    # controller = brayns.CameraController(
    #     target=brayns.get_bounds(instance),
    #     aspect_ratio=RESOLUTION.aspect_ratio,
    # )
    # camera = controller.camera

    # camera.distance *= DISTANCE_FACTOR

    # translation = camera.right * TRANSLATION_X + camera.real_up * TRANSLATION_Y
    # camera = camera.translate(translation)

    # return camera


def snapshot(instance: brayns.Instance, camera: brayns.Camera, frame: int) -> None:
    snapshot = brayns.Snapshot(RESOLUTION, camera, RENDERER)
    snapshot.save(instance, OUTPUT % frame)


def make_movie() -> None:
    path = Path(OUTPUT).parent / "movie.mp4"
    movie = brayns.Movie(OUTPUT, FPS)
    movie.save(str(path))


def get_density(frame: int) -> float:
    if frame >= SPAWN_FRAME_COUNT:
        return 1.0

    return frame / SPAWN_FRAME_COUNT


def get_cells(cells: list[Cell], frame: int) -> list[int]:
    density = get_density(frame)
    count = int(density * (len(cells) - 1))
    return [cell.id for cell in cells[:count]]


def get_growth(frame: int) -> float:
    if frame <= SPAWN_FRAME_COUNT:
        return 0.0

    return (frame - SPAWN_FRAME_COUNT) / GROWTH_FRAME_COUNT


def run(node: str, frames: Frames, circuit: Circuit) -> None:
    connector = brayns.Connector(f"{node}:5000")

    print(f"Connecting to {node=}")

    with connector.connect() as instance:
        print(f"Connected to {node=}")

        print(f"Preparing scene for {node=}")

        brayns.clear_models(instance)

        load_model(instance, [cell.id for cell in circuit.cells])

        camera = focus_camera(instance)

        print(f"Scene ready for {node=}")

        while True:
            frame = frames.get()

            if frame is None:
                print(f"All frames rendered for {node=}")
                return

            print(f"Rendering frame {frame}")

            brayns.clear_models(instance)

            add_lights(instance)
            upload_mesh(instance)

            cells = get_cells(circuit.cells, frame)
            growth = get_growth(frame)

            density = len(cells) / len(circuit.cells)

            print(f"Reloading model with {density=}, {growth=}")
            load_model(instance, cells, growth, morphologies=True)

            print("Taking snapshot")
            snapshot(instance, camera, frame)

            print(f"Frame {frame} rendered")


def main() -> None:
    cleanup()

    print("Parsing circuit")
    circuit = parse_circuit()
    print("Parsed circuit")

    frames = Frames(FRAMES)

    threads = [Thread(target=run, args=(node, frames, circuit)) for node in NODES]

    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()

    make_movie()


if __name__ == "__main__":
    main()
