import math
import shutil
from collections.abc import Callable, Iterator
from contextlib import contextmanager
from dataclasses import dataclass
from pathlib import Path
from threading import RLock, Thread
from typing import Any, TypeVarTuple

import brayns
import libsonata

FOLDER = "/gpfs/bbp.cscs.ch/project/proj83/scratch/bbp_workflow/sscx_calibration_mgfix/5-FullCircuit/5-FullCircuit-2-BetterMinis-Fpr15-StimScan-10x/bb16bd9f-3d21-4a35-8296-d6aec4c55bf7"
STORAGE = "/gpfs/bbp.cscs.ch/project/proj83/circuits/Bio_M/20200805/sonata/networks/nodes/All/nodes_midrange.h5"
PATH = "/gpfs/bbp.cscs.ch/home/acfleury/src/Test/circuit_config_sscx.json"
SPIKES = FOLDER + "/{simulation}/out.h5"

SIMULATIONS = list(range(2, 5))
SIMULATIONS = [2]

MODEL_ROTATION = (
    brayns.euler(0, 0, 90, degrees=True)
    .then(brayns.euler(0, 180, 0, degrees=True))
    .then(brayns.euler(90, 0, 0, degrees=True))
)

OUTPUT = "/home/acfleury/source/test/scripts/mopro691/frames/{simulation}/{view}/%d.png"
POPULATION = "All"
GLOBAL_SKIP_FACTOR = 10
NON_SPIKING_SKIP_FACTOR = 2
# SKIP_FACTOR = 20

# CAMERA_DISTANCE_FACTOR = 0.75
CAMERA_DISTANCE_FACTOR = 0.9
CAMERA_TRANSLATION = brayns.Vector3(0, -300, 0)
ROTATIONS = 0.25
ROTATIONS = 0
ROTATION_AXIS = brayns.Axis.y
START_ANGLE = -45
START_ANGLE = 0

SLOWING_FACTOR = 3
SPIKE_TRANSITION_TIME_UP = 500
SPIKE_TRANSITION_TIME_DOWN = 1000

TSTART = 0
TSTOP = 7000
DURATION = TSTOP - TSTART

TEST = False

BACKGROUND = brayns.Color4.black.transparent

if TEST:
    FPS = 10
    RENDERER = brayns.InteractiveRenderer(background_color=BACKGROUND)
    RESOLUTION = brayns.Resolution.full_hd
    MATERIAL = brayns.PhongMaterial()
else:
    FPS = 25
    RENDERER = brayns.ProductionRenderer(background_color=BACKGROUND)
    RESOLUTION = brayns.Resolution.ultra_hd
    MATERIAL = brayns.PrincipledMaterial(
        roughness=0.35,
        specular=0.5,
    )

COLORS = [
    "#171786",
    "#0064BD",
    "#60099B",
    "#AC005A",
    "#DB4400",
    "#E98B00",
    "#FAF8B9",
]
REST_ALPHA = 0.3

with open("/gpfs/bbp.cscs.ch/home/acfleury/src/Test/test.txt") as file:
    NODES = file.read().split()

# HACK
# NODES = ["r1i7n7"]


@dataclass
class Frame:
    index: int
    current_time: float
    colors: dict[int, brayns.Color4]


def get_frames_to_render() -> list[float]:
    frames = list[float]()

    frame_count = math.ceil(SLOWING_FACTOR * DURATION * FPS / 1000)

    for i in range(frame_count):
        t = TSTART + DURATION * i / (frame_count - 1)
        frames.append(t)

    return frames


FRAMES = get_frames_to_render()


class Frames:
    def __init__(
        self,
        frames: list[float],
        parse_frame: Callable[[float], dict[int, brayns.Color4]],
    ) -> None:
        self._lock = RLock()
        self._index = 0
        self._frames = frames
        self._parse = parse_frame

    def get(self) -> Frame | None:
        with self._lock:
            index = self._index

            if index == len(self._frames):
                return None

            frame = self._frames[index]

            self._index += 1

            return Frame(
                index=index,
                current_time=frame,
                colors=self._parse(frame),
            )


@dataclass
class Simulation:
    index: int
    circuit: str
    spikes: libsonata.SpikePopulation
    frames: list[float]
    cells: list[int]
    base_rotation: brayns.Rotation
    colors: list[brayns.Color4]


@dataclass
class View:
    name: str
    rotation: brayns.Rotation


@dataclass
class Render:
    simulation: Simulation
    view: View
    frames: Frames
    output_pattern: str


VIEWS = [
    View(
        name="front",
        rotation=brayns.Rotation.identity,
    )
]


def cleanup() -> None:
    output = Path(OUTPUT).parent.parent.parent
    shutil.rmtree(output)
    output.mkdir()


