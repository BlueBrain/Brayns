from dataclasses import dataclass
from pathlib import Path
from threading import RLock, Thread

import brayns
import libsonata

PATH = "/gpfs/bbp.cscs.ch/home/acfleury/src/Test/circuit_config_thalamus.json"
POPULATION = "All"

COLORS = {
    "Rt": "#9A3F90",
    "VPL": "#A6175E",
}

CAMERA_POSITION = brayns.Vector3(-2479.504, -490.174, -3171.017)
CAMERA_ROTATION = brayns.euler(162, 49.2, 0, degrees=True)
FOVY = brayns.Fovy(29, degrees=True)

LIGHTS = [
    brayns.AmbientLight(intensity=0.3),
    brayns.DirectionalLight(
        intensity=10, direction=CAMERA_ROTATION.apply(brayns.Vector3(1, -1, -0.5))
    ),
]

SKIP_FACTOR = 1
RADIUS_MULTIPLIER = 1
MORPHOLOGIES = True
RESAMPLING = 0.99
SUBSAMPLING = 2

FPS = 1
RESOLUTION = brayns.Resolution.ultra_hd
SPAWN_DURATION = 5
GROWTH_DURATION = 10

TEST = True

BACKGROUND = brayns.Color4.black.transparent

if TEST:
    RENDERER = brayns.InteractiveRenderer(background_color=BACKGROUND)
    MATERIAL = brayns.PhongMaterial()
else:
    RENDERER = brayns.ProductionRenderer(background_color=BACKGROUND)
    MATERIAL = brayns.PrincipledMaterial(
        roughness=0.35,
        specular=0.5,
    )

SPAWN_FRAME_COUNT = SPAWN_DURATION * FPS
GROWTH_FRAME_COUNT = GROWTH_DURATION * FPS

FRAMES = list(range(SPAWN_FRAME_COUNT + GROWTH_FRAME_COUNT + 1))

OUTPUT = "/home/acfleury/source/test/scripts/sfn/frames_thalamus/%d.png"

with open("/gpfs/bbp.cscs.ch/home/acfleury/src/Test/test.txt") as file:
    NODES = file.read().split()

# HACK
NODES = ["r2i2n1"]
# FRAMES = [SPAWN_FRAME_COUNT]


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


def parse_circuit() -> Circuit:
    config = libsonata.CircuitConfig.from_file(PATH)
    nodes = config.node_population(POPULATION)

    selection = nodes.select_all()
    cells = selection.flatten()
    cells = [int(cell) for cell in cells[::SKIP_FACTOR]]

    xs, ys, zs = (
        nodes.get_attribute(attribute, cells) for attribute in ["x", "y", "z"]
    )

    positions = [brayns.Vector3(x, y, z) for x, y, z in zip(xs, ys, zs)]

    assert len(positions) == len(cells)

    cells = [Cell(gid, position) for gid, position in zip(cells, positions)]
    cells.sort(key=lambda cell: cell.position.y)

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
    instance: brayns.Instance, cells: list[int], growth: float
) -> brayns.Model | None:
    if not cells:
        return None

    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name=POPULATION,
                nodes=brayns.SonataNodes.from_ids(cells),
                morphology=brayns.Morphology(
                    radius_multiplier=RADIUS_MULTIPLIER,
                    load_soma=MORPHOLOGIES,
                    load_dendrites=MORPHOLOGIES,
                    growth=growth,
                    resampling=RESAMPLING,
                    subsampling=SUBSAMPLING,
                ),
            )
        ]
    )

    model = loader.load_models(instance, PATH)[0]

    brayns.set_material(instance, model.id, MATERIAL)

    method = brayns.CircuitColorMethod.LAYER

    colors = {
        key: brayns.Color4.from_color3(brayns.parse_hex_color(color))
        for key, color in COLORS.items()
    }

    brayns.color_model(instance, model.id, method, colors)

    return model


def focus_camera() -> brayns.Camera:
    position = CAMERA_POSITION
    direction = CAMERA_ROTATION.apply(-brayns.Axis.z)
    up = CAMERA_ROTATION.apply(brayns.Axis.y)

    view = brayns.View(position, position + direction, up)

    projection = brayns.PerspectiveProjection(FOVY)

    return brayns.Camera(view, projection)


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
    count = int(density * len(cells))
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

        add_lights(instance)

        camera = focus_camera()

        print(f"Scene ready for {node=}")

        while True:
            frame = frames.get()

            if frame is None:
                print(f"All frames rendered for {node=}")
                return

            print(f"Rendering {frame=} with {node=}")

            cells = get_cells(circuit.cells, frame)
            growth = get_growth(frame)

            density = len(cells) / len(circuit.cells)

            print(f"Reloading model with {density=}, {growth=}")
            model = load_model(instance, cells, growth)

            print("Taking snapshot")
            snapshot(instance, camera, frame)

            print(f"Frame {frame} rendered")

            if model is not None:
                brayns.remove_models(instance, [model.id])


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
