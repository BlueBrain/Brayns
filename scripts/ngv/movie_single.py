from pathlib import Path

import brayns

URI = "r1i7n24.bbp.epfl.ch:5000"

PATH = "/gpfs/bbp.cscs.ch/project/proj3/cloned_circuits/morphologies/GolgiCell.swc"
LOAD_AXON = True
LOAD_DENDRITES = True
LOAD_MULTIPLIER = 1

SOMA = brayns.Color4.red
DENDRITES = brayns.Color4.blue
APICAL_DENDRITES = brayns.Color4(1, 1, 0)
AXON = brayns.Color4.green

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

DISTANCE = 1.5
TRANSLATION = brayns.Vector3(0, 0, 0)

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


def load_cell(instance: brayns.Instance) -> int:
    morphology = brayns.Morphology(
        radius_multiplier=LOAD_MULTIPLIER,
        load_axon=LOAD_AXON,
        load_dendrites=LOAD_DENDRITES,
    )

    loader = brayns.MorphologyLoader(morphology)
    model = loader.load_models(instance, PATH)[0]

    return model.id


def color_cell(instance: brayns.Instance, model: int) -> None:
    method = brayns.CircuitColorMethod.MORPHOLOGY_SECTION
    colors = {
        "soma": SOMA,
        "dendrite": DENDRITES,
        "apical_dendrite": APICAL_DENDRITES,
        "axon": AXON,
    }
    brayns.color_model(instance, model, method, colors)


def find_soma_position(instance: brayns.Instance) -> brayns.Vector3:
    morphology = brayns.Morphology(LOAD_MULTIPLIER)

    loader = brayns.MorphologyLoader(morphology)
    model = loader.load_models(instance, PATH)[0]

    center = model.bounds.center

    brayns.remove_models(instance, [model.id])

    return center


def focus_camera(instance: brayns.Instance, center: brayns.Vector3) -> brayns.Camera:
    target = brayns.get_bounds(instance)

    target = target.translate(center - target.center)

    controller = brayns.CameraController(
        target=target,
        aspect_ratio=RESOLUTION.aspect_ratio,
    )
    camera = controller.camera

    camera = camera.translate(TRANSLATION * target.size)

    camera.distance *= DISTANCE

    return camera


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

        center = find_soma_position(instance)

        model = load_cell(instance)

        color_cell(instance, model)

        for light in LIGHTS:
            brayns.add_light(instance, light)

        camera = focus_camera(instance, center)

        snapshot = brayns.Snapshot(RESOLUTION, camera, RENDERER)
        snapshot.save(instance, SNAPSHOT)

        make_movie(instance, camera)


if __name__ == "__main__":
    main()
