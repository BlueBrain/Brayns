import json
from dataclasses import dataclass
from pathlib import Path
from threading import RLock, Thread

import brayns
import libsonata

OUTPUT_CELLS = "/home/acfleury/source/test/scripts/sfn/cells.json"

PATH = "/gpfs/bbp.cscs.ch/project/proj3/cloned_circuits/FullBrainSynthesized/circuit_config.json"
# PATH = "/gpfs/bbp.cscs.ch/data/scratch/proj134/fullcircuit/full-synth-more-split-v2/circuit_config.json"
POPULATION = "default"
# POPULATION = "root__neurons"

THALAMUS = "TH"
THALAMUS_COLOR = "#9A3F90"
THALAMUS_OUTPUT = "/home/acfleury/source/test/scripts/sfn/frames_thalamus/%d.png"
THALAMUS_CAMERA_POSITION = brayns.Vector3(-2479.504, -490.174, -3171.017)
THALAMUS_CAMERA_ROTATION = brayns.euler(162, 49.2, 0, degrees=True)
THALAMUS_CELLS = "/home/acfleury/source/test/scripts/sfn/thalamus_cells.json"
THALAMUS_RESAMPLING = 0.99
THALAMUS_SUBSAMPLING = 2
THALAMUS_SKIP_FACTOR = 1

HIPPOCAMPUS = "CA1"
HIPPOCAMPUS_COLOR = "#D14412"
HIPPOCAMPUS_OUTPUT = "/home/acfleury/source/test/scripts/sfn/frames_hippo/%d.png"
HIPPOCAMPUS_CAMERA_POSITION = brayns.Vector3(-910.239, 1621.451, -5344.393)
HIPPOCAMPUS_CAMERA_ROTATION = brayns.euler(171.6, 42, 0, degrees=True)
HIPPOCAMPUS_CELLS = "/home/acfleury/source/test/scripts/sfn/hippocampus_cells.json"
HIPPOCAMPUS_RESAMPLING = 0.99
HIPPOCAMPUS_SUBSAMPLING = 2
HIPPOCAMPUS_SKIP_FACTOR = 1

SSCX = "SS"
SSCX_COLOR = "#0F78C8"
SSCX_OUTPUT = "/home/acfleury/source/test/scripts/sfn/frames_sscx/%d.png"
SSCX_CAMERA_POSITION = brayns.Vector3(-4921.211, -2668.256, -6578.021)
SSCX_CAMERA_ROTATION = brayns.euler(160.8, 43.2, 0, degrees=True)
SSCX_CELLS = "/home/acfleury/source/test/scripts/sfn/sscx_cells.json"
SSCX_RESAMPLING = 0.99
SSCX_SUBSAMPLING = 2
SSCX_SKIP_FACTOR = 1

NEOCORTEX = "Isocortex"
NEOCORTEX_COLOR = "#CF0E1B"
NEOCORTEX_OUTPUT = "/home/acfleury/source/test/scripts/sfn/frames_neocortex/%d.png"
NEOCORTEX_CAMERA_POSITION = brayns.Vector3(-5351.59, -2522.193, -7304.59)
NEOCORTEX_CAMERA_ROTATION = brayns.euler(160.8, 43.2, 0, degrees=True)
NEOCORTEX_CELLS = "/home/acfleury/source/test/scripts/sfn/neocortex_cells.json"
NEOCORTEX_RESAMPLING = 0.9
NEOCORTEX_SUBSAMPLING = 4
NEOCORTEX_SKIP_FACTOR = 1

FULL_BRAIN = "All"
FULL_BRAIN_COLORS = "/home/acfleury/source/test/scripts/sfn/colors.json"
FULL_BRAIN_OUTPUT = "/home/acfleury/source/test/scripts/sfn/frames_fullbrain/%d.png"
FULL_BRAIN_CAMERA_POSITION = brayns.Vector3(-11333.343, -7163.718, -10376.665)
FULL_BRAIN_CAMERA_ROTATION = brayns.euler(153.6, 48, 0, degrees=True)
FULL_BRAIN_CELLS = "/home/acfleury/source/test/scripts/sfn/fullbrain_cells.json"
FULL_BRAIN_RESAMPLING = 0.9
FULL_BRAIN_SUBSAMPLING = 4
FULL_BRAIN_SKIP_FACTOR = 1

COMMON_CAMERA_POSITION = brayns.Vector3(-7370.48, -5062.222, -6766.448)
COMMON_CAMERA_ROTATION = brayns.euler(153.6, 48, 0, degrees=True)

