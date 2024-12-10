import logging
import pathlib

import brayns

EXECUTABLE = (
    "/gpfs/bbp.cscs.ch/project/proj3/software/BraynsCircuitStudio/b27f4a9/braynsService"
)
CIRCUIT = "/gpfs/bbp.cscs.ch/project/proj83/circuits/Bio_M/20200805/CircuitConfig"
CIRCUIT = "/gpfs/bbp.cscs.ch/project/proj3/cloned_circuits/SSCX_2020_08_05/BlueConfig"
DENSITY = 0.01
RENDERER = brayns.InteractiveRenderer()
STEPS = 100
FRAMES = "/gpfs/bbp.cscs.ch/home/acfleury/src/Test/frames/%d.png"
RESOLUTION = brayns.Resolution.full_hd
VIEW = brayns.View(
    position=brayns.Vector3(
        -3894.676,
        -962.0773,
        -3261.0808,
    ),
    target=brayns.Vector3(
        4049.8518,
        -425.287,
        -3134.8289,
    ),
    up=brayns.Vector3(
        -9.9142916e-14,
        -0.22895055,
        0.9734381,
    ),
)

for path in pathlib.Path(FRAMES).parent.glob("*.png"):
    path.unlink()

service = brayns.Service(
    uri="localhost:5000",
    executable=EXECUTABLE,
)

connector = brayns.Connector(
    uri="localhost:5000",
    logger=brayns.Logger(logging.INFO),
    max_attempts=None,
)

with brayns.start(service, connector) as (process, instance):
    loader = brayns.BbpLoader(
        cells=brayns.BbpCells.from_density(DENSITY),
        morphology=brayns.Morphology(
            radius_multiplier=1,
            load_soma=True,
            load_dendrites=True,
        ),
    )
    models = loader.load_models(instance, CIRCUIT)
    model = models[0]

    target = model.bounds

    controller = brayns.CameraController(
        target=target,
        aspect_ratio=RESOLUTION.aspect_ratio,
    )
    camera = controller.camera

    brayns.color_circuit_by_id(instance, model.id, {})

    brayns.add_light(
        instance,
        brayns.AmbientLight(
            intensity=0.5,
        ),
    )
    brayns.add_light(
        instance,
        brayns.DirectionalLight(
            intensity=10,
            direction=brayns.Vector3(1, -1, -1).normalized,
        ),
    )

    for i in range(STEPS):
        angle = 360 * i / STEPS
        rotation = brayns.euler(0, angle, 0, degrees=True)
        rotation = VIEW.orientation.inverse.then(rotation)

        transform = brayns.Transform.rotate(rotation, target.center)
        model = brayns.update_model(instance, model.id, transform)

        snapshot = brayns.Snapshot(
            resolution=RESOLUTION,
            camera=camera,
            renderer=RENDERER,
        )
        snapshot.save(instance, FRAMES % i)