def get_output_pattern(simulation: int, view: str) -> str:
    return OUTPUT.format(simulation=simulation, view=view)


def ensure_directory(pattern: str) -> None:
    path = Path(pattern).parent
    path.mkdir(parents=True, exist_ok=True)


def get_cells_to_load(
    nodes: libsonata.NodePopulation, population: libsonata.SpikePopulation
) -> list[int]:
    selection = nodes.select_all()
    print(f"{selection.flat_size} cells in circuit")

    spikes = population.get(selection, TSTART, TSTOP)
    print(f"{len(spikes)} spikes in total")

    spiking_cells = {cell for cell, _ in spikes}
    print(f"{len(spiking_cells)} cells are spiking")

    all_cells = {int(cell) for cell in selection.flatten()}

    non_spiking_cells = all_cells - spiking_cells

    cells = list(non_spiking_cells)[::NON_SPIKING_SKIP_FACTOR] + list(spiking_cells)
    cells.sort()
    print(f"{len(cells)} cells selected before skip")

    cells = cells[::GLOBAL_SKIP_FACTOR]
    print(f"{len(cells)} cells selected")

    return cells


def parse_colors() -> list[brayns.Color4]:
    color3s = [brayns.parse_hex_color(color) for color in COLORS]

    colors = [brayns.Color4.from_color3(color3s[0], REST_ALPHA)]
    colors.extend(brayns.Color4.from_color3(color3) for color3 in color3s[1:])

    return colors


def parse_simulation(index: int) -> Simulation:
    path = SPIKES.format(simulation=index)
    reader = libsonata.SpikeReader(path)
    spikes = reader[POPULATION]

    storage = libsonata.NodeStorage(STORAGE)
    nodes = storage.open_population("All")

    cells = get_cells_to_load(nodes, spikes)

    return Simulation(
        index=index,
        circuit=PATH,
        spikes=spikes,
        frames=FRAMES,
        cells=cells,
        base_rotation=MODEL_ROTATION,
        colors=parse_colors(),
    )


def add_lights(instance: brayns.Instance) -> None:
    light = brayns.AmbientLight(intensity=0.5)
    brayns.add_light(instance, light)

    light = brayns.DirectionalLight(intensity=5, direction=brayns.Vector3(1, -1, -1))
    brayns.add_light(instance, light)

    light = brayns.DirectionalLight(intensity=2, direction=brayns.Vector3(-1, -1, -1))
    brayns.add_light(instance, light)

    light = brayns.DirectionalLight(intensity=5, direction=brayns.Vector3(0, 0, 1))
    brayns.add_light(instance, light)


def load_circuit(instance: brayns.Instance, simulation: Simulation) -> brayns.Model:
    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name=POPULATION,
                nodes=brayns.SonataNodes.from_ids(simulation.cells),
                morphology=brayns.Morphology(
                    radius_multiplier=1,
                    load_soma=True,
                    load_dendrites=True,
                    load_axon=False,
                    resampling=1,
                ),
            )
        ]
    )

    model = loader.load_models(instance, simulation.circuit)[0]

    brayns.set_material(instance, model.id, MATERIAL)

    transform = brayns.Transform(rotation=simulation.base_rotation)

    return brayns.update_model(instance, model.id, transform)


@contextmanager
def connect(node: str) -> Iterator[brayns.Instance]:
    print(f"Connecting to {node}")
    connector = brayns.Connector(f"{node}:5000")

    with connector.connect() as instance:
        print(f"Connected to {node}")
        yield instance


Ts = TypeVarTuple("Ts")


def run_in_parallel(target: Callable[[*Ts], Any], args: list[tuple[*Ts]]) -> None:
    threads = [Thread(target=target, args=arg) for arg in args]

    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()


def load_models(node: str, simulation: Simulation) -> brayns.Model:
    with connect(node) as instance:
        brayns.clear_models(instance)

        add_lights(instance)

        print(f"Loading circuit for {node}")
        model = load_circuit(instance, simulation)
        print(f"Circuit loaded for {node}")

        return model


def prepare_snapshot(instance: brayns.Instance, view: View) -> brayns.Snapshot:
    resolution = RESOLUTION

    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=resolution.aspect_ratio,
    )
    camera = controller.camera

    camera.distance *= CAMERA_DISTANCE_FACTOR

    camera = camera.translate(CAMERA_TRANSLATION)

    camera = camera.rotate_around_target(view.rotation)

    return brayns.Snapshot(resolution, camera, RENDERER)


