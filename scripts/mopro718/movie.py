import json
from dataclasses import dataclass
from pathlib import Path
from threading import RLock, Thread

import brayns
import libsonata

PATH = "/gpfs/bbp.cscs.ch/project/proj3/cloned_circuits/FullBrainSynthesizedWithAttributes/circuit_config.json"
POPULATION = "root__neurons"

NODE_SET = "All"
SOLID_COLOR = brayns.Color4(1.0, 0.5, 0)
MTYPE_COLORS = "/gpfs/bbp.cscs.ch/home/acfleury/src/Test/mopro718/colors.json"
OUTPUT = "/gpfs/bbp.cscs.ch/home/acfleury/src/Test/mopro718/frames/%d.png"
CAMERA_POSITION = brayns.Vector3(-11333.343, -7163.718, -10376.665)
CAMERA_ROTATION = brayns.euler(153.6, 48, 0, degrees=True)
CELLS = "/gpfs/bbp.cscs.ch/home/acfleury/src/Test/mopro718/fullbrain_cells.json"
RESAMPLING = 0.9
SUBSAMPLING = 4
SKIP_FACTOR = 1

FOVY = brayns.Fovy(29, degrees=True)

RESOLUTION = brayns.Resolution.ultra_hd

BACKGROUND = brayns.Color4.black.transparent

# RENDERER = brayns.ProductionRenderer(background_color=BACKGROUND)
RENDERER = brayns.InteractiveRenderer(background_color=BACKGROUND)

MATERIAL = (
    brayns.PrincipledMaterial(
        roughness=0.35,
        specular=0.5,
    )
    if isinstance(RENDERER, brayns.ProductionRenderer)
    else brayns.PhongMaterial()
)

FPS = 25
# FPS = 1

SPAWN_START = 0
SPAWN_DURATION = 5

GROWTH_START = SPAWN_START + SPAWN_DURATION
GROWTH_DURATION = 10

COLOR_START = GROWTH_START + GROWTH_DURATION
COLOR_DURATION = 5

ZOOM_START = COLOR_START + COLOR_DURATION
ZOOM_DURATION = 10
MAX_ZOOM = 0.2

DURATION = ZOOM_START + ZOOM_DURATION

FRAMES = list(range(DURATION * FPS + 1))
# FRAMES = [6]

with open("/gpfs/bbp.cscs.ch/home/acfleury/src/Test/test.txt") as file:
    NODES = file.read().split()

# NODES = ["r1i6n34"]


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
class Render:
    node_set: str
    visible_cells: set[int] | None
    resampling: float
    subsampling: int
    skip_factor: int
    solid_color: brayns.Color4
    mtype_colors: dict[str, brayns.Color4]
    output: str
    camera_position: brayns.Vector3
    camera_rotation: brayns.Rotation
    use_spheres: bool = False
    radius_multiplier: float = 10


def cleanup(output: str) -> None:
    frames = Path(output).parent

    if not frames.exists():
        frames.mkdir(parents=True, exist_ok=True)
        return

    for frame in frames.glob("*.png"):
        frame.unlink()


def parse_visible_cells(path: Path) -> set[int]:
    with path.open() as file:
        return set(json.load(file))


def parse_mtype_colors() -> dict[str, brayns.Color4]:
    with open(MTYPE_COLORS) as file:
        data = json.load(file)
        assert isinstance(data, dict)

    return {mtype: brayns.Color4(*color) for mtype, color in data.items()}


def parse_cells(node_set: str) -> list[int]:
    config = libsonata.CircuitConfig.from_file(PATH)
    nodes = config.node_population(POPULATION)

    node_sets = libsonata.NodeSets.from_file(config.node_sets_path)
    selection = node_sets.materialize(node_set, nodes)

    cells = [int(cell) for cell in selection.flatten()]

    ys = [float(y) for y in nodes.get_attribute("y", cells)]

    cells = [cell for _, cell in sorted(zip(ys, cells), key=lambda x: -x[0])]

    return cells


def add_lights(instance: brayns.Instance, camera_rotation: brayns.Rotation) -> None:
    brayns.add_light(instance, brayns.AmbientLight(0.3))
    brayns.add_light(
        instance,
        brayns.DirectionalLight(
            10, direction=camera_rotation.apply(brayns.Vector3(1, -1, -1))
        ),
    )


def start_loading(
    instance: brayns.Instance,
    cells: list[int],
    render: Render,
    density: float,
    growth: float,
) -> brayns.Future[list[brayns.Model]] | None:
    if density == 1.0 and render.visible_cells is not None:
        cells = [cell for cell in cells if cell in render.visible_cells]

    morphologies = growth > 0 or not render.use_spheres

    cells = cells[:: render.skip_factor]
    cell_count = int(len(cells) * density)
    cells = cells[:cell_count]

    if not cells:
        return None

    radius_multiplier = 1 if morphologies else render.radius_multiplier

    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name=POPULATION,
                nodes=brayns.SonataNodes.from_ids(cells),
                morphology=brayns.Morphology(
                    radius_multiplier=radius_multiplier,
                    load_soma=morphologies,
                    load_dendrites=morphologies,
                    load_axon=False,
                    resampling=render.resampling,
                    subsampling=render.subsampling,
                    growth=growth,
                ),
            )
        ]
    )

    return loader.load_models_task(instance, PATH)


