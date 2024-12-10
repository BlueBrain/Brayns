import logging
from pathlib import Path

import brayns

EXECUTABLE = "/home/acfleury/Source/repos/Brayns/build/bin/braynsService"
OSPRAY = "/home/acfleury/Source/repos/OSPRAY_2_10/install_dir/lib"
PATH = "/gpfs/bbp.cscs.ch/project/proj3/tolokoban/brain.obj"
RESOLUTION = brayns.Resolution.full_hd
STEPS = 300
PATTERN = "/home/acfleury/Test/frames/%d.png"
MOVIE = "/home/acfleury/Test/frames/movie.mp4"

service = brayns.Service(
    "localhost:5000",
    executable=EXECUTABLE,
    env={"LD_LIBRARY_PATH": OSPRAY},
)

connector = brayns.Connector(
    "localhost:5000",
    logger=brayns.Logger(logging.INFO),
    max_attempts=None,
)

for child in Path(PATTERN).parent.glob("*"):
    child.unlink()

with brayns.start(service, connector) as (process, instance):

    loader = brayns.MeshLoader()
    models = loader.load_models(instance, PATH)
    model = models[0]

    rotation = brayns.euler(180, 0, 0, degrees=True)
    transform = brayns.Transform(rotation=rotation)
    model = brayns.update_model(instance, model.id, transform)

    color = brayns.Color4(1, 1, 1, 0.1)
    brayns.set_model_color(instance, model.id, color)

    controller = brayns.CameraController(
        target=model.bounds,
        aspect_ratio=RESOLUTION.aspect_ratio,
        rotation=rotation,
    )
    camera = controller.camera
    distance = camera.distance

    for i in range(STEPS):

        angle = i * 360 / STEPS
        controller.rotation = brayns.euler(0, angle, 0, degrees=True)
        camera = controller.camera
        camera.distance = distance

        brayns.clear_lights(instance)

        ambient = brayns.AmbientLight(intensity=0.5)
        brayns.add_light(instance, ambient)

        directional = brayns.DirectionalLight(intensity=10, direction=camera.direction)
        brayns.add_light(instance, directional)

        snapshot = brayns.Snapshot(
            resolution=RESOLUTION,
            camera=camera,
            renderer=brayns.InteractiveRenderer(
                background_color=brayns.Color4.black.transparent
            ),
        )
        snapshot.save(instance, PATTERN % i)

movie = brayns.Movie(PATTERN, fps=30)
movie.save(MOVIE)
