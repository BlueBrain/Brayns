import json
import math
import time
from pathlib import Path
from random import random
from threading import RLock, Thread

import brayns
import libsonata

PATH = "/gpfs/bbp.cscs.ch/data/project/proj135/home/petkantc/axon/axonal-projection/axon_projection/validation/circuit-build/lite_iso_final/auxiliary/struct_circuit_config_fake.json"
POPULATION = "neurons"

DENSITY = 0.2
# DENSITY = 0.01

CAMERA_POSITION = brayns.Vector3(-11333.343, -7163.718, -10376.665)
CAMERA_ROTATION = brayns.euler(153.6, 48, 0, degrees=True)
FOVY = brayns.Fovy(29, degrees=True)

RESOLUTION = brayns.Resolution.ultra_hd
# RESOLUTION = brayns.Resolution.full_hd

RENDERER = brayns.ProductionRenderer()
# RENDERER = brayns.InteractiveRenderer()

MATERIAL = (
    brayns.PrincipledMaterial(
        roughness=0.4,
        ior=1.52,
    )
    if isinstance(RENDERER, brayns.ProductionRenderer)
    else brayns.PhongMaterial()
)

COLORS = "/home/acfleury/source/test/scripts/mopro704/colors.json"

PATTERN = "/home/acfleury/source/test/scripts/mopro704/frames/%d.png"

FPS = 25
# FPS = 1

ZOOM_START = 0
ZOOM_DURATION = 10
MAX_ZOOM = 0.18

DURATION = ZOOM_DURATION

FRAMES = list(range(DURATION * FPS + 1))
FRAMES = FRAMES[184:]

with open("/gpfs/bbp.cscs.ch/home/acfleury/src/Test/test.txt") as file:
    NODES = file.read().split()

# NODES = ["r2i2n2"]


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


def cleanup() -> None:
    frames = Path(PATTERN).parent

    if not frames.exists():
        frames.mkdir(parents=True, exist_ok=True)
        return

    for frame in frames.glob("*"):
        frame.unlink()


def export_colors() -> None:
    config = libsonata.CircuitConfig.from_file(PATH)
    population = config.node_population(POPULATION)
    regions = population.enumeration_values("region")

    colors = {region: brayns.Color3(random(), random(), random()) for region in regions}

    with open(COLORS, "w") as file:
        json.dump(colors, file, indent=4)


def load_colors() -> dict[str, brayns.Color4]:
    with open(COLORS) as file:
        data: dict[str, list] = json.load(file)

    return {region: brayns.Color4(*color) for region, color in data.items()}


CIRCUIT_COLORS = load_colors()


def add_lights(instance: brayns.Instance) -> None:
    lights = [
        brayns.AmbientLight(intensity=0.5),
        brayns.DirectionalLight(
            intensity=10, direction=CAMERA_ROTATION.apply(brayns.Vector3(1, -1, -1))
        ),
    ]

    for light in lights:
        brayns.add_light(instance, light)


def load_circuit(instance: brayns.Instance) -> brayns.Model:
    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name=POPULATION,
                nodes=brayns.SonataNodes.from_density(DENSITY),
                morphology=brayns.Morphology(
                    load_soma=True,
                    load_dendrites=True,
                    load_axon=True,
                    resampling=1,
                    subsampling=2,
                ),
            )
        ]
    )

    model = loader.load_models(instance, PATH)[0]

    brayns.set_material(instance, model.id, MATERIAL)

    method = brayns.CircuitColorMethod.REGION
    colors = CIRCUIT_COLORS

    brayns.color_model(instance, model.id, method, colors)

    return model


def get_progress(frame: int, start: int, duration: int) -> float:
    if frame <= start * FPS:
        return 0.0

    if frame >= (start + duration) * FPS:
        return 1.0

    return (frame - start * FPS) / (duration * FPS)


def ease(x: float) -> float:
    return 1 - math.pow(1 - x, 3)


def get_zoom(frame: int) -> float:
    progress = get_progress(frame, ZOOM_START, ZOOM_DURATION)
    progress = ease(progress)
    return 1 - (1 - MAX_ZOOM) * progress


def focus_camera(instance: brayns.Instance, frame: int) -> brayns.Camera:
    position = CAMERA_POSITION
    direction = CAMERA_ROTATION.apply(-brayns.Axis.z)
    up = CAMERA_ROTATION.apply(brayns.Axis.y)

    target = brayns.get_bounds(instance).center
    distance = (position - target).norm

    camera_view = brayns.View(position, position + direction * distance, up)

    projection = brayns.PerspectiveProjection(FOVY)

    camera = brayns.Camera(camera_view, projection)

    camera.distance *= get_zoom(frame)

    return camera


def snapshot(instance: brayns.Instance, frame: int) -> None:
    camera = focus_camera(instance, frame)
    snapshot = brayns.Snapshot(RESOLUTION, camera, RENDERER)
    snapshot.save(instance, PATTERN % frame)


def export_frames(node: str, instance: brayns.Instance, frames: Frames) -> None:
    while True:
        frame = frames.get()

        if frame is None:
            print(f"Frames exported for {node=}")
            return

        print(f"Rendering {frame=} for {node=}")
        snapshot(instance, frame)
        print(f"Rendered {frame=} for {node=}")


def run(node: str, frames: Frames) -> None:
    connector = brayns.Connector(f"{node}:5000")

    print(f"Connecting to {node=}")

    with connector.connect() as instance:
        print(f"Connected to {node=}")

        brayns.clear_models(instance)

        add_lights(instance)

        print(f"Loading circuit for {node=}")
        load_circuit(instance)
        print(f"Loaded circuit for {node=}")

        export_frames(node, instance, frames)


def make_movie() -> None:
    path = Path(PATTERN).parent / "movie.mp4"
    movie = brayns.Movie(PATTERN, FPS)
    movie.save(str(path))


def main() -> None:
    # cleanup()

    frames = Frames(FRAMES)

    threads = [Thread(target=run, args=[node, frames]) for node in NODES]

    for thread in threads:
        thread.start()
        time.sleep(1)

    for thread in threads:
        thread.join()

    make_movie()


if __name__ == "__main__":
    main()
