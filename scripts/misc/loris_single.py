import logging
from pathlib import Path

import brayns

MESH = "/gpfs/bbp.cscs.ch/project/proj3/tolokoban/brain.obj"
CIRCUIT = "/gpfs/bbp.cscs.ch/data/scratch/proj134/home/king/BBPP134-15/2022.10.28/circuit_config.json"
PATTERN = "/gpfs/bbp.cscs.ch/home/acfleury/src/Test/frames/%d.png"
MOVIE = "/gpfs/bbp.cscs.ch/home/acfleury/src/Test/frames/movie.mp4"

COLOR = brayns.Color4(1, 1, 1, 0.1)
RESOLUTION = brayns.Resolution.ultra_hd

RENDERER = brayns.ProductionRenderer(
    samples_per_pixel=128,
    max_ray_bounces=5,
    background_color=brayns.Color4.black.transparent,
)

STEPS = 300
FPS = 30

COLORS = [
    [1.0, 0.9490196078431372, 0.0],
    [0.9686274509803922, 0.5803921568627451, 0.11372549019607843],
    [0.8784313725490196, 0.1843137254901961, 0.3803921568627451],
    [0.9882352941176471, 0.6078431372549019, 0.9921568627450981],
    [0.40784313725490196, 0.6588235294117647, 0.8784313725490196],
    [0.4235294117647059, 0.9019607843137255, 0.3843137254901961],
    [0.6509803921568628, 0.33725490196078434, 0.1568627450980392],
    [0.9686274509803922, 0.5058823529411764, 0.7490196078431373],
    [0.6, 0.6, 0.6],
    [0.8941176470588236, 0.10196078431372549, 0.10980392156862745],
    [0.21568627450980393, 0.49411764705882355, 0.7215686274509804],
    [0.30196078431372547, 0.6862745098039216, 0.2901960784313726],
    [0.596078431372549, 0.3058823529411765, 0.6392156862745098],
    [1.0, 0.4980392156862745, 0.0],
    [1.0, 1.0, 0.2],
    [0.6509803921568628, 0.33725490196078434, 0.1568627450980392],
    [0.9686274509803922, 0.5058823529411764, 0.7490196078431373],
    [0.6, 0.6, 0.6],
    [0.8941176470588236, 0.10196078431372549, 0.10980392156862745],
    [0.21568627450980393, 0.49411764705882355, 0.7215686274509804],
]

service = brayns.Service("localhost:5000")

connector = brayns.Connector(
    "localhost:5000",
    logger=brayns.Logger(logging.INFO),
    max_attempts=None,
)

for child in Path(PATTERN).parent.glob("*"):
    child.unlink()

with brayns.start(service, connector) as (process, instance):

    loader = brayns.MeshLoader()
    mesh = loader.load_models(instance, MESH)[0]

    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                "root_neurons",
                nodes=brayns.SonataNodes.from_density(0.1),
                morphology=brayns.Morphology(radius_multiplier=10),
            )
        ]
    )
    circuit = loader.load_models(instance, CIRCUIT)[0]

    rotation = brayns.euler(180, 0, 0, degrees=True)
    transform = brayns.Transform(rotation=rotation)
    mesh = brayns.update_model(instance, mesh.id, transform)
    circuit = brayns.update_model(instance, circuit.id, transform)

    target = mesh.bounds
    size = target.size

    if isinstance(RENDERER, brayns.ProductionRenderer):
        material = brayns.GlassMaterial()
        brayns.set_material(instance, mesh.id, material)
    brayns.set_model_color(instance, mesh.id, COLOR)

    cells = brayns.get_circuit_ids(instance, circuit.id)
    method = brayns.CircuitColorMethod.ID
    colors = {
        str(cell): brayns.Color4(*COLORS[i % len(COLORS)])
        for i, cell in enumerate(cells)
    }
    brayns.color_model(instance, circuit.id, method, colors)

    quad = brayns.QuadLight(
        intensity=10,
        bottom_left=target.max + brayns.Axis.z * size,
        edge1=-brayns.Axis.y * size,
        edge2=-brayns.Axis.x * size,
    )
    front = brayns.add_light(instance, quad)

    quad = brayns.QuadLight(
        intensity=10,
        bottom_left=target.max + brayns.Axis.y * size,
        edge1=-brayns.Axis.x * size / 2,
        edge2=-brayns.Axis.z * size / 2,
    )
    top = brayns.add_light(instance, quad)

    controller = brayns.CameraController(
        target,
        aspect_ratio=RESOLUTION.aspect_ratio,
        rotation=brayns.CameraRotation.front,
    )
    camera = controller.camera
    distance = camera.distance * 1.2

    for i in range(STEPS):

        angle = i * 360 / STEPS
        controller.rotation = brayns.euler(0, angle, 0, degrees=True)
        camera = controller.camera
        camera.distance = distance

        transform = brayns.Transform.rotate(controller.rotation, target.center)
        front = brayns.update_model(instance, front.id, transform)
        top = brayns.update_model(instance, top.id, transform)

        snapshot = brayns.Snapshot(RESOLUTION, camera, RENDERER)
        snapshot.save(instance, PATTERN % i)

movie = brayns.Movie(PATTERN, FPS)
movie.save(MOVIE)
