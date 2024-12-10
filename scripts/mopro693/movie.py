import json
import shutil
from collections.abc import Iterable
from dataclasses import dataclass
from pathlib import Path
from threading import RLock, Thread

import brayns
import libsonata

PATH = "/gpfs/bbp.cscs.ch/project/proj96/home/ecker/simulations/d21efd45-7740-4268-b06f-e1de35f2b6cf/1/BlueConfig"
PATH = "/gpfs/bbp.cscs.ch/home/acfleury/src/Test/simulation_config_mopro693.json"

SPIKE_FILE = "/gpfs/bbp.cscs.ch/project/proj96/home/ecker/simulations/d21efd45-7740-4268-b06f-e1de35f2b6cf/1/out.h5"
STORAGE = "/gpfs/bbp.cscs.ch/project/proj83/circuits/Bio_M/20200805/sonata/networks/nodes/All/nodes.h5"

ASSEMBLY_TIMESTAMPS = "/gpfs/bbp.cscs.ch/project/proj96/home/ecker/simulations/d21efd45-7740-4268-b06f-e1de35f2b6cf/viz/assemblies.txt"
ASSEMBLY_CELLS = "/gpfs/bbp.cscs.ch/project/proj96/home/ecker/simulations/d21efd45-7740-4268-b06f-e1de35f2b6cf/viz/assemblies.json"
ASSEMBLY_COLORS = "/gpfs/bbp.cscs.ch/project/proj96/home/ecker/simulations/d21efd45-7740-4268-b06f-e1de35f2b6cf/viz/colors.txt"
COLUMN_CELLS = "/home/acfleury/source/test/scripts/mopro693/column_cells.json"

ASSEMBLY_PATTERN = "/home/acfleury/source/test/scripts/mopro693/frames/assembly/%d.png"
SIMULATION_PATTERN = (
    "/home/acfleury/source/test/scripts/mopro693/frames/simulation/%d.png"
)
SKIP_FACTOR = 5
RADIUS_MULTIPLIER = 8
POPULATION = "All"
COLUMN_TARGET = "hex_O1"

ZOOM = 1.1
TRANSLATION = brayns.Vector3(0, 100, 0)

COLORS = [
    "#171786",
    "#0064BD",
    "#60099B",
    "#AC005A",
    "#DB4400",
    "#E98B00",
    "#FAF8B9",
]

SIMULATION = True
ASSEMBLIES = True

FPS = 25
SLOWING_FACTOR = 10
TSTART = 1500
TSTOP = 7000
# TSTART = 7400
# TSTOP = 7900
DURATION = TSTOP - TSTART
TIMEBIN_DURATION = 20

SPIKE_TRANSITION_TIME = TIMEBIN_DURATION

BACKGROUND = brayns.Color4.black.transparent

TEST = False

if TEST:
    RENDERER = brayns.InteractiveRenderer(background_color=BACKGROUND)
    RESOLUTION = brayns.Resolution.full_hd
    MATERIAL = brayns.PhongMaterial()
else:
    RENDERER = brayns.ProductionRenderer(background_color=BACKGROUND)
    RESOLUTION = brayns.Resolution.ultra_hd
    MATERIAL = brayns.PrincipledMaterial(
        roughness=0.4,
        ior=1.52,
    )

with open("/gpfs/bbp.cscs.ch/home/acfleury/src/Test/test.txt") as file:
    NODES = file.read().split()

MODEL_ROTATION = brayns.Rotation.from_quaternion(
    brayns.Quaternion(
        0.7646707222538381,
        0.4145383245573556,
        0.006126413543916389,
        0.4933549746967825,
    )
).inverse


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

            if index >= len(self._frames):
                return None

            frame = self._frames[index]

            self._index += 1

            return index, frame


@dataclass
class Assembly:
    id: int
    cells: set[int]
    color: brayns.Color4


@dataclass
class Timebin:
    timestamp: int
    assembly_id: int


