import csv

import libsonata
import morphio
import numpy
from scipy.spatial.transform import Rotation

PATH = "/gpfs/bbp.cscs.ch/project/proj137/NGVCircuits/rat_sscx_S1HL/V10/build/ngv_config.json"
MORPHOLOGY_FOLDER = (
    "/gpfs/bbp.cscs.ch/project/proj83/entities/fixed-ais-L23PC-2020-12-10/ascii"
)
EXTENSION = "asc"
GIDS = [101720]


circuit = libsonata.CircuitConfig.from_file(PATH)

population = circuit.node_population("All")
selection = libsonata.Selection(GIDS)

xs = population.get_attribute("x", selection)
ys = population.get_attribute("y", selection)
zs = population.get_attribute("z", selection)

rxs = population.get_attribute("orientation_x", selection)
rys = population.get_attribute("orientation_y", selection)
rzs = population.get_attribute("orientation_z", selection)
rws = population.get_attribute("orientation_w", selection)

filenames = population.get_attribute("morphology", selection)

for i, gid in enumerate(GIDS):
    translation = numpy.array([xs[i], ys[i], zs[i]])
    rotation = Rotation.from_quat([rxs[i], rys[i], rzs[i], rws[i]])
    path = f"{MORPHOLOGY_FOLDER}/{filenames[i]}.{EXTENSION}"

    morphology = morphio.Morphology(path)

    points = [
        point
        for section in morphology.sections
        for point in section.points
        if section.type != morphio.SectionType.axon
    ]

    points = [rotation.apply(point) + translation for point in points]

    with open(f"morphology_{gid}.csv", "wt") as file:
        writer = csv.writer(file)
        writer.writerows(points)
