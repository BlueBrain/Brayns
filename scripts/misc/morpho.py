import time
import base64
import brayns
import math
import numpy
from scipy.spatial.transform import Rotation

CIRCUIT = '/gpfs/bbp.cscs.ch/project/proj96/scratch/home/ecker/simulations/LayerWiseEShotNoise_PyramidPatterns/BlueConfig'
FRAMES = '/gpfs/bbp.cscs.ch/home/acfleury/src/Test/frames'
MOVIE = '/gpfs/bbp.cscs.ch/home/acfleury/src/Test/movie'
LOAD = True
CAMERA = True
RENDERER = True
SNAPSHOT = False
EXPORT = True
FPS = 1
DURATION = 60
MAKE = False

client = brayns.Client('r1i5n20.bbp.epfl.ch:5000')
explorer = brayns.CircuitExplorer(client)

if LOAD:
    print('Loading model')

    client.remove_model(list(range(1000)))

    model_id = client.add_model(
        loader_name='BBP loader',
        loader_properties={
            'percentage': 0.2,
            'report_type': 'spikes',
            'neuron_morphology_parameters': {
                'radius_multiplier': 5,
                'load_soma': True,
                'load_dendrites': False,
                'load_axon': False
            },
            'spike_transition_time': 3
        },
        name='Test',
        path=CIRCUIT
    )[0]['id']

    '''client.set_material_range(
        model_id=model_id,
        material_ids=[],
        properties={
            'opacity': 0.5
        }
    )'''

    print('Model loaded')

if CAMERA:
    scene = client.get_scene()
    bounds = scene['bounds']
    bottom_left = numpy.array(bounds['min'], numpy.float64)
    top_right = numpy.array(bounds['max'], numpy.float64)
    size = top_right - bottom_left
    width = size[0]
    height = size[1]
    depth = size[2]
    center = (bottom_left + top_right) / 2

    print(f'Scene center: {center}')
    print(f'Scene size: {size}')

    client.set_camera(current='perspective')
    camera_params = client.get_camera_params()
    fovy = math.radians(camera_params['fovy'])

    position = center
    position[1] += depth / 2 / math.tan(fovy / 2)
    position[1] += height * 0.5
    target = center
    orientation = Rotation.from_euler(
        'xyz',
        [-90, 0, 0],
        degrees=True
    )

    correction = Rotation.from_euler(
        'xyz',
        [0, 30, 0],
        degrees=True
    )

    position = correction.apply(position - center) + center
    orientation = correction * orientation

    '''atlas_position = [3450.37077451, -371.51590913, -2107.76507547]
    atlas_orientation = Rotation.from_quat([85, 0, -41, 85])'''

    '''atlas_orientation = Rotation.from_quat([
        0.11582841816539256,
        0.5075351818531512,
        -0.624087637819538,
        -0.5826718090378246
    ])'''

    '''position = atlas_orientation.apply(position) + atlas_position
    orientation = atlas_orientation * orientation'''

    '''orientation = Rotation.from_quat([
        0.11582841816539256,
        0.5075351818531512,
        -0.624087637819538,
        -0.5826718090378246
    ])'''

    '''position = numpy.array([
        -254.97258404549456,
        -2910.167474054098,
        -473.0883644949593
    ], numpy.float64)'''

    '''position = numpy.array([
        700,
        -4000,
        -200
    ], numpy.float64)

    shift = orientation.apply(numpy.array(
        [200, 200, -1000],
        numpy.float64
    ))

    position += shift'''

    target = [
        6019.224581241716,
        1339.3619055750096,
        -4377.129124233203
    ]

    client.set_camera(
        orientation=list(orientation.as_quat()),
        position=list(position),
        target=list(target),
    )

    print(f'Camera position: {position}')


if RENDERER:
    client.set_renderer(
        head_light=False,
        background_color=[1, 1, 1]
    )
    client.set_renderer_params({
        'epsilonFactor': 1,
        'exposure': 1,
        'fogStart': 0,
        'fogThickness': 100000000,
        'giDistance': 10000,
        'giSamples': 0,
        'giWeight': 0,
        'maxBounces': 5,
        'maxDistanceToSecondaryModel': 30,
        'samplingThreshold': 0.001,
        'shadows': 1,
        'softShadows': 0.4,
        'softShadowsSamples': 1,
        'volumeAlphaCorrection': 0.5,
        'volumeSpecularExponent': 20
    })
    client.clear_lights()
    client.add_light_directional(
        color=[1, 1, 1],
        intensity=2,
        is_visible=True,
        direction=[-1, -1, 0],
        angular_diameter=1
    )
    print('Lights adjusted')

if SNAPSHOT:
    result = client.snapshot(
        format='png',
        size=[1920, 1080],
        samples_per_pixel=128
    )
    with open('snapshot.png', 'wb') as file:
        file.write(base64.b64decode(result['data']))
        print(f'Snapshot: {file}')

if EXPORT:
    animation_params = client.get_animation_parameters()
    start_frame = animation_params['start_frame']
    end_frame = animation_params['end_frame']
    frame_count = animation_params['frame_count']
    dt = animation_params['dt'] / 1000

    print(f'Animation params: {animation_params}')

    frame_count = min(frame_count, DURATION / dt)
    step = frame_count / DURATION / FPS

    def generate_frame_indices():
        i = start_frame
        while i < frame_count:
            yield int(i)
            i += step
        print(i)

    print(f'Key frames: {frame_count, step}')

    camera = client.get_camera()
    camera_params = client.get_camera_params()

    client.export_frames_to_disk(
        path=FRAMES,
        format='png',
        quality=100,
        spp=128,
        name_image_after_simulation_index=False,
        key_frames=[
            {
                'frame_index': i,
                'camera': camera,
                'camera_params': camera_params
            }
            for i in generate_frame_indices()
        ]
    )

    progress = 0
    print('Waiting for frames')
    while (progress < 1):
        progress = client.get_export_frames_progress()['progress']
        print(f'\rProgress: {100 * progress}%', end='')
    print('Frames exported')

if MAKE:
    print('Make movie')
    explorer.make_movie(
        output_movie_path=MOVIE,
        fps_rate=FPS,
        frames_folder_path=FRAMES,
        frame_file_extension='png',
        dimensions=[1920, 1080],
        erase_frames=True
    )
    print('Movie generated')