@dataclass
class Circuit:
    column_cells: set[int]
    positions: dict[int, brayns.Vector3]
    assemblies_by_id: dict[int, Assembly]
    assemblies_by_timestamp: dict[int, Assembly]
    spikes: libsonata.SpikePopulation

    @property
    def assembly_cells(self) -> set[int]:
        return {
            cell
            for assembly in self.assemblies_by_id.values()
            for cell in assembly.cells
        }

    @property
    def all_assemblies(self) -> Iterable[Assembly]:
        return self.assemblies_by_id.values()

    def find_assembly_by_id(self, id: int) -> Assembly | None:
        return self.assemblies_by_id.get(id)

    def find_assembly_by_timestamp(self, timestamp: int) -> Assembly | None:
        t = (timestamp // TIMEBIN_DURATION) * TIMEBIN_DURATION
        return self.assemblies_by_timestamp.get(t)

    def get_spiking_cells(self, tstart: int, tstop: int) -> set[int]:
        frame = self.spikes.get(tstart=tstart, tstop=tstop)
        return {cell for cell, _ in frame}


def cleanup(pattern: str) -> None:
    output = Path(pattern).parent
    shutil.rmtree(output, ignore_errors=True)
    output.mkdir(parents=True, exist_ok=True)


def add_lights(instance: brayns.Instance) -> None:
    brayns.clear_lights(instance)

    light = brayns.AmbientLight(intensity=0.5)
    brayns.add_light(instance, light)


def add_camera_lights(instance: brayns.Instance, camera: brayns.Camera) -> list[int]:
    left = brayns.add_light(
        instance,
        brayns.DirectionalLight(
            intensity=5,
            direction=brayns.euler(0, -45, 0, degrees=True).apply(camera.direction),
        ),
    )

    right = brayns.add_light(
        instance,
        brayns.DirectionalLight(
            intensity=2,
            direction=brayns.euler(0, 45, 0, degrees=True).apply(camera.direction),
        ),
    )

    back = brayns.add_light(
        instance,
        brayns.DirectionalLight(
            intensity=5,
            direction=brayns.euler(0, 180, 0, degrees=True).apply(camera.direction),
        ),
    )

    return [left.id, right.id, back.id]


def parse_timebins() -> list[Timebin]:
    with open(ASSEMBLY_TIMESTAMPS) as file:
        data = file.read()

    result = list[Timebin]()

    for line in data.splitlines():
        timestamp, assembly_id = line.split()
        timebin = Timebin(int(timestamp), int(assembly_id))
        result.append(timebin)

    return result


def parse_assembly_cells() -> dict[int, set[int]]:
    with open(ASSEMBLY_CELLS) as file:
        data: dict[str, list[int]] = json.load(file)

    return {int(id): {cell - 1 for cell in cells} for id, cells in data.items()}


def parse_column_cells() -> list[int]:
    with open(COLUMN_CELLS) as file:
        data: list[int] = json.load(file)

    return [cell - 1 for cell in data]


def parse_assembly_colors() -> dict[int, brayns.Color4]:
    with open(ASSEMBLY_COLORS) as file:
        data = file.read()

    colors = dict[int, brayns.Color4]()

    for line in data.splitlines():
        assembly_id, color_hex = line.split()
        color3 = brayns.parse_hex_color(color_hex)
        colors[int(assembly_id)] = brayns.Color4.from_color3(color3)

    return colors


def parse_assemblies() -> dict[int, Assembly]:
    cells = parse_assembly_cells()
    colors = parse_assembly_colors()

    return {
        assembly_id: Assembly(assembly_id, cells[assembly_id], colors[assembly_id])
        for assembly_id in cells.keys()
    }


def map_assemblies_by_timestamp(assemblies: dict[int, Assembly]) -> dict[int, Assembly]:
    timebins = parse_timebins()
    return {timebin.timestamp: assemblies[timebin.assembly_id] for timebin in timebins}


def get_cell_positions(cells: list[int]) -> list[brayns.Vector3]:
    storage = libsonata.NodeStorage(STORAGE)
    nodes = storage.open_population(POPULATION)

    selection = libsonata.Selection(list(cells))

    xs = nodes.get_attribute("x", selection)
    ys = nodes.get_attribute("y", selection)
    zs = nodes.get_attribute("z", selection)

    return [brayns.Vector3(x, y, z) for x, y, z in zip(xs, ys, zs)]


def open_spike_population() -> libsonata.SpikePopulation:
    reader = libsonata.SpikeReader(SPIKE_FILE)
    return reader[POPULATION]


def parse_circuit() -> Circuit:
    assemblies_by_id = parse_assemblies()
    assemblies_by_timestamp = map_assemblies_by_timestamp(assemblies_by_id)

    column_cells = parse_column_cells()
    positions = get_cell_positions(column_cells)

    return Circuit(
        column_cells=set(column_cells),
        positions={
            int(cell): position for cell, position in zip(column_cells, positions)
        },
        assemblies_by_id=assemblies_by_id,
        assemblies_by_timestamp=assemblies_by_timestamp,
        spikes=open_spike_population(),
    )


def load_spheres(
    instance: brayns.Instance, positions: list[brayns.Vector3]
) -> brayns.Model:
    spheres = [
        (brayns.Sphere(RADIUS_MULTIPLIER, position), brayns.Color4.white)
        for position in positions
    ]

    model = brayns.add_geometries(instance, spheres)

    brayns.set_material(instance, model.id, MATERIAL)

    transform = brayns.Transform(rotation=MODEL_ROTATION)
    model = brayns.update_model(instance, model.id, transform)

    return model


def load_circuit(instance: brayns.Instance, circuit: Circuit) -> brayns.Model:
    spiking_cells = circuit.get_spiking_cells(TSTART, TSTOP)
    column_cells = circuit.column_cells

    cells = spiking_cells & column_cells
    cells = list(cells)[::SKIP_FACTOR]

    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name=POPULATION,
                nodes=brayns.SonataNodes.from_ids(cells),
                report=brayns.SonataReport.spikes(SPIKE_TRANSITION_TIME),
                morphology=brayns.Morphology(
                    load_soma=True,
                    load_dendrites=True,
                ),
            )
        ]
    )

    model = loader.load_models(instance, PATH)[0]

    brayns.set_material(instance, model.id, MATERIAL)

    transform = brayns.Transform(rotation=MODEL_ROTATION)
    model = brayns.update_model(instance, model.id, transform)

    colors = brayns.ColorRamp(
        value_range=brayns.ValueRange(0, 1),
        colors=[
            brayns.Color4.from_color3(brayns.parse_hex_color(color)) for color in COLORS
        ],
    )

    brayns.set_color_ramp(instance, model.id, colors)

    return model


