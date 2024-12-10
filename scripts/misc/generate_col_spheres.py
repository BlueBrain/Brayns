import csv
from typing import cast

import bluepy
import brayns
from bluepy.cells import CellCollection

CONFIG = "/gpfs/bbp.cscs.ch/project/proj96/home/ecker/simulations/d21efd45-7740-4268-b06f-e1de35f2b6cf/1/BlueConfig"
NODES = "/gpfs/bbp.cscs.ch/project/proj83/circuits/Bio_M/20200805/sonata/networks/nodes/All/nodes.h5"
RADIUS = 10
URI = "localhost:5000"

# circuit = bluepy.Circuit(CONFIG)
# cells = cast(CellCollection, circuit.cells)

# positions = cells.get("hex_O1", ["x", "y", "z"])
# xs = positions["x"]
# ys = positions["y"]
# zs = positions["z"]

# with open("spheres.csv", "w") as file:
#     writer = csv.writer(file)
#     writer.writerows([x, y, z, RADIUS] for x, y, z in zip(xs, ys, zs))

connector = brayns.Connector(URI)

with connector.connect() as instance:
    brayns.clear_models(instance)

    brayns.add_light(
        instance,
        brayns.DirectionalLight(intensity=5, direction=brayns.Vector3(1, -1, -1)),
    )

    # spheres = [
    #     brayns.Sphere(RADIUS, center=brayns.Vector3(x, y, z))
    #     for x, y, z in zip(xs, ys, zs)
    # ]

    # brayns.add_geometries(
    #     instance, [(sphere, brayns.Color4.blue) for sphere in spheres]
    # )

    loader = brayns.MeshLoader()

    loader.load_models(instance, "/home/acfleury/Downloads/hull.obj")

    resolution = brayns.Resolution.ultra_hd

    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=resolution.aspect_ratio,
    )
    camera = controller.camera

    renderer = brayns.InteractiveRenderer()

    snapshot = brayns.Snapshot(resolution, camera, renderer)
    snapshot.save(instance, "test.png")
