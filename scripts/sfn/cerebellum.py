import time
from pathlib import Path
from threading import RLock, Thread

import brayns

PATH = "/gpfs/bbp.cscs.ch/data/scratch/proj134/fullcircuit/full-synth-more-split-v2/circuit_config.json"
POPULATION = "root__neurons"
NODE_SET = "CB"
OUTPUT = "/home/acfleury/source/test/scripts/sfn/frames_cerebellum/%d.png"

FORCE_RELOAD = True

CLIP_DURATION = 10
ROTATION_DURATION = 10
ZOOM_DURATION = 10

CAMERA_DISTANCE_FACTOR = 0.9

# PROD = False
PROD = True
BACKGOUND = brayns.Color4.bbp_background.transparent

if PROD:
    FPS = 25
    # FPS = 1
    RESOLUTION = brayns.Resolution.ultra_hd
    RENDERER = brayns.ProductionRenderer(background_color=BACKGOUND)
    CIRCUIT_MATERIAL = brayns.PrincipledMaterial(
        roughness=0.4,
        ior=1.52,
    )
else:
    FPS = 10
    RESOLUTION = brayns.Resolution.full_hd
    RENDERER = brayns.InteractiveRenderer(background_color=BACKGOUND)
    CIRCUIT_MATERIAL = brayns.PhongMaterial()

COLOR = brayns.Color4.from_color3(brayns.parse_hex_color("#F0F080"))

MODEL_ROTATION = brayns.euler(180, 0, 0, degrees=True)

with open("/gpfs/bbp.cscs.ch/home/acfleury/src/Test/test2.txt") as file:
    NODES = file.read().split()

CLIP_FRAMES = list(range(CLIP_DURATION * FPS))
ROTATION_FRAMES = list(range(ROTATION_DURATION * FPS))
ZOOM_FRAMES = list(range(ZOOM_DURATION * FPS))

# HACK
# NODES = ["r1i7n18"]


class Frames:
    def __init__(self, frames: list[int]) -> None:
        self._frames = frames
        self._lock = RLock()
        self._index = 0

    @property
    def count(self) -> int:
        return len(self._frames)

    def get(self) -> int | None:
        with self._lock:
            index = self._index

            if index == len(self._frames):
                return None

            frame = self._frames[index]

            self._index += 1

            return frame


def cleanup() -> None:
    folder = Path(OUTPUT).parent

    folder.mkdir(parents=True, exist_ok=True)

    for frame in folder.glob("*.png"):
        frame.unlink()


def rotate_model(instance: brayns.Instance, model: brayns.Model) -> brayns.Model:
    transform = brayns.Transform(rotation=MODEL_ROTATION)
    return brayns.update_model(instance, model.id, transform)


def load_circuit(instance: brayns.Instance) -> brayns.Model:
    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name=POPULATION,
                nodes=brayns.SonataNodes.from_names([NODE_SET]),
                morphology=brayns.Morphology(
                    radius_multiplier=10,
                    load_soma=False,
                    load_dendrites=False,
                    load_axon=False,
                ),
            )
        ]
    )
    return loader.load_models(instance, PATH)[0]


def color_circuit(instance: brayns.Instance, model: brayns.Model) -> None:
    brayns.set_material(instance, model.id, CIRCUIT_MATERIAL)
    brayns.set_model_color(instance, model.id, COLOR)


def add_lights(instance: brayns.Instance) -> None:
    brayns.clear_lights(instance)

    lights = [
        brayns.AmbientLight(0.5),
        brayns.DirectionalLight(intensity=10, direction=brayns.Vector3(1, -1, -1)),
        # brayns.DirectionalLight(intensity=5, direction=brayns.Vector3(-1, -1, -1)),
        brayns.DirectionalLight(intensity=10, direction=brayns.Vector3(-1, -1, 1)),
        # brayns.DirectionalLight(intensity=5, direction=brayns.Vector3(1, -1, 1)),
    ]

    for light in lights:
        brayns.add_light(instance, light)


def load_models(instance: brayns.Instance) -> brayns.Model:
    models = brayns.get_models(instance)

    if models and not FORCE_RELOAD:
        circuit = next(model for model in models if model.type == "neurons")
        brayns.remove_models(
            instance, [model.id for model in models if model.id != circuit.id]
        )
    else:
        brayns.clear_models(instance)
        circuit = load_circuit(instance)
        circuit = rotate_model(instance, circuit)

    color_circuit(instance, circuit)

    add_lights(instance)

    return circuit