def focus_camera(instance: brayns.Instance) -> brayns.Camera:
    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=RESOLUTION.aspect_ratio,
    )
    camera = controller.camera

    camera.distance /= ZOOM

    camera = camera.translate(TRANSLATION)

    return camera


def load_assembly(
    instance: brayns.Instance, circuit: Circuit, timestamp: int, assembly: Assembly
) -> brayns.Model:
    tstart = (timestamp // TIMEBIN_DURATION) * TIMEBIN_DURATION
    tstop = tstart + TIMEBIN_DURATION
    spiking_cells = circuit.get_spiking_cells(tstart, tstop)

    print(f"{timestamp}: {len(spiking_cells)} cells spiking")

    cells = spiking_cells & assembly.cells

    print(f"    {len(cells)} cells spiking in assembly {assembly.id}")

    positions = [circuit.positions[cell] for cell in cells]

    model = load_spheres(instance, positions)

    brayns.set_model_color(instance, model.id, assembly.color)

    return model


def export_assemblies(
    instance: brayns.Instance,
    circuit: Circuit,
    camera: brayns.Camera,
    frames: Frames,
) -> None:
    snapshot = brayns.Snapshot(RESOLUTION, camera, RENDERER)

    while True:
        frame = frames.get()

        if frame is None:
            return

        index, timestamp = frame

        angle = index / DURATION * 360
        rotation = brayns.euler(0, angle, 0, degrees=True)

        snapshot.camera = camera.rotate_around_target(rotation)

        lights = add_camera_lights(instance, snapshot.camera)

        assembly = circuit.find_assembly_by_timestamp(timestamp)

        if assembly is None:
            print(f"No assemblies at time {timestamp}")
            model = None
        else:
            print(f"Using assembly {assembly.id} for timestamp {timestamp}")
            model = load_assembly(instance, circuit, timestamp, assembly)

        snapshot.save(instance, ASSEMBLY_PATTERN % index)

        brayns.remove_models(instance, lights)

        if model is not None:
            brayns.remove_models(instance, [model.id])

        print(f"Assembly frame {index} rendered")


def export_simulation(
    instance: brayns.Instance, camera: brayns.Camera, frames: Frames
) -> None:
    snapshot = brayns.Snapshot(RESOLUTION, camera, RENDERER)

    simulation = brayns.get_simulation(instance)

    while True:
        frame = frames.get()

        if frame is None:
            return

        index, timestamp = frame

        angle = index / DURATION * 360
        rotation = brayns.euler(0, angle, 0, degrees=True)

        snapshot.camera = camera.rotate_around_target(rotation)

        lights = add_camera_lights(instance, snapshot.camera)

        i = simulation.get_frame(timestamp)
        brayns.set_simulation_frame(instance, i)

        snapshot.save(instance, SIMULATION_PATTERN % index)

        brayns.remove_models(instance, lights)

        print(f"Simulation frame {index=} {timestamp=} rendered")


def run(
    node: str,
    circuit: Circuit,
    simulation_frames: Frames | None,
    assembly_frames: Frames | None,
) -> None:
    print(f"Connecting to {node=}")
    connector = brayns.Connector(f"{node}:5000", max_attempts=None)

    with connector.connect() as instance:
        print(f"Connected to {node=}:", brayns.get_version(instance))

        if simulation_frames is not None:
            brayns.clear_models(instance)

            print(f"Loading circuit for {node=}")
            load_circuit(instance, circuit)
            print(f"Loaded circuit for {node=}")

            camera = focus_camera(instance)

            add_lights(instance)

            print(f"Exporting simulation frames for {node=}")
            export_simulation(instance, camera, simulation_frames)
            print(f"Simulation frames rendered for {node=}")

        if assembly_frames is not None:
            brayns.clear_models(instance)

            add_lights(instance)

            print(f"Exporting assembly frames for {node=}")
            export_assemblies(instance, circuit, camera, assembly_frames)
            print(f"Assembly frames rendered for {node=}")

        print(f"Frames rendered for {node=}")


def main() -> None:
    if SIMULATION:
        cleanup(SIMULATION_PATTERN)

    if ASSEMBLIES:
        cleanup(ASSEMBLY_PATTERN)

    print("Parsing circuit")
    circuit = parse_circuit()
    print("Parsed circuit")

    duration = (TSTOP - TSTART) * SLOWING_FACTOR
    frame_count = FPS * duration // 1000
    step = (TSTOP - TSTART) // frame_count

    frames = list(range(TSTART, TSTOP + step, step))

    simulation_frames = Frames(frames) if SIMULATION else None
    assembly_frames = Frames(frames) if ASSEMBLIES else None

    threads = [
        Thread(
            target=run,
            args=(node, circuit, simulation_frames, assembly_frames),
        )
        for node in NODES
    ]

    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()

    if SIMULATION:
        movie = brayns.Movie(SIMULATION_PATTERN, FPS)
        movie.save("mopro693_simulation.mp4")

    if ASSEMBLIES:
        movie = brayns.Movie(ASSEMBLY_PATTERN, FPS)
        movie.save("mopro693_assemblies.mp4")


if __name__ == "__main__":
    main()