STRIATUM = "STRd"
STRIATUM_COLOR = "#98D6F9"
STRIATUM_OUTPUT = "/home/acfleury/source/test/scripts/sfn/frames_striatum/%d.png"
STRIATUM_CAMERA_POSITION = COMMON_CAMERA_POSITION
STRIATUM_CAMERA_ROTATION = COMMON_CAMERA_ROTATION
STRIATUM_CELLS = "/home/acfleury/source/test/scripts/sfn/striatum_cells.json"
STRIATUM_RESAMPLING = 0.99
STRIATUM_SUBSAMPLING = 2
STRIATUM_SKIP_FACTOR = 1

MOB = "MOB"
MOB_COLOR = "#9AD2BD"
MOB_OUTPUT = "/home/acfleury/source/test/scripts/sfn/frames_mob/%d.png"
MOB_CAMERA_POSITION = COMMON_CAMERA_POSITION
MOB_CAMERA_ROTATION = COMMON_CAMERA_ROTATION
MOB_CELLS = "/home/acfleury/source/test/scripts/sfn/mob_cells.json"
MOB_RESAMPLING = 0.99
MOB_SUBSAMPLING = 2
MOB_SKIP_FACTOR = 1

CEREBELLUM = "CB"
CEREBELLUM_COLOR = "#F0F080"
CEREBELLUM_OUTPUT = "/home/acfleury/source/test/scripts/sfn/frames_cerebellum/%d.png"
CEREBELLUM_CAMERA_POSITION = COMMON_CAMERA_POSITION
CEREBELLUM_CAMERA_ROTATION = COMMON_CAMERA_ROTATION
CEREBELLUM_CELLS = "/home/acfleury/source/test/scripts/sfn/cerebellum_cells.json"
CEREBELLUM_RESAMPLING = 0.9
CEREBELLUM_SUBSAMPLING = 4
CEREBELLUM_SKIP_FACTOR = 1

FOVY = brayns.Fovy(29, degrees=True)

RESOLUTION = brayns.Resolution.ultra_hd

BACKGROUND = brayns.Color4.black.transparent

RENDERER = brayns.ProductionRenderer(background_color=BACKGROUND)
# RENDERER = brayns.InteractiveRenderer(background_color=BACKGROUND)

MATERIAL = (
    brayns.PrincipledMaterial(
        roughness=0.35,
        specular=0.5,
    )
    if isinstance(RENDERER, brayns.ProductionRenderer)
    else brayns.PhongMaterial()
)

TEST_VISIBLE_CELLS = False
# TEST_VISIBLE_CELLS = True
TEST_RADIUS_MULTIPLIER = 3
FPS = 25
# FPS = 1

SPAWN_DURATION = 5
GROWTH_DURATION = 10

SPAWN_FRAME_COUNT = SPAWN_DURATION * FPS
GROWTH_FRAME_COUNT = GROWTH_DURATION * FPS
FRAMES = list(range(SPAWN_FRAME_COUNT + GROWTH_FRAME_COUNT + 1))

with open("/gpfs/bbp.cscs.ch/home/acfleury/src/Test/test.txt") as file:
    NODES = file.read().split()

# NODES = NODES[:5]

# HACK
# if TEST_VISIBLE_CELLS:
#     NODES = ["localhost"]
#     FRAMES = [0]
#     RENDERER = brayns.InteractiveRenderer(background_color=BACKGROUND)
# else:
#     NODES = ["r1i7n0"]
#     FRAMES = [
#         SPAWN_FRAME_COUNT - 1,
#         SPAWN_FRAME_COUNT,
#         SPAWN_FRAME_COUNT + GROWTH_FRAME_COUNT,
#     ]


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
class View:
    camera_position: brayns.Vector3
    camera_rotation: brayns.Rotation


@dataclass
class Render:
    node_set: str
    visible_cells: set[int] | None
    resampling: float
    subsampling: int
    skip_factor: int
    color: brayns.Color4 | dict[str, brayns.Color4]
    output: str
    view: View
    use_spheres: bool = False
    radius_multiplier: float = 10


def cleanup(output: str) -> None:
    frames = Path(output).parent

    if not frames.exists():
        frames.mkdir(parents=True, exist_ok=True)
        return

    for frame in frames.glob("*.png"):
        frame.unlink()


def parse_visible_cells(path: Path) -> set[int] | None:
    if TEST_VISIBLE_CELLS:
        return None

    with path.open() as file:
        return set(json.load(file))


def parse_full_brain_colors() -> dict[str, brayns.Color4]:
    with open(FULL_BRAIN_COLORS) as file:
        data = json.load(file)
        assert isinstance(data, dict)

    return {region: brayns.Color4(*color) for region, color in data.items()}


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


