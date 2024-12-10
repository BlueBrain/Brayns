import time
from pathlib import Path
from threading import RLock, Thread

import brayns

PATH = "/gpfs/bbp.cscs.ch/data/scratch/proj145/circuits/ds_2024/20240812/sonata/circuit_config.json"
MESH = "/gpfs/bbp.cscs.ch/home/arnaudon/code/synthdb/synthdb/insitu_synthesis_inputs/mouse_STRd/striatum_boundary.obj"
MESH = "/gpfs/bbp.cscs.ch/project/proj3/tolokoban/brain.obj"
MESH = "/home/acfleury/source/test/scripts/sfn/STRd.obj"
OUTPUT = "/home/acfleury/source/test/scripts/sfn/frames_strd/%d.png"
POPULATION = "CP_neurons"

DENSITY = 0.1
RESAMPLING = 0.99
SUBSAMPLING = 2
FORCE_RELOAD = True

ROTATION_DURATION = 30
ZOOM_DURATION = 10

# PROD = False
PROD = True
BACKGOUND = brayns.Color4.bbp_background.transparent

if PROD:
    FPS = 25
    # FPS = 1
    RESOLUTION = brayns.Resolution.ultra_hd
    RENDERER = brayns.ProductionRenderer(background_color=BACKGOUND)
    MESH_MATERIAL = brayns.GlassMaterial()
    CIRCUIT_MATERIAL = brayns.PrincipledMaterial(
        roughness=0.4,
        ior=1.52,
    )
else:
    FPS = 10
    RESOLUTION = brayns.Resolution.full_hd
    RENDERER = brayns.InteractiveRenderer(background_color=BACKGOUND)
    MESH_MATERIAL = brayns.GhostMaterial()
    CIRCUIT_MATERIAL = brayns.PhongMaterial()

MESH_COLOR = brayns.Color4.from_color3(brayns.parse_hex_color("#98D6F9"))

CIRCUIT_COLORS = {
    "CP_MSN1": brayns.Color4(1.0, 0.5, 0),
    "CP_MSN2": brayns.Color4(0, 0.25, 0.5),
    "CP_FS": brayns.Color4(0.5, 0, 0),
    "CP_CTAN": brayns.Color4(0.5, 0, 0.25),
    "CP_LTS": brayns.Color4(0, 0.5, 0),
}

MODEL_ROTATION = brayns.euler(180, 0, 0, degrees=True)

with open("/gpfs/bbp.cscs.ch/home/acfleury/src/Test/test.txt") as file:
    NODES = file.read().split()

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


def load_mesh(instance: brayns.Instance) -> brayns.Model:
    with open(MESH, "rb") as file:
        data = file.read()

    loader = brayns.MeshLoader()
    return loader.upload_models(instance, loader.OBJ, data)[0]


def color_mesh(instance: brayns.Instance, model: brayns.Model) -> None:
    brayns.set_model_color(instance, model.id, MESH_COLOR)
    brayns.set_material(instance, model.id, MESH_MATERIAL)


def load_circuit(instance: brayns.Instance) -> brayns.Model:
    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name=POPULATION,
                nodes=brayns.SonataNodes.from_density(DENSITY),
                morphology=brayns.Morphology(
                    radius_multiplier=1,
                    load_soma=True,
                    load_dendrites=True,
                    resampling=RESAMPLING,
                    subsampling=SUBSAMPLING,
                ),
            )
        ]
    )
    return loader.load_models(instance, PATH)[0]


def color_circuit(instance: brayns.Instance, model: brayns.Model) -> None:
    brayns.set_material(instance, model.id, CIRCUIT_MATERIAL)
    method = brayns.CircuitColorMethod.MTYPE
    brayns.color_model(instance, model.id, method, CIRCUIT_COLORS)


def add_clip_plane(instance: brayns.Instance, circuit: brayns.Model) -> None:
    radius = max(circuit.bounds.size) / 1.5
    center = circuit.bounds.center

    sphere = brayns.Sphere(radius, center)

    brayns.add_clipping_geometries(instance, [sphere], invert_normals=True)


def add_lights(instance: brayns.Instance) -> None:
    brayns.clear_lights(instance)

    lights = [
        brayns.AmbientLight(0.5),
        brayns.DirectionalLight(intensity=15, direction=brayns.Vector3(1, -1, -1)),
        brayns.DirectionalLight(intensity=15, direction=brayns.Vector3(-1, -1, 1)),
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

    mesh = load_mesh(instance)
    mesh = rotate_model(instance, mesh)
    color_mesh(instance, mesh)

    add_clip_plane(instance, circuit)

    return circuit


def focus_camera(model: brayns.Model) -> brayns.Camera:
    controller = brayns.CameraController(
        target=model.bounds,
        aspect_ratio=RESOLUTION.aspect_ratio,
    )
    camera = controller.camera

    return camera


def export_rotation(
    instance: brayns.Instance, circuit: brayns.Model, frames: Frames
) -> None:
    camera = focus_camera(circuit)
    snapshot = brayns.Snapshot(RESOLUTION, camera, RENDERER)

    while True:
        index = frames.get()

        if index is None:
            return

        angle = 360 * index / frames.count
        rotation = brayns.euler(0, angle, 0, degrees=True)

        snapshot.camera = camera.rotate_around_target(rotation)

        print(f"Rendering frame {index}")
        snapshot.save(instance, OUTPUT % index)
        print(f"Rendered frame {index}")


def export_zoom(
    instance: brayns.Instance, circuit: brayns.Model, frames: Frames, offset: int
) -> None:
    camera = focus_camera(circuit)
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


def run(node: str, rotation_frames: Frames, zoom_frames: Frames) -> None:
    connector = brayns.Connector(f"{node}:5000")

    print(f"Connecting to {node=}")
    with connector.connect() as instance:
        print(f"Connected to {node=}")

        print(f"Loading models for {node=}")
        circuit = load_models(instance)
        print(f"Loaded models for {node=}")

        print(f"Exporting rotation for {node=}")
        export_rotation(instance, circuit, rotation_frames)
        print(f"Exported rotation for {node=}")

        print(f"Exporting zoom for {node=}")
        export_zoom(instance, circuit, zoom_frames, rotation_frames.count)
        print(f"Exported zoom for {node=}")


def make_movie() -> None:
    path = Path(OUTPUT).parent / "movie.mp4"
    movie = brayns.Movie(OUTPUT, FPS)
    movie.save(str(path))


def main() -> None:
    cleanup()

    rotation_frames = Frames(ROTATION_FRAMES)
    zoom_frames = Frames(ZOOM_FRAMES)

    threads = [
        Thread(target=run, args=(node, rotation_frames, zoom_frames)) for node in NODES
    ]

    for thread in threads:
        thread.start()
        time.sleep(1)

    for thread in threads:
        thread.join()

    make_movie()


if __name__ == "__main__":
    main()
