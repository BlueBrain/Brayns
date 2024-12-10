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

PATH1 = "/gpfs/bbp.cscs.ch/data/scratch/proj83/home/pokorny/SimplifiedConnectomeModels/simulations_v2/SSCx-HexO1-Release-TC__Evoked/seed=628462/simulation_config.json"
PATH2 = "/gpfs/bbp.cscs.ch/data/scratch/proj83/home/pokorny/SimplifiedConnectomeModels/simulations_v2/SSCx-HexO1-Release-TC__Evoked__ConnRewireOrder3Hex0EE/seed=628462/simulation_config.json"
PATH3 = "/gpfs/bbp.cscs.ch/data/scratch/proj83/home/pokorny/SimplifiedConnectomeModels/simulations_v2/SSCx-HexO1-Release-TC__Evoked__ConnRewireOrder5Hex0EE/seed=628462/simulation_config.json"

PATH1 = "/gpfs/bbp.cscs.ch/home/acfleury/src/Test/mopro690/simulation_config_1.json"
PATH2 = "/gpfs/bbp.cscs.ch/home/acfleury/src/Test/mopro690/simulation_config_2.json"
PATH3 = "/gpfs/bbp.cscs.ch/home/acfleury/src/Test/mopro690/simulation_config_3.json"
PATHS = [PATH1, PATH2, PATH3]

SIMULATIONS = list(range(3))

OUTPUT = "/home/acfleury/source/test/scripts/mopro690/frames/{simulation}/{view}/%d.png"
POPULATION = "S1nonbarrel_neurons"
SKIP_FACTOR = 2

FPS = 25
SPIKE_TRANSITION_TIME_UP = 500
SPIKE_TRANSITION_TIME_DOWN = 1000

SLOWING_FACTOR = 3
TSTART = 0
TSTOP = 12_000
DURATION = TSTOP - TSTART

CAMERA_DISTANCE_FACTOR = 1.0
CAMERA_TRANSLATION = brayns.Vector3(0, 0, 0)
ROTATIONS = 1

TEST = False

BACKGROUND = brayns.Color4.black.transparent

if TEST:
    RENDERER = brayns.InteractiveRenderer(background_color=BACKGROUND)
    RESOLUTION = brayns.Resolution.ultra_hd
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
# SIMULATIONS = [0, 1, 2]
# RESOLUTION = brayns.Resolution.full_hd
# FPS = 1


def get_frames_to_render() -> list[float]:
    frames = list[float]()

    frame_count = math.ceil(SLOWING_FACTOR * DURATION * FPS / 1000)

    for i in range(frame_count):
        t = TSTART + DURATION * i / (frame_count - 1)
        frames.append(t)

    return frames


FRAMES = get_frames_to_render()


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
    distance_factor: float
    translation: brayns.Vector3
    rotate: bool = False


@dataclass
class Render:
    simulation: Simulation
    view: View
    frames: Frames
    output_pattern: str


VIEWS = [
    View(
        "front",
        brayns.CameraRotation.front,
        CAMERA_DISTANCE_FACTOR * 0.9,
        CAMERA_TRANSLATION + brayns.Vector3(0, 150, 0),
        rotate=True,
    ),
    View(
        "top",
        brayns.CameraRotation.top,
        CAMERA_DISTANCE_FACTOR,
        CAMERA_TRANSLATION,
    ),
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
    print(f"{selection.flat_size} cells in column")

    spikes = population.get(selection, TSTART, TSTOP)
    print(f"{len(spikes)} spikes in total")

    spiking_cells = {cell for cell, _ in spikes}
    print(f"{len(spiking_cells)} cells are spiking in column")

    column_cells = [int(cell) for cell in selection.flatten() if cell in spiking_cells]
    print(f"{len(column_cells)} cells selected before skip")

    cells = column_cells[::SKIP_FACTOR]
    print(f"{len(cells)} cells selected")

    return cells


def get_model_rotation(
    cells: list[int], nodes: libsonata.NodePopulation
) -> brayns.Rotation:
    xs, ys, zs, ws = (
        nodes.get_attribute(attribute, cells)
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
    path = PATHS[index]

    simulation = libsonata.SimulationConfig.from_file(path)
    circuit = libsonata.CircuitConfig.from_file(simulation.network)
    nodes = circuit.node_population(POPULATION)

    spike_file = f"{simulation.output.output_dir}/{simulation.output.spikes_file}"
    reader = libsonata.SpikeReader(spike_file)
    spikes = reader[POPULATION]

    cells = get_cells_to_load(nodes, spikes)

    return Simulation(
        index=index,
        circuit=simulation.network,
        spikes=spikes,
        frames=FRAMES,
        cells=cells,
        base_rotation=get_model_rotation(cells, nodes),
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
        rotation=view.rotation,
    )
    camera = controller.camera

    camera.distance *= view.distance_factor

    camera = camera.translate(view.translation)

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
    instance: brayns.Instance, model: brayns.Model, render: Render
) -> None:
    snapshot = prepare_snapshot(instance, render.view)
    original_camera = snapshot.camera

    assert original_camera is not None

    while True:
        frame = render.frames.get()

        if frame is None:
            return

        index, current_time = frame

        print(f"Rendering frame {index}")

        if render.view.rotate:
            speed = ROTATIONS / DURATION
            angle = 360 * speed * current_time
            rotation = brayns.euler(0, angle, 0, degrees=True)
            snapshot.camera = original_camera.rotate_around_target(rotation)

        color_spikes(instance, model, render.simulation, current_time)

        snapshot.save(instance, render.output_pattern % index)

        print(f"Frame {index} rendered")


def export_simulation(node: str, render: Render) -> None:
    with connect(node) as instance:
        print(f"Exporting frames for node {node}")

        model = next(
            model for model in brayns.get_models(instance) if model.type == "neurons"
        )

        export_frames(instance, model, render)

        print(f"All frames rendered for {node}")


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

        frames = Frames(simulation.frames)

        pattern = get_output_pattern(simulation_index, view.name)

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