def load_circuit(
    instance: brayns.Instance,
    cells: list[int],
    render: Render,
    density: float,
    growth: float,
    morphologies: bool,
) -> brayns.Future[list[brayns.Model]] | None:
    if density == 1.0 and render.visible_cells is not None:
        cells = [cell for cell in cells if cell in render.visible_cells]

    if growth == 0.0 and render.use_spheres:
        morphologies = False

    cells = cells[:: render.skip_factor]
    cell_count = int(len(cells) * density)
    cells = cells[:cell_count]

    if not cells:
        return None

    radius_multiplier = 1 if morphologies else render.radius_multiplier

    if TEST_VISIBLE_CELLS:
        radius_multiplier = TEST_RADIUS_MULTIPLIER

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


def color_circuit(
    instance: brayns.Instance,
    model: brayns.Model,
    color: brayns.Color4 | dict[str, brayns.Color4],
) -> None:
    if isinstance(color, brayns.Color4):
        brayns.set_model_color(instance, model.id, color)
        brayns.set_material(instance, model.id, MATERIAL)
        return

    method = brayns.CircuitColorMethod.REGION
    brayns.color_model(instance, model.id, method, color)


def get_density(frame: int) -> float:
    if frame >= SPAWN_FRAME_COUNT:
        return 1.0

    return frame / SPAWN_FRAME_COUNT


def get_growth(frame: int) -> float:
    if frame <= SPAWN_FRAME_COUNT:
        return 0.0

    return (frame - SPAWN_FRAME_COUNT) / GROWTH_FRAME_COUNT


def focus_camera(view: View) -> brayns.Camera:
    position = view.camera_position
    direction = view.camera_rotation.apply(-brayns.Axis.z)
    up = view.camera_rotation.apply(brayns.Axis.y)

    camera_view = brayns.View(position, position + direction, up)

    projection = brayns.PerspectiveProjection(FOVY)

    return brayns.Camera(camera_view, projection)


def snapshot(
    instance: brayns.Instance, camera: brayns.Camera, frame: int, output: str
) -> None:
    snapshot = brayns.Snapshot(RESOLUTION, camera, RENDERER)
    snapshot.save(instance, output % frame)


def make_movie(output: str) -> None:
    path = Path(output).parent / "movie.mp4"
    movie = brayns.Movie(output, FPS)
    movie.save(str(path))


def export_frames(node: str, cells: list[int], render: Render, frames: Frames) -> None:
    uri = f"{node}:5000"
    connector = brayns.Connector(uri)

    camera = focus_camera(render.view)

    print(f"Connecting to {node}")
    with connector.connect() as instance:
        print(f"Connected to {node=}")

        brayns.clear_models(instance)

        add_lights(instance, render.view.camera_rotation)

        while True:
            frame = frames.get()

            if frame is None:
                print(f"All frames exported for {node=}")
                return

            density = get_density(frame)
            growth = get_growth(frame)

            if TEST_VISIBLE_CELLS:
                print("Loading sphere model")
                task = load_circuit(instance, cells, render, 1, 1, False)
            else:
                print(f"Loading model for {node=} with {density=} {growth=}")
                task = load_circuit(instance, cells, render, density, growth, True)

            model = None if task is None else task.wait_for_result()[0]
            print(f"Loaded model for {node=} with {density=} {growth=}")

            if model is not None:
                color_circuit(instance, model, render.color)

            print(f"Rendering {frame=} for {node=}")
            snapshot(instance, camera, frame, render.output)
            print(f"Rendered {frame=} for {node=}")

            if model is not None:
                brayns.remove_models(instance, [model.id])


def run(render: Render) -> None:
    cleanup(render.output)

    print("Parsing circuit")
    cells = parse_cells(render.node_set)
    print("Circuit parsed")

    if TEST_VISIBLE_CELLS:
        with open(OUTPUT_CELLS, "w") as file:
            json.dump(cells, file)

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

    print("Frame exported")

    make_movie(render.output)


