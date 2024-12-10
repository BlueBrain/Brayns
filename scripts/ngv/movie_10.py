from pathlib import Path

import brayns

URI = "localhost:5000"

PATH = "/gpfs/bbp.cscs.ch/project/proj143/home/arnaudon/proj143/circuit-build/auxiliary/circuit_config_hpc.json"
IDS = list(range(10))
LOAD_AXON = False
LOAD_DENDRITES = True
LOAD_MULTIPLIER = 1

COLORS = [
    brayns.Color4.red,
    brayns.Color4.green,
    brayns.Color4.blue,
    brayns.Color4(1, 1, 0),
    brayns.Color4(1, 0, 1),
    brayns.Color4(0, 1, 1),
    brayns.Color4(1, 0.5, 0.5),
    brayns.Color4(0.5, 1, 0.5),
    brayns.Color4(0.5, 0.5, 1),
    brayns.Color4(0.5, 1, 1),
    brayns.Color4(1, 0.5, 1),
]

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

DISTANCE = 1
TRANSLATION = brayns.Vector3(0, 0, 0)
ROTATION = brayns.euler(0, 72, 0, degrees=True)

SNAPSHOT = "snapshot.png"
MOVIE = "movie.mp4"
PATTERN = "frames/%d.png"
FPS = 20
DURATION = 10
LAPS = 1


def cleanup() -> None:
    output = Path(PATTERN).parent
    output.mkdir(parents=True, exist_ok=True)
    for item in output.glob("*"):
        item.unlink()


def load_cells(instance: brayns.Instance) -> int:
    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name="cerebellum_neurons",
                nodes=brayns.SonataNodes.from_ids(IDS),
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


def color_cells(instance: brayns.Instance, model: int) -> None:
    method = brayns.CircuitColorMethod.ID
    ids = brayns.get_circuit_ids(instance, model)
    colors = {str(id): color for id, color in zip(ids, COLORS)}
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

    connector = brayns.Connector(URI)

    with connector.connect() as instance:
        brayns.clear_models(instance)

        model = load_cells(instance)

        color_cells(instance, model)

        for light in LIGHTS:
            brayns.add_light(instance, light)

        camera = focus_camera(instance)

        snapshot = brayns.Snapshot(RESOLUTION, camera, RENDERER)
        snapshot.save(instance, SNAPSHOT)

        make_movie(instance, camera)


if __name__ == "__main__":
    main()