def load_circuit(
    instance: brayns.Instance,
    cells: list[int],
    render: Render,
    density: float = 1.0,
    growth: float = 1.0,
) -> brayns.Model | None:
    print(f"Loading model with {density=} and {growth=}")

    future = start_loading(instance, cells, render, density, growth)

    if future is None:
        return None

    model = future.wait_for_result()[0]

    print(f"Loaded model with {density=} and {growth=}")

    brayns.set_material(instance, model.id, MATERIAL)

    return model


def get_progress(frame: int, start: int, duration: int) -> float:
    if frame <= start * FPS:
        return 0.0

    if frame >= (start + duration) * FPS:
        return 1.0

    return (frame - start * FPS) / (duration * FPS)


def get_density(frame: int) -> float:
    return get_progress(frame, SPAWN_START, SPAWN_DURATION)


def get_growth(frame: int) -> float:
    return get_progress(frame, GROWTH_START, GROWTH_DURATION)


def get_coloring(frame: int) -> float:
    return get_progress(frame, COLOR_START, COLOR_DURATION)


def get_zoom(frame: int) -> float:
    return get_progress(frame, ZOOM_START, ZOOM_DURATION)


def focus_camera(
    instance: brayns.Instance, render: Render, zoom: float
) -> brayns.Camera:
    position = render.camera_position
    direction = render.camera_rotation.apply(-brayns.Axis.z)
    up = render.camera_rotation.apply(brayns.Axis.y)

    target = brayns.get_bounds(instance).center
    distance = (position - target).norm

    camera_view = brayns.View(position, position + direction * distance, up)

    projection = brayns.PerspectiveProjection(FOVY)

    camera = brayns.Camera(camera_view, projection)

    zoom_factor = 1 - (1 - MAX_ZOOM) * zoom

    camera.distance *= zoom_factor

    return camera


def snapshot(node: str, instance: brayns.Instance, render: Render, frame: int) -> None:
    print(f"Rendering {frame=} for {node=}")

    zoom = get_zoom(frame)
    camera = focus_camera(instance, render, zoom)

    snapshot = brayns.Snapshot(RESOLUTION, camera, RENDERER)
    snapshot.save(instance, render.output % frame)

    print(f"Rendered {frame=} for {node=}")


def export_growth(
    node: str,
    instance: brayns.Instance,
    cells: list[int],
    render: Render,
    frame: int,
    density: float,
    growth: float,
) -> None:
    model = load_circuit(instance, cells, render, density, growth)

    if model is not None:
        brayns.set_model_color(instance, model.id, render.solid_color)

    snapshot(node, instance, render, frame)

    if model is not None:
        brayns.remove_models(instance, [model.id])


def export_coloring(
    node: str,
    instance: brayns.Instance,
    cells: list[int],
    render: Render,
    frame: int,
    progress: float,
) -> None:
    pivot = int(len(cells) * progress)

    non_colored_cells = cells[pivot:]

    non_colored_model = load_circuit(instance, non_colored_cells, render)
    assert non_colored_model is not None

    brayns.set_model_color(instance, non_colored_model.id, render.solid_color)

    colored_cells = cells[:pivot]
    colored_model = load_circuit(instance, colored_cells, render)

    if colored_model is not None:
        method = brayns.CircuitColorMethod.MTYPE
        brayns.color_model(instance, colored_model.id, method, render.mtype_colors)

    snapshot(node, instance, render, frame)

    brayns.remove_models(instance, [non_colored_model.id])

    if colored_model is not None:
        brayns.remove_models(instance, [colored_model.id])


def export_frames(node: str, cells: list[int], render: Render, frames: Frames) -> None:
    uri = f"{node}:5000"
    connector = brayns.Connector(uri)

    print(f"Connecting to {node}")

    with connector.connect() as instance:
        print(f"Connected to {node=}")

        brayns.clear_models(instance)

        add_lights(instance, render.camera_rotation)

        exporting_zoom = False

        while True:
            frame = frames.get()

            if frame is None:
                print(f"All frames exported for {node=}")
                return

            density = get_density(frame)
            growth = get_growth(frame)

            if density < 1.0 or growth < 1.0:
                export_growth(node, instance, cells, render, frame, density, growth)
                continue

            coloring = get_coloring(frame)

            if coloring < 1.0:
                export_coloring(node, instance, cells, render, frame, coloring)
                continue

            if not exporting_zoom:
                model = load_circuit(instance, cells, render)
                assert model is not None

                method = brayns.CircuitColorMethod.MTYPE
                brayns.color_model(instance, model.id, method, render.mtype_colors)

                exporting_zoom = True

            snapshot(node, instance, render, frame)


def make_movie(output: str) -> None:
    path = Path(output).parent / "movie.mp4"
    movie = brayns.Movie(output, FPS)
    movie.save(str(path))


def run(render: Render) -> None:
    cleanup(render.output)

    print("Parsing circuit")
    cells = parse_cells(render.node_set)
    print("Circuit parsed")

    frames = Frames(FRAMES)

    threads = [
        Thread(target=export_frames, args=(node, cells, render, frames))
        for node in NODES
    ]

    print("Exporting frames")

    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()

    print("Exported frames")

    make_movie(render.output)


def main() -> None:
    full_brain = Render(
        node_set=NODE_SET,
        visible_cells=parse_visible_cells(Path(CELLS)),
        resampling=RESAMPLING,
        subsampling=SUBSAMPLING,
        skip_factor=SKIP_FACTOR,
        solid_color=SOLID_COLOR,
        mtype_colors=parse_mtype_colors(),
        output=OUTPUT,
        camera_position=CAMERA_POSITION,
        camera_rotation=CAMERA_ROTATION,
        use_spheres=True,
    )
    run(full_brain)


if __name__ == "__main__":
    main()
