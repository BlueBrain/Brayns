from typing import cast

import bluepy
import brayns
import numpy
import pandas
from bluepy.cells import CellCollection

CONFIG = "/gpfs/bbp.cscs.ch/project/proj112/scratch/circuits/20211110-BioM/CircuitConfig_struct"
MESH = "/gpfs/bbp.cscs.ch/project/proj112/entities/atlas/20211004_BioM/meshes/SP.stl"
URI = "r1i7n5.bbp.epfl.ch:5000"  # Change localhost with node ID
SNAPSHOT = "snapshot.jpg"

# Some colors to pick a random one for each GID, feel free to change it
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


def pick_color(index: int) -> brayns.Color4:
    return brayns.Color4(*COLORS[index % len(COLORS)])


numpy.random.seed(10)

# Extract positions and orientations from Bluepy

# Circuit
circuit = bluepy.Circuit(CONFIG)
cells = cast(CellCollection, circuit.cells)

# Target cells
data = cast(pandas.DataFrame, cells.get("SP_PC"))
index = data.index

# Gids
count = 15
gids = numpy.random.choice(index, count)

# Positions
data = cast(pandas.DataFrame, cells.positions(gids))
positions = data.to_numpy()

# Orientations
orientations = cast(pandas.DataFrame, cells.orientations(gids))
matrices = orientations.to_numpy()
transverse = [matrix[:, 0] for matrix in matrices]

# Send data to brayns

connector = brayns.Connector(URI)

with connector.connect() as instance:
    brayns.clear_models(instance)

    # Load mesh
    loader = brayns.MeshLoader()
    mesh = loader.load_models(instance, MESH)[0]

    # Mesh material (yours is not supported anymore)
    material = brayns.PhongMaterial()
    brayns.set_material(instance, mesh.id, material)

    # Color mesh
    color = brayns.Color4(90 / 255, 0, 0, 0.6)
    brayns.set_model_color(instance, mesh.id, color)

    # Add pills
    pills = list[tuple[brayns.Capsule, brayns.Color4]]()
    color = brayns.Color4(0, 230, 64)
    for position, orientation in zip(positions, transverse):
        end_point = position + 500 * orientation
        pill = brayns.Capsule(
            start_point=brayns.Vector3(*position),
            start_radius=25,
            end_point=brayns.Vector3(*end_point),
            end_radius=1,
        )
        pills.append((pill, color))
    brayns.add_geometries(instance, pills)

    # Load axons
    loader = brayns.BbpLoader(
        cells=brayns.BbpCells.from_gids([int(gid) for gid in gids]),
        morphology=brayns.Morphology(
            radius_multiplier=50,
            geometry_type=brayns.GeometryType.ORIGINAL,
            load_soma=False,
            load_dendrites=False,
            load_axon=True,
        ),
    )
    axons = loader.load_models(instance, CONFIG)[0]

    # Color by GID
    method = brayns.CircuitColorMethod.ID
    colors = {str(gid): pick_color(i) for i, gid in enumerate(gids)}
    brayns.color_model(instance, axons.id, method, colors)

    # Take snapshot to see result

    # Choose resolution
    resolution = brayns.Resolution.full_hd

    # Auto adjusted orthographic camera
    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=resolution.aspect_ratio,
        projection=brayns.OrthographicProjection,
    )
    camera = controller.camera

    # Some lights to see something
    lights = [brayns.DirectionalLight(intensity=10, direction=camera.direction)]
    for light in lights:
        brayns.add_light(instance, light)

    # Fast renderer, use ProductionRenderer to use realistic materials
    renderer = brayns.InteractiveRenderer()

    # Take snapshot
    snapshot = brayns.Snapshot(resolution, camera, renderer)
    snapshot.save(instance, SNAPSHOT)