def focus_camera(instance: brayns.Instance) -> brayns.Camera:
    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=RESOLUTION.aspect_ratio,
    )
    camera = controller.camera

    camera = camera.rotate_around_target(brayns.euler(0, -90, 0, degrees=True))

    camera.distance *= CAMERA_DISTANCE_FACTOR

    return camera


def add_clip_plane(instance: brayns.Instance, progress: float) -> brayns.Model:
    bounds = brayns.get_bounds(instance)

    start = bounds.max.z
    end = bounds.center.z

    distance = start + progress * (end - start)

    equation = brayns.PlaneEquation(0, 0, 1, distance)
    plane = brayns.Plane(equation)

    return brayns.add_clipping_geometries(instance, [plane], invert_normals=True)


def export_clip(instance: brayns.Instance, frames: Frames) -> None:
    camera = focus_camera(instance)
    snapshot = brayns.Snapshot(RESOLUTION, camera, RENDERER)

    while True:
        index = frames.get()

        if index is None:
            break

        progress = index / frames.count

        plane = add_clip_plane(instance, progress)

        print(f"Rendering frame {index}")
        snapshot.save(instance, OUTPUT % index)
        print(f"Rendered frame {index}")

        brayns.remove_models(instance, [plane.id])

    add_clip_plane(instance, 1)


def export_rotation(instance: brayns.Instance, frames: Frames, offset: int) -> None:
    camera = focus_camera(instance)
    snapshot = brayns.Snapshot(RESOLUTION, camera, RENDERER)

    while True:
        index = frames.get()

        if index is None:
            return

        angle = 90 * index / frames.count
        rotation = brayns.euler(0, angle, 0, degrees=True)

        snapshot.camera = camera.rotate_around_target(rotation)

        index += offset

        print(f"Rendering frame {index}")
        snapshot.save(instance, OUTPUT % index)
        print(f"Rendered frame {index}")


def export_zoom(instance: brayns.Instance, frames: Frames, offset: int) -> None:
    camera = focus_camera(instance)
    camera = camera.rotate_around_target(brayns.euler(0, 90, 0, degrees=True))

    snapshot = brayns.Snapshot(RESOLUTION, camera, RENDERER)

    start_distance = camera.distance
    zoom_distance = start_distance / 2

    while True:
        index = frames.get()

        if index is None:
            break

        distance = start_distance - zoom_distance * index / frames.count

        camera.distance = distance

        index += offset

        print(f"Rendering frame {index}")
        snapshot.save(instance, OUTPUT % index)
        print(f"Rendered frame {index}")


def run(
    node: str, clip_frames: Frames, rotation_frames: Frames, zoom_frames: Frames
) -> None:
    connector = brayns.Connector(f"{node}:5000")

    print(f"Connecting to {node=}")
    with connector.connect() as instance:
        print(f"Connected to {node=}")

        print(f"Loading models for {node=}")
        load_models(instance)
        print(f"Loaded models for {node=}")

        print(f"Exporting clip for {node=}")
        export_clip(instance, clip_frames)
        print(f"Exported clip for {node=}")

        print(f"Exporting rotation for {node=}")
        export_rotation(instance, rotation_frames, clip_frames.count)
        print(f"Exported rotation for {node=}")

        print(f"Exporting zoom for {node=}")
        export_zoom(instance, zoom_frames, clip_frames.count + rotation_frames.count)
        print(f"Exported zoom for {node=}")


def make_movie() -> None:
    path = Path(OUTPUT).parent / "movie.mp4"
    movie = brayns.Movie(OUTPUT, FPS)
    movie.save(str(path))


def main() -> None:
    cleanup()

    clip_frames = Frames(CLIP_FRAMES)
    rotation_frames = Frames(ROTATION_FRAMES)
    zoom_frames = Frames(ZOOM_FRAMES)

    threads = [
        Thread(target=run, args=(node, clip_frames, rotation_frames, zoom_frames))
        for node in NODES
    ]

    for thread in threads:
        thread.start()
        time.sleep(1)

    for thread in threads:
        thread.join()

    make_movie()


if __name__ == "__main__":
    main()