def main() -> None:
    # thalamus = Render(
    #     node_set=THALAMUS,
    #     visible_cells=parse_visible_cells(Path(THALAMUS_CELLS)),
    #     resampling=THALAMUS_RESAMPLING,
    #     subsampling=THALAMUS_SUBSAMPLING,
    #     skip_factor=THALAMUS_SKIP_FACTOR,
    #     color=brayns.Color4.from_color3(brayns.parse_hex_color(THALAMUS_COLOR)),
    #     output=THALAMUS_OUTPUT,
    #     view=View(THALAMUS_CAMERA_POSITION, THALAMUS_CAMERA_ROTATION),
    # )
    # run(thalamus)

    # hippocampus = Render(
    #     node_set=HIPPOCAMPUS,
    #     visible_cells=parse_visible_cells(Path(HIPPOCAMPUS_CELLS)),
    #     resampling=HIPPOCAMPUS_RESAMPLING,
    #     subsampling=HIPPOCAMPUS_SUBSAMPLING,
    #     skip_factor=HIPPOCAMPUS_SKIP_FACTOR,
    #     color=brayns.Color4.from_color3(brayns.parse_hex_color(HIPPOCAMPUS_COLOR)),
    #     output=HIPPOCAMPUS_OUTPUT,
    #     view=View(HIPPOCAMPUS_CAMERA_POSITION, HIPPOCAMPUS_CAMERA_ROTATION),
    # )
    # run(hippocampus)

    # sscx = Render(
    #     node_set=SSCX,
    #     visible_cells=parse_visible_cells(Path(SSCX_CELLS)),
    #     resampling=SSCX_RESAMPLING,
    #     subsampling=SSCX_SUBSAMPLING,
    #     skip_factor=SSCX_SKIP_FACTOR,
    #     color=brayns.Color4.from_color3(brayns.parse_hex_color(SSCX_COLOR)),
    #     output=SSCX_OUTPUT,
    #     view=View(SSCX_CAMERA_POSITION, SSCX_CAMERA_ROTATION),
    # )
    # run(sscx)

    # neocortex = Render(
    #     node_set=NEOCORTEX,
    #     visible_cells=parse_visible_cells(Path(NEOCORTEX_CELLS)),
    #     resampling=NEOCORTEX_RESAMPLING,
    #     subsampling=NEOCORTEX_SUBSAMPLING,
    #     skip_factor=NEOCORTEX_SKIP_FACTOR,
    #     color=brayns.Color4.from_color3(brayns.parse_hex_color(NEOCORTEX_COLOR)),
    #     output=NEOCORTEX_OUTPUT,
    #     view=View(NEOCORTEX_CAMERA_POSITION, NEOCORTEX_CAMERA_ROTATION),
    # )
    # run(neocortex)

    # full_brain = Render(
    #     node_set=FULL_BRAIN,
    #     visible_cells=parse_visible_cells(Path(FULL_BRAIN_CELLS)),
    #     resampling=FULL_BRAIN_RESAMPLING,
    #     subsampling=FULL_BRAIN_SUBSAMPLING,
    #     skip_factor=FULL_BRAIN_SKIP_FACTOR,
    #     color=parse_full_brain_colors(),
    #     output=FULL_BRAIN_OUTPUT,
    #     view=View(FULL_BRAIN_CAMERA_POSITION, FULL_BRAIN_CAMERA_ROTATION),
    #     use_spheres=True,
    # )
    # run(full_brain)

    # striatum = Render(
    #     node_set=STRIATUM,
    #     visible_cells=parse_visible_cells(Path(STRIATUM_CELLS)),
    #     resampling=STRIATUM_RESAMPLING,
    #     subsampling=STRIATUM_SUBSAMPLING,
    #     skip_factor=STRIATUM_SKIP_FACTOR,
    #     color=brayns.Color4.from_color3(brayns.parse_hex_color(STRIATUM_COLOR)),
    #     output=STRIATUM_OUTPUT,
    #     view=View(STRIATUM_CAMERA_POSITION, STRIATUM_CAMERA_ROTATION),
    # )
    # run(striatum)

    # mob = Render(
    #     node_set=MOB,
    #     visible_cells=parse_visible_cells(Path(MOB_CELLS)),
    #     resampling=MOB_RESAMPLING,
    #     subsampling=MOB_SUBSAMPLING,
    #     skip_factor=MOB_SKIP_FACTOR,
    #     color=brayns.Color4.from_color3(brayns.parse_hex_color(MOB_COLOR)),
    #     output=MOB_OUTPUT,
    #     view=View(MOB_CAMERA_POSITION, MOB_CAMERA_ROTATION),
    # )
    # run(mob)

    cerebellum = Render(
        node_set=CEREBELLUM,
        visible_cells=parse_visible_cells(Path(CEREBELLUM_CELLS)),
        resampling=CEREBELLUM_RESAMPLING,
        subsampling=CEREBELLUM_SUBSAMPLING,
        skip_factor=CEREBELLUM_SKIP_FACTOR,
        color=brayns.Color4.from_color3(brayns.parse_hex_color(CEREBELLUM_COLOR)),
        output=CEREBELLUM_OUTPUT,
        view=View(CEREBELLUM_CAMERA_POSITION, CEREBELLUM_CAMERA_ROTATION),
    )
    run(cerebellum)


if __name__ == "__main__":
    main()
