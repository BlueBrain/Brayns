import json
import math
import shutil
import time
from collections import defaultdict
from collections.abc import Callable, Iterator
from contextlib import contextmanager
from dataclasses import dataclass
from pathlib import Path
from threading import RLock, Thread
from typing import Any, TypeVarTuple

import brayns
import libsonata

STIMULI = "/gpfs/bbp.cscs.ch/project/proj96/home/ecker/simulations/d21efd45-7740-4268-b06f-e1de35f2b6cf/input_spikes/stimulus_stream.txt"
PATH = "/gpfs/bbp.cscs.ch/home/acfleury/src/Test/circuit_config_mopro689.json"
FOLDER = "/gpfs/bbp.cscs.ch/project/proj96/home/ecker/simulations/d21efd45-7740-4268-b06f-e1de35f2b6cf"
STORAGE = "/gpfs/bbp.cscs.ch/project/proj83/circuits/Bio_M/20200805/sonata/networks/nodes/All/nodes.h5"
SPIKES = FOLDER + "/{simulation}/out.h5"
POPULATION = "All"
SIMULATION = 0

OUTPUT = "/home/acfleury/source/test/scripts/mopro689/frames"

GIDS = "/home/acfleury/source/test/scripts/mopro689/gids.json"
SKIP_FACTOR = 10

CAMERA_DISTANCE_FACTOR = 0.8
CAMERA_TRANSLATION = brayns.Vector3(0, 200, 0)

FPS = 25
SPIKE_TRANSITION_TIME_UP = 250
SPIKE_TRANSITION_TIME_DOWN = 500

TSTART = 1000
TSTOP = 11_000
STIMULUS_DURATION = 500

IGNORE_STIMULI = ["J"]
MAX_STIMULI = 20

TEST = False

BACKGROUND = brayns.Color4.black.transparent

if TEST:
    RENDERER = brayns.InteractiveRenderer(background_color=BACKGROUND)
    RESOLUTION = brayns.Resolution.full_hd
    MATERIAL = brayns.PhongMaterial()
else:
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
# FPS = 2
# NODES = ["r2i2n1"]


@dataclass
class Stimulus:
    name: str
    start: float


@dataclass
class Simulation:
    spikes: libsonata.SpikePopulation
    cells: list[int]
    base_rotation: brayns.Rotation
    colors: list[brayns.Color4]


class Frames:
    def __init__(self, frames: list[float]) -> None:
        self._lock = RLock()
        self._index = 0
        self._frames = frames

    def get(self) -> tuple[int, float] | None:
        with self._lock:
            index = self._index

            if index == len(self._frames):
                return None

            frame = self._frames[index]

            self._index += 1

            return index, frame


@dataclass
class Render:
    name: str
    frames: Frames
    camera_rotation: brayns.Rotation
    camera_distance_factor: float
    camera_translation: brayns.Vector3

    @property
    def output_pattern(self) -> str:
        return f"{OUTPUT}/{self.name}/%d.png"


def parse_stimulus(line: str) -> Stimulus:
    start, name = line.split()
    return Stimulus(name, int(start))


def parse_stimuli() -> list[Stimulus]:
    with open(STIMULI) as file:
        data = file.read()
    return [parse_stimulus(line) for line in data.splitlines()]


def group_stimuli_by_name(stimuli: list[Stimulus]) -> dict[str, list[float]]:
    result = defaultdict[str, list[float]](list)

    for stimulus in stimuli:
        result[stimulus.name].append(stimulus.start)

    for key in IGNORE_STIMULI:
        del result[key]

    for key in result.keys():
        result[key] = result[key][:MAX_STIMULI]

    return result


def cleanup() -> None:
    output = Path(OUTPUT)
    shutil.rmtree(output, ignore_errors=True)
    output.mkdir()


def ensure_directory(pattern: str) -> None:
    path = Path(pattern).parent
    path.mkdir(parents=True, exist_ok=True)


def open_spike_population(simulation: int) -> libsonata.SpikePopulation:
    path = SPIKES.format(simulation=simulation)
    reader = libsonata.SpikeReader(path)
    return reader[POPULATION]


def get_column_cells() -> list[int]:
    with open(GIDS) as file:
        return json.load(file)


def get_cells_to_load(population: libsonata.SpikePopulation) -> list[int]:
    column_cells = get_column_cells()
    print(f"{len(column_cells)} cells in column")

    spikes = population.get(column_cells, TSTART, TSTOP)
    print(f"{len(spikes)} spikes in total")

    spiking_cells = {cell for cell, _ in spikes}
    print(f"{len(spiking_cells)} cells are spiking in column")

    column_cells = [cell for cell in column_cells if cell in spiking_cells]
    print(f"{len(column_cells)} cells selected before skip")

    cells = column_cells[::SKIP_FACTOR]
    print(f"{len(cells)} cells selected")

    return cells


def get_model_rotation(cells: list[int]) -> brayns.Rotation:
    storage = libsonata.NodeStorage(STORAGE)
    population = storage.open_population("All")

    xs, ys, zs, ws = (
        population.get_attribute(attribute, cells)
        for attribute in [
            "orientation_x",
            "orientation_y",
            "orientation_z",
            "orientation_w",
        ]
    )

    x, y, z, w = (sum(items) / len(items) for items in [xs, ys, zs, ws])

    quaternion = brayns.Quaternion(x, y, z, w)
    rotation = brayns.Rotation.from_quaternion(quaternion)

    return rotation.inverse


