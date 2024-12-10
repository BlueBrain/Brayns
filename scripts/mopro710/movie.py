import time
from collections.abc import Callable
from dataclasses import dataclass
from pathlib import Path
from threading import RLock, Thread
from typing import Any, NamedTuple, TypeVarTuple

import brayns
import libsonata
import numpy as np

PATH = "/gpfs/bbp.cscs.ch/data/project/proj82/simulations/Thal-SSCx/simulation_campaigns_ctc_v3/CTRL_spon_activity_CHECK/f90c96c6-d537-4141-9dc9-ffd2cc05c1c6/0/simulation_config.json"
PATH = "/gpfs/bbp.cscs.ch/data/project/proj82/simulations/Thal-SSCx/simulation_campaigns_ctc_v3/repeat_flicks_scan_TC_cortex_UDF/MASTER_CHECK/f2de9360-14e6-4560-8a75-4b64fe9f76e5/4/simulation_config.json"
PATH = "/gpfs/bbp.cscs.ch/data/project/proj82/simulations/Thal-SSCx/simulation_campaigns_ctc_v3/repeat_flicks_scan_TC_cortex_UDF/MASTER_CHECK/f2de9360-14e6-4560-8a75-4b64fe9f76e5/0/simulation_config.json"

SSCX_POPULATION = "All"

SSCX_NODESET = "hex_O1"
SSCX_NODESET = "hex_71"

THALAMUS_POPULATION = "thalamus_neurons"
THALAMUS_NODESET = "mc2_Column"

REPORT = "soma_report"

MORPHOLOGIES = False
DENSITY = 1.0
RADIUS_MULTIPLIER = 10

# CAMERA_DISTANCE_FACTOR = 1.0
CAMERA_DISTANCE_FACTOR = 1.1
CAMERA_BASE_ROTATION = brayns.euler(0, 0, 0, degrees=True)

# RESOLUTION = brayns.Resolution.ultra_hd
RESOLUTION = brayns.Resolution.full_hd

# RENDERER = brayns.ProductionRenderer()
RENDERER = brayns.InteractiveRenderer()

MATERIAL = (
    brayns.PrincipledMaterial(
        roughness=0.4,
        ior=1.52,
    )
    if isinstance(RENDERER, brayns.ProductionRenderer)
    else brayns.PhongMaterial()
)

VALUE_RANGE = brayns.ValueRange(-80, -45)

COLORS = [
    brayns.Color4(*brayns.parse_hex_color("#171786")),
    brayns.Color4(*brayns.parse_hex_color("#0064BD")),
    brayns.Color4(*brayns.parse_hex_color("#60099B")),
    brayns.Color4(*brayns.parse_hex_color("#AC005A")),
    brayns.Color4(*brayns.parse_hex_color("#DB4400")),
    brayns.Color4(*brayns.parse_hex_color("#E98B00")),
    brayns.Color4(*brayns.parse_hex_color("#FAF8B9")),
]

PATTERN = "/home/acfleury/source/test/scripts/mopro710/frames/{nodeset}/%d.png"

ROTATIONS_PER_SECOND = 1 / 30

# FPS = 25
FPS = 10

TSTART = 1800
TSTOP = 5000
DT = 0.1
SLOWING_FACTOR = 4

START_FRAME = int(TSTART / DT)
STOP_FRAME = int(TSTOP / DT)

FRAMES = list(range(START_FRAME, STOP_FRAME, int(1000 / FPS / DT / SLOWING_FACTOR)))

with open("/gpfs/bbp.cscs.ch/home/acfleury/src/Test/test.txt") as file:
    NODES = file.read().split()

NODES = ["r1i7n18"]


@dataclass
class Settings:
    sscx_nodeset: str
    sscx_rotation: brayns.Rotation
    frames_pattern: str


class Circuits(NamedTuple):
    sscx: brayns.Model
    thalamus: brayns.Model


class Frames:
    def __init__(self, frames: list[int]) -> None:
        self._frames = frames
        self._lock = RLock()
        self._index = 0

    @property
    def count(self) -> int:
        return len(self._frames)

    def get(self) -> tuple[int, int] | None:
        with self._lock:
            index = self._index

            if index == len(self._frames):
                return None

            frame = self._frames[index]

            self._index += 1

            return index, frame


def cleanup(frames_pattern: str) -> None:
    frames = Path(frames_pattern).parent

    if not frames.exists():
        frames.mkdir(parents=True, exist_ok=True)
        return

    for frame in frames.glob("*"):
        frame.unlink()


def add_lights(instance: brayns.Instance, rotation: brayns.Rotation) -> None:
    brayns.clear_lights(instance)

    lights = [
        brayns.AmbientLight(intensity=0.5),
        brayns.DirectionalLight(
            intensity=10, direction=rotation.apply(brayns.Vector3(1, -1, -1))
        ),
    ]

    for light in lights:
        brayns.add_light(instance, light)


