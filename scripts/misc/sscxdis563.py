import logging

import bluepy
import brayns
from scipy.spatial.transform import Rotation

PATH = '/gpfs/bbp.cscs.ch/project/proj96/scratch/home/ecker/simulations/LayerWiseEShotNoise_PyramidPatterns/BlueConfig'
RESOLUTION = brayns.Resolution.full_hd
OUTPUT = '/gpfs/bbp.cscs.ch/home/acfleury/src/Test/frames'
MOVIE = '/gpfs/bbp.cscs.ch/home/acfleury/src/Test/movie.mp4'

connector = brayns.Connector(
    uri='r2i1n34:5000',
    logger=brayns.Logger(logging.INFO),
)

with connector.connect() as instance:

    brayns.clear_models(instance)

    loader = brayns.BbpLoader(
        report=brayns.BbpReport.compartment('soma'),
        morphology=brayns.Morphology(
            radius_multiplier=10,
        ),
    )
    models = loader.load_models(instance, PATH)
    model = models[0]

    ramp = brayns.get_color_ramp(instance, model.id)

    ramp.value_range = brayns.ValueRange(-80, 10)

    brayns.set_color_ramp(instance, model.id, ramp)

    circuit = bluepy.Circuit(PATH)
    cells = circuit.cells
    gid = cells.ids(None)[0]
    cell = cells.get(gid)
    orientation = cell['orientation']
    rotation = Rotation.from_matrix(orientation)
    inverse = rotation.inv()
    quaternion = brayns.Quaternion(*inverse.as_quat())

    '''quaternion = brayns.Quaternion(
        x=0.6560921729403664,
        y=0.09987636406732446,
        z=-0.3768641867826438,
        w=-0.6461742468001461,
    )'''

    model.transform.rotation = brayns.Rotation.from_quaternion(quaternion)
    model = brayns.update_model(instance, model.id, transform=model.transform)

    brayns.enable_simulation(instance, model.id, False)

    method = brayns.ColorMethod.LAYER
    color = brayns.color_circuit_by_method(instance, model.id, method, {
        '1': brayns.Color4(255, 242, 59, 255) / 255,
        '2': brayns.Color4(248, 148, 48, 255) / 255,
        '3': brayns.Color4(225, 45, 97, 255) / 255,
        '4': brayns.Color4(253, 156, 250, 255) / 255,
        '5': brayns.Color4(103, 168, 222, 255) / 255,
        '6': brayns.Color4(106, 230, 109, 255) / 255,
    })

    camera = brayns.look_at(
        target=model.bounds,
        aspect_ratio=RESOLUTION.aspect_ratio,
        rotation=brayns.CameraRotation.left,
    )

    brayns.clear_lights(instance)

    light = brayns.DirectionalLight(
        intensity=4,
        direction=camera.direction,
    )

    brayns.add_light(instance, light)

    renderer = brayns.InteractiveRenderer(
        samples_per_pixel=32
    )

    snapshot = brayns.Snapshot(
        resolution=brayns.Resolution.full_hd,
        camera=camera,
        renderer=renderer,
    )
    snapshot.save(instance, 'snapshot.png')

    brayns.enable_simulation(instance, model.id, True)

    simulation = brayns.get_simulation(instance)

    frames = brayns.MovieFrames(
        fps=25,
        slowing_factor=1,
        start_frame=0,
        end_frame=-1,
    )

    indices = frames.get_indices(simulation)

    key_frames = brayns.KeyFrame.from_indices(indices, camera.view)

    exporter = brayns.FrameExporter(
        frames=key_frames,
        resolution=brayns.Resolution.full_hd,
        projection=camera.projection,
        renderer=renderer,
    )

    exporter.export_frames(instance, OUTPUT)

    movie = brayns.Movie(
        frames_folder=OUTPUT,
        frames_format=brayns.ImageFormat.PNG,
        fps=frames.fps,
        resolution=exporter.resolution,
    )

    movie.save(MOVIE)