def parse_colors() -> list[brayns.Color4]:
    color3s = [brayns.parse_hex_color(color) for color in COLORS]

    colors = [brayns.Color4.from_color3(color3s[0], REST_ALPHA)]
    colors.extend(brayns.Color4.from_color3(color3) for color3 in color3s[1:])

    return colors


def parse_simulation(index: int) -> Simulation:
    population = open_spike_population(index)
    cells = get_cells_to_load(population)
    return Simulation(
        spikes=population,
        cells=cells,
        base_rotation=get_model_rotation(cells),
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


def load_circuit(
    instance: brayns.Instance, cells: list[int], rotation: brayns.Rotation
) -> brayns.Model:
    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name=POPULATION,
                nodes=brayns.SonataNodes.from_ids(cells),
                morphology=brayns.Morphology(
                    load_soma=True,
                    load_dendrites=True,
                ),
            )
        ]
    )

    model = loader.load_models(instance, PATH)[0]

    brayns.set_material(instance, model.id, MATERIAL)

    transform = brayns.Transform(rotation=rotation)

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
        time.sleep(1)

    for thread in threads:
        thread.join()


def load_models(node: str, simulation: Simulation) -> brayns.Model:
    with connect(node) as instance:
        brayns.clear_models(instance)

        add_lights(instance)

        print(f"Loading circuit for {node}")
        model = load_circuit(instance, simulation.cells, simulation.base_rotation)
        print(f"Circuit loaded for {node}")

        return model


def prepare_snapshot(instance: brayns.Instance, render: Render) -> brayns.Snapshot:
    resolution = RESOLUTION

    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=resolution.aspect_ratio,
    )
    camera = controller.camera

    camera.distance *= render.camera_distance_factor

    camera = camera.translate(render.camera_translation)

    camera = camera.rotate_around_target(render.camera_rotation)

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


def color_spikes(
    instance: brayns.Instance,
    model: brayns.Model,
    simulation: Simulation,
    current_time: float,
) -> None:
    spikes = read_spikes(simulation.spikes, simulation.cells, current_time)

    print(f"{len(spikes)} spikes at {current_time=}")

    intensities = get_spike_intensities(spikes, current_time)

    print(f"{len(intensities)} cells spiking at {current_time=}")

    method = brayns.CircuitColorMethod.ID

    colors = {
        str(cell): get_spike_color(simulation.colors, intensities.get(cell, 0.0))
        for cell in simulation.cells
    }

    brayns.color_model(instance, model.id, method, colors)


def export_frames(
    instance: brayns.Instance,
    model: brayns.Model,
    simulation: Simulation,
    render: Render,
) -> None:
    snapshot = prepare_snapshot(instance, render)

    while True:
        frame = render.frames.get()

        if frame is None:
            return

        index, current_time = frame

        print(f"Rendering frame {index}")

        color_spikes(instance, model, simulation, current_time)

        snapshot.save(instance, render.output_pattern % index)

        print(f"Frame {index} rendered")


def export_simulation(node: str, simulation: Simulation, render: Render) -> None:
    with connect(node) as instance:
        model = next(
            model for model in brayns.get_models(instance) if model.type == "neurons"
        )
        export_frames(instance, model, simulation, render)


def get_stimulus_frames(stimuli: list[float]) -> list[float]:
    result = list[float]()

    for stimulus in stimuli:
        start = int(stimulus)
        stop = start + STIMULUS_DURATION
        step = 1000 // FPS
        result.extend(range(start, stop, step))

    return result


def make_movie(pattern: str) -> None:
    output = Path(pattern).parent / "movie.mp4"
    movie = brayns.Movie(pattern, FPS)
    movie.save(str(output))


def run(simulation: Simulation, render: Render) -> None:
    ensure_directory(render.output_pattern)

    print(f"Exporting frames for {render.name}")
    run_in_parallel(export_simulation, [(node, simulation, render) for node in NODES])
    print(f"Exported frames for {render.name}")

    print(f"Exporting movie for {render.name}")
    make_movie(render.output_pattern)
    print(f"Exported movie for {render.name}")


def main() -> None:
    raw_stimuli = parse_stimuli()
    stimuli = group_stimuli_by_name(raw_stimuli)

    cleanup()

    print("Parsing simulation")
    simulation = parse_simulation(SIMULATION)
    print("Parsed simulation")

    print("Loading models")
    run_in_parallel(load_models, [(node, simulation) for node in NODES])
    print("Models loaded")

    renders = [
        Render(
            name="top_10s",
            frames=Frames(list(range(TSTART, TSTOP, 1000 // FPS))),
            camera_rotation=brayns.CameraRotation.top,
            camera_distance_factor=CAMERA_DISTANCE_FACTOR,
            camera_translation=CAMERA_TRANSLATION,
        ),
        *[
            Render(
                name=f"top_{stimulus}",
                frames=Frames(get_stimulus_frames(stimuli[stimulus])),
                camera_rotation=brayns.CameraRotation.top,
                camera_distance_factor=CAMERA_DISTANCE_FACTOR,
                camera_translation=CAMERA_TRANSLATION,
            )
            for stimulus in stimuli.keys()
        ],
        *[
            Render(
                name=f"side_{stimulus}",
                frames=Frames(get_stimulus_frames(stimuli[stimulus])),
                camera_rotation=brayns.CameraRotation.front,
                camera_distance_factor=CAMERA_DISTANCE_FACTOR,
                camera_translation=CAMERA_TRANSLATION,
            )
            for stimulus in stimuli.keys()
        ],
    ]

    for render in renders:
        run(simulation, render)


if __name__ == "__main__":
    main()
