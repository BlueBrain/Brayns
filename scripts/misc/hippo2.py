import logging

import brayns

URI = 'r1i6n34.bbp.epfl.ch:5000'
PATH = '/gpfs/bbp.cscs.ch/project/proj112/circuits/CA1/20211110-BioM/CircuitConfig'
OUTPUT = 'snapshot.png'
SIDE_POSITION = [1132, 354, 444]
SIDE_ROTATION = [78, -60, 103]
ROTATED_POSITION = [994, 392, 828]
ROTATED_ROTATION = [213, -128, -31]
POSITION = ROTATED_POSITION
ROTATION = ROTATED_ROTATION
LOAD = False

connector = brayns.Connector(
    uri=URI,
    logger=brayns.Logger(logging.INFO),
)

with connector.connect() as instance:

    if LOAD:
        brayns.clear_models(instance)
        loader = brayns.BbpLoader(
            cells=brayns.BbpCells.from_density(0.1),
            morphology=brayns.Morphology(
                load_dendrites=True,
            )
        )
        models = loader.load(instance, PATH)
    else:
        models = brayns.get_models(instance)

    model = models[0]

    brayns.color_circuit_by_id(instance, model.id, {})

    camera = brayns.look_at(model.bounds)
    camera.position -= 0.2 * camera.distance * brayns.Vector3.forward
    euler = brayns.Vector3(*ROTATION)
    rotation = brayns.Rotation.from_euler(euler, degrees=True)
    camera.rotate_around_target(rotation)

    brayns.clear_lights(instance)

    light = brayns.DirectionalLight(
        intensity=4,
    )

    euler = brayns.Vector3(0, 0, 45)
    rotation = brayns.Rotation.from_euler(euler, degrees=True)
    light.direction = rotation.apply(camera.direction)
    brayns.add_light(instance, light)

    euler = brayns.Vector3(0, 0, -45)
    rotation = brayns.Rotation.from_euler(euler, degrees=True)
    light.direction = rotation.apply(camera.direction)
    brayns.add_light(instance, light)

    snapshot = brayns.Snapshot(
        resolution=2 * brayns.Resolution.ultra_hd,
        camera=camera,
        renderer=brayns.InteractiveRenderer(
            samples_per_pixel=5,
        ),
    )
    snapshot.save(instance, 'snapshot.png')
