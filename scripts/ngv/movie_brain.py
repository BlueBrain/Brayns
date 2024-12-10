import json
from pathlib import Path
from typing import Any

import brayns

URI = "r1i7n24:5000"

PATH = "/gpfs/bbp.cscs.ch/project/proj3/cloned_circuits/FULL_BRAIN_WITH_SIM_15_06_2023/circuit_config.json"
PATH = "/gpfs/bbp.cscs.ch/project/proj3/cloned_circuits/FullBrainSoma/config.json"
MESH = "/gpfs/bbp.cscs.ch/project/proj3/tolokoban/brain.obj"
REGIONS = "/home/acfleury/source/test/scripts/movies/regions.json"
DENSITY = 1.0
LOAD_AXON = False
LOAD_DENDRITES = False
LOAD_MULTIPLIER = 20

RESOLUTION = brayns.Resolution.full_hd
RENDERER = brayns.InteractiveRenderer()
LIGHTS = [
    brayns.DirectionalLight(direction=brayns.Vector3(1, 1, 1)),
    brayns.DirectionalLight(direction=brayns.Vector3(-1, 1, 1)),
    brayns.DirectionalLight(direction=brayns.Vector3(1, -1, 1)),
    brayns.DirectionalLight(direction=brayns.Vector3(1, 1, -1)),
    brayns.DirectionalLight(direction=brayns.Vector3(-1, -1, 1)),
    brayns.DirectionalLight(direction=brayns.Vector3(-1, 1, -1)),
    brayns.DirectionalLight(direction=brayns.Vector3(1, -1, -1)),
    brayns.DirectionalLight(direction=brayns.Vector3(-1, -1, -1)),
]

DISTANCE = 1.2
TRANSLATION = brayns.Vector3(0, 0, 0)
ROTATION = brayns.euler(0, 0, 0, degrees=True)

SNAPSHOT = "snapshot.png"
MOVIE = "movie.mp4"
PATTERN = "frames/%d.png"
FPS = 20
DURATION = 15
LAPS = 1


def cleanup() -> None:
    output = Path(PATTERN).parent
    output.mkdir(parents=True, exist_ok=True)
    for item in output.glob("*"):
        item.unlink()


def parse_color(value: str) -> brayns.Color4:
    if len(value) == 3:
        value = "".join(2 * item for item in value)
    if len(value) != 6:
        raise ValueError(f"Invalid color {value}")
    color = brayns.parse_hex_color(value)
    return brayns.Color4.from_color3(color)


def parse_regions(data: list[Any]) -> dict[str, brayns.Color4]:
    result = dict[str, brayns.Color4]()
    tasks = data
    while tasks:
        new_tasks = []
        for task in tasks:
            name = task[1]
            color = task[3]
            children = task[4] if len(task) >= 5 else []
            new_tasks.extend(children)
            result[name] = parse_color(color)
        tasks = new_tasks
    return result


def load_cells(instance: brayns.Instance) -> int:
    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name="root__neurons",
                nodes=brayns.SonataNodes.from_density(DENSITY),
                morphology=brayns.Morphology(
                    radius_multiplier=LOAD_MULTIPLIER,
                    load_axon=LOAD_AXON,
                    load_dendrites=LOAD_DENDRITES,
                ),
            )
        ]
    )
    model = loader.load_models(instance, PATH)[0]

    return model.id


def load_mesh(instance: brayns.Instance) -> int:
    loader = brayns.MeshLoader()
    model = loader.load_models(instance, MESH)[0]
    material = brayns.GhostMaterial()
    brayns.set_material(instance, model.id, material)
    color = brayns.Color4.white
    brayns.set_model_color(instance, model.id, color)
    return model.id


def color_cells(
    instance: brayns.Instance, model: int, colors: dict[str, brayns.Color4]
) -> None:
    method = brayns.CircuitColorMethod.REGION
    values = brayns.get_color_values(instance, model, method)
    allowed = set(values)
    colors = {key: value for key, value in colors.items() if key in allowed}
    brayns.color_model(instance, model, method, colors)


def focus_camera(instance: brayns.Instance) -> brayns.Camera:
    target = brayns.get_bounds(instance)

    controller = brayns.CameraController(
        target=target,
        aspect_ratio=RESOLUTION.aspect_ratio,
    )
    camera = controller.camera

    camera = camera.translate(TRANSLATION * target.size)

    camera.distance *= DISTANCE

    camera.up = brayns.Axis.down

    return camera.rotate_around_target(ROTATION)


def make_movie(instance: brayns.Instance, camera: brayns.Camera) -> None:
    frame_count = FPS * DURATION

    angle_step = LAPS * 360 / frame_count

    for i in range(frame_count):
        snapshot = brayns.Snapshot(RESOLUTION, camera, RENDERER)
        snapshot.save(instance, PATTERN % i)

        rotation = brayns.euler(0, angle_step, 0, degrees=True)
        camera = camera.rotate_around_target(rotation)

    movie = brayns.Movie(PATTERN, FPS)
    movie.save(MOVIE)


def main() -> None:
    cleanup()

    with open(REGIONS) as file:
        data = json.load(file)
        colors = parse_regions(data)

    connector = brayns.Connector(URI)

    with connector.connect() as instance:
        brayns.clear_models(instance)

        # load_mesh(instance)
        model = load_cells(instance)

        color_cells(instance, model, colors)

        for light in LIGHTS:
            brayns.add_light(instance, light)

        camera = focus_camera(instance)

        snapshot = brayns.Snapshot(RESOLUTION, camera, RENDERER)
        snapshot.save(instance, SNAPSHOT)

        make_movie(instance, camera)


if __name__ == "__main__":
    main()
