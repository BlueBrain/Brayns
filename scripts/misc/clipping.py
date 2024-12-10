from pathlib import Path

import brayns

URI = "localhost:5000"

RESOLUTION = brayns.Resolution.full_hd
LIGHTS = [
    brayns.AmbientLight(0.1),
    brayns.DirectionalLight(intensity=5, direction=brayns.Vector3(-1, 0, -1)),
    brayns.DirectionalLight(intensity=2, direction=brayns.Vector3(1, 0, -1)),
]
RENDERER = brayns.ProductionRenderer()
RENDERER = brayns.InteractiveRenderer(enable_shadows=True)

FINAL_MATERIAL = brayns.PrincipledMaterial(
    roughness=0.35,
    specular=0.5,
)

PATTERN = "frames/%d.png"
DURATION = 10
FPS = 10

if isinstance(RENDERER, brayns.ProductionRenderer):
    MATERIAL = FINAL_MATERIAL
else:
    MATERIAL = brayns.PhongMaterial()


def cleanup() -> None:
    output = Path(PATTERN).parent
    output.mkdir(parents=True, exist_ok=True)
    for item in output.glob("*"):
        item.unlink()


def focus_camera(instance: brayns.Instance) -> brayns.Camera:
    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=RESOLUTION.aspect_ratio,
        rotation=brayns.euler(-20, 0, 0, degrees=True),
    )
    return controller.camera


def add_lights(instance: brayns.Instance) -> None:
    for light in LIGHTS:
        brayns.add_light(instance, light)


def make_movie(instance: brayns.Instance, camera: brayns.Camera) -> None:
    frame_count = DURATION * FPS
    angle_step = 360 / frame_count

    for i in range(frame_count):
        snapshot = brayns.Snapshot(RESOLUTION, camera, RENDERER)
        snapshot.save(instance, PATTERN % i)

        rotation = brayns.euler(0, angle_step, 0, degrees=True)
        camera = camera.rotate_around_target(rotation)

    movie = brayns.Movie(PATTERN, FPS)
    movie.save("movie.mp4")


def main() -> None:
    cleanup()

    connector = brayns.Connector(URI)

    with connector.connect() as instance:
        brayns.clear_models(instance)

        box = brayns.Box(
            min=brayns.Vector3(-1, -1, -1),
            max=brayns.Vector3(1, 1, 1),
        )
        brayns.add_clipping_geometries(instance, [box], invert_normals=True)

        center = brayns.Vector3(0, 0, 1)
        sphere = brayns.Sphere(0.5, center)
        color = brayns.Color4(0.9, 0.1, 0.1)
        brayns.add_geometries(instance, [(sphere, color)])

        camera = focus_camera(instance)

        add_lights(instance)

        snapshot = brayns.Snapshot(RESOLUTION, camera, RENDERER)
        snapshot.save(instance, "test.png")

        make_movie(instance, camera)


if __name__ == "__main__":
    main()