def get_sscx_column_rotation(nodeset: str) -> brayns.Rotation:
    simulation = libsonata.SimulationConfig.from_file(PATH)
    circuit = libsonata.CircuitConfig.from_file(simulation.network)

    population = circuit.node_population(SSCX_POPULATION)
    nodes = libsonata.NodeSets.from_file(circuit.node_sets_path)
    selection = nodes.materialize(nodeset, population)

    xs = population.get_attribute("orientation_x", selection)
    ys = population.get_attribute("orientation_y", selection)
    zs = population.get_attribute("orientation_z", selection)
    ws = population.get_attribute("orientation_w", selection)

    quaternion = brayns.Quaternion(np.mean(xs), np.mean(ys), np.mean(zs), np.mean(ws))

    return brayns.Rotation.from_quaternion(quaternion).inverse


def apply_color_ramp(instance: brayns.Instance, model: brayns.Model) -> None:
    colors = brayns.ColorRamp(VALUE_RANGE, COLORS)
    brayns.set_color_ramp(instance, model.id, colors)


def rotate_sscx_colum(
    instance: brayns.Instance, model: brayns.Model, rotation: brayns.Rotation
) -> brayns.Model:
    transform = brayns.Transform(rotation=rotation)
    return brayns.update_model(instance, model.id, transform)


def move_thalamus(
    instance: brayns.Instance, sscx: brayns.Model, thalamus: brayns.Model
) -> brayns.Model:
    sscx_position = sscx.bounds.center
    thalamus_position = thalamus.bounds.center

    translation = sscx_position - thalamus_position

    sscx_height = sscx.bounds.height
    translation -= sscx_height * brayns.Axis.y * 0.75

    return brayns.update_model(instance, thalamus.id, brayns.Transform(translation))


def load_simulations(instance: brayns.Instance, settings: Settings) -> Circuits:
    report = brayns.SonataReport.compartment(REPORT)

    density = DENSITY if MORPHOLOGIES else 1
    radius_multiplier = 1 if MORPHOLOGIES else RADIUS_MULTIPLIER

    morphology = brayns.Morphology(
        radius_multiplier=radius_multiplier,
        load_soma=MORPHOLOGIES,
        load_dendrites=MORPHOLOGIES,
        load_axon=False,
    )

    sscx_population = brayns.SonataNodePopulation(
        name=SSCX_POPULATION,
        nodes=brayns.SonataNodes.from_names([settings.sscx_nodeset], density),
        report=report,
        morphology=morphology,
    )

    sscx_loader = brayns.SonataLoader([sscx_population])
    sscx = sscx_loader.load_models(instance, PATH)[0]
    brayns.set_material(instance, sscx.id, MATERIAL)

    sscx = rotate_sscx_colum(instance, sscx, settings.sscx_rotation)

    thalamus_population = brayns.SonataNodePopulation(
        name=THALAMUS_POPULATION,
        nodes=brayns.SonataNodes.from_names([THALAMUS_NODESET], density),
        report=report,
        morphology=morphology,
    )

    thalamus_loader = brayns.SonataLoader([thalamus_population])
    thalamus = thalamus_loader.load_models(instance, PATH)[0]
    brayns.set_material(instance, thalamus.id, MATERIAL)

    thalamus = move_thalamus(instance, sscx, thalamus)

    apply_color_ramp(instance, sscx)
    apply_color_ramp(instance, thalamus)

    return Circuits(sscx, thalamus)


def snapshot(
    instance: brayns.Instance, frames_pattern: str, index: int, frame: int
) -> None:
    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=RESOLUTION.aspect_ratio,
    )
    camera = controller.camera

    camera.distance *= CAMERA_DISTANCE_FACTOR

    t = index / FPS
    angle = 360 * t * ROTATIONS_PER_SECOND

    rotation = brayns.euler(0, angle, 0, degrees=True)

    rotation = CAMERA_BASE_ROTATION.then(rotation)

    camera = camera.rotate_around_target(rotation)

    add_lights(instance, rotation)

    snapshot = brayns.Snapshot(RESOLUTION, camera, RENDERER, frame)
    snapshot.save(instance, frames_pattern % index)


def export_frames(instance: brayns.Instance, pattern: str, frames: Frames) -> None:
    while True:
        item = frames.get()

        if item is None:
            return

        index, frame = item

        snapshot(instance, pattern, index, frame)


def run(node: str, settings: Settings, frames: Frames) -> None:
    connector = brayns.Connector(f"{node}:5000")

    with connector.connect() as instance:
        brayns.clear_models(instance)
        load_simulations(instance, settings)

        export_frames(instance, settings.frames_pattern, frames)


Ts = TypeVarTuple("Ts")


def run_in_parallel(target: Callable[[*Ts], Any], args: list[tuple[*Ts]]) -> None:
    threads = [Thread(target=target, args=arg) for arg in args]

    for thread in threads:
        thread.start()
        time.sleep(1)

    for thread in threads:
        thread.join()


def make_movie(pattern: str) -> None:
    path = Path(pattern).parent / "movie.mp4"
    movie = brayns.Movie(pattern, FPS)
    movie.save(str(path))


def render(settings: Settings) -> None:
    cleanup(settings.frames_pattern)

    frames = Frames(FRAMES)

    run_in_parallel(run, [(node, settings, frames) for node in NODES])

    make_movie(settings.frames_pattern)


def main() -> None:
    settings = Settings(
        sscx_nodeset=SSCX_NODESET,
        sscx_rotation=get_sscx_column_rotation(SSCX_NODESET),
        frames_pattern=PATTERN.format(nodeset=SSCX_NODESET),
    )

    render(settings)


if __name__ == "__main__":
    main()
