import logging
import pathlib
import subprocess

import brayns

EXECUTABLE = '/home/acfleury/Source/repos/Brayns/build/bin/braynsService'
OSPRAY = '/home/acfleury/Source/repos/OSPRAY_2_10/install_dir/lib'
BRAIN = '/home/acfleury/Test/mopro634/997.obj'
ULTRALISER = '/home/acfleury/Source/repos/Ultraliser/build/bin/ultraNeuroMorpho2Mesh'

MORPHOLOGIES_FOLDER = '/home/acfleury/Test/mopro634/morphologies'
MESHES_DIR = '/home/acfleury/Test/mopro634'
MESHES_FOLDER = MESHES_DIR + '/' + 'meshes'

MORPHOLOGIES = [
    str(path)
    for path in pathlib.Path(MORPHOLOGIES_FOLDER).glob('*.swc')
]

MESHES = [
    str(path)
    for path in pathlib.Path(MESHES_FOLDER).glob('*.obj')
]

RESOLUTION = 2 * brayns.Resolution.ultra_hd
BACKGROUND = brayns.Color4(1, 1, 1, 1)
INTENSITY = 1.0
REFRACTION = 1.5

OUTPUT = '/home/acfleury/Test/mopro634/images/{name}.png'

VIEWS = {
    # 'front': brayns.CameraRotation.front,
    # 'back': brayns.CameraRotation.back,
    'right': brayns.CameraRotation.right,
    'left': brayns.CameraRotation.left,
    'top': brayns.CameraRotation.top,
    'bottom': brayns.CameraRotation.bottom,
}


def build_meshes() -> None:
    for morphology in MORPHOLOGIES:
        subprocess.run([
            ULTRALISER,
            '--morphology',
            morphology,
            '--output-directory',
            MESHES_DIR,
            '--export-obj-mesh',
            '--ignore-marching-cubes-mesh',
            '--ignore-laplacian-mesh',
        ])


# build_meshes()

service = brayns.Service(
    'localhost:5000',
    executable=EXECUTABLE,
    env={'LD_LIBRARY_PATH': OSPRAY},
)

connector = brayns.Connector(
    'localhost:5000',
    logger=brayns.Logger(logging.INFO),
    max_attempts=None,
)

with brayns.start(service, connector) as (process, instance):

    loader = brayns.MeshLoader()

    brain = loader.load_models(instance, BRAIN)[0]

    glass = brayns.GlassMaterial(REFRACTION)
    brayns.set_material(instance, brain.id, glass)

    meshes = [
        model
        for path in MESHES
        for model in loader.load_models(instance, path)
    ]

    matte = brayns.MatteMaterial()
    for mesh in meshes:
        instance.request(f'set-material-{matte.name}', {
            'model_id': mesh.id,
            'material': {
                'color': [0, 0, 1],
                **matte.get_properties(),
            }
        })

    target = brayns.get_bounds(instance)

    for name, rotation in VIEWS.items():

        save_as = OUTPUT.format(name=name)

        camera = brayns.look_at(
            target,
            aspect_ratio=RESOLUTION.aspect_ratio,
            rotation=rotation,
        )

        brayns.clear_lights(instance)

        ambient = brayns.AmbientLight(
            intensity=INTENSITY,
        )
        brayns.add_light(instance, ambient)

        snapshot = brayns.Snapshot(
            resolution=RESOLUTION,
            camera=camera,
            renderer=brayns.ProductionRenderer(
                background_color=BACKGROUND,
            ),
        )
        snapshot.save(instance, save_as)