def read_spikes(
    population: libsonata.SpikePopulation, cells: list[int], current_time: float
) -> list[tuple[int, float]]:
    tstart = max(0, current_time - SPIKE_TRANSITION_TIME_UP)
    tstop = current_time + SPIKE_TRANSITION_TIME_DOWN

    return population.get(cells, tstart, tstop)


def clamp(value: float, lower: float = 0.0, upper: float = 1.0) -> float:
    return max(min(value, upper), lower)


def get_spike_intensity(spike_time: float, current_time: float) -> float:
    delta = abs(spike_time - current_time)

    if current_time <= spike_time:
        intensity = 1 - delta / SPIKE_TRANSITION_TIME_UP
    else:
        intensity = 1 - delta / SPIKE_TRANSITION_TIME_DOWN

    return clamp(intensity)


def get_spike_intensities(
    spikes: list[tuple[int, float]], current_time: float
) -> dict[int, float]:
    intensities = dict[int, float]()

    for cell, spike_time in spikes:
        new_intensity = get_spike_intensity(spike_time, current_time)

        if new_intensity == 0.0:
            continue

        previous_intensity = intensities.get(cell)

        if previous_intensity is None or new_intensity > previous_intensity:
            intensities[cell] = new_intensity

    return intensities


def get_spike_color(colors: list[brayns.Color4], intensity: float) -> brayns.Color4:
    max_index = len(colors) - 1

    index = intensity * max_index
    remainder = index - math.floor(index)

    index = int(index)
    next_index = min(index + 1, max_index)

    return (1 - remainder) * colors[index] + remainder * colors[next_index]


def get_spike_colors(
    simulation: Simulation, current_time: float
) -> dict[int, brayns.Color4]:
    spikes = read_spikes(simulation.spikes, simulation.cells, current_time)
    print(f"{len(spikes)} spikes at {current_time=}")

    intensities = get_spike_intensities(spikes, current_time)
    print(f"{len(intensities)} cells spiking at {current_time=}")

    return {
        cell: get_spike_color(simulation.colors, intensities.get(cell, 0.0))
        for cell in simulation.cells
    }


def color_spikes(
    instance: brayns.Instance, model: brayns.Model, spikes: dict[int, brayns.Color4]
) -> None:
    method = brayns.CircuitColorMethod.ID

    colors = {str(cell): color for cell, color in spikes.items()}

    brayns.color_model(instance, model.id, method, colors)


def export_frames(
    node: str, instance: brayns.Instance, model: brayns.Model, render: Render
) -> None:
    snapshot = prepare_snapshot(instance, render.view)
    original_camera = snapshot.camera

    assert original_camera is not None

    while True:
        print(f"Node {node} getting next frame")
        frame = render.frames.get()

        if frame is None:
            print(f"Node {node} done")
            return

        print(f"Rendering frame {frame.index} with node {node}")

        speed = ROTATIONS / DURATION
        angle = START_ANGLE + 360 * speed * frame.current_time
        rotation = brayns.euler(0, angle, 0, degrees=True)
        snapshot.camera = original_camera.rotate_around_target(rotation)

        color_spikes(instance, model, frame.colors)

        snapshot.save(instance, render.output_pattern % frame.index)

        print(f"Frame {frame.index} rendered with node {node}")


def export_simulation(node: str, render: Render) -> None:
    with connect(node) as instance:
        print(f"Exporting frames for node {node}")

        model = next(
            model for model in brayns.get_models(instance) if model.type == "neurons"
        )

        export_frames(node, instance, model, render)

        print(f"Frames exported for node {node}")


def make_movie(pattern: str) -> None:
    output = Path(pattern).parent / "movie.mp4"
    movie = brayns.Movie(pattern, FPS)
    movie.save(str(output))


def run(simulation_index: int) -> None:
    print(f"Parsing simulation {simulation_index}")
    simulation = parse_simulation(simulation_index)
    print(f"Simulation {simulation_index} parsed")

    print(f"{len(simulation.cells)} cells to load")

    print("Loading models")
    run_in_parallel(load_models, [(node, simulation) for node in NODES])
    print("Models loaded")

    for view in VIEWS:
        print(f"Exporting simulation {simulation_index} and view {view.name}")

        pattern = get_output_pattern(simulation_index, view.name)

        frames = Frames(simulation.frames, lambda t: get_spike_colors(simulation, t))

        ensure_directory(pattern)

        render = Render(simulation, view, frames, pattern)

        run_in_parallel(export_simulation, [(node, render) for node in NODES])

        print(f"Exported simulation {simulation_index} and view {view.name}")

        make_movie(pattern)


def main() -> None:
    cleanup()

    for simulation in SIMULATIONS:
        print(f"Exporting simulation {simulation}")
        run(simulation)


if __name__ == "__main__":
    main()
