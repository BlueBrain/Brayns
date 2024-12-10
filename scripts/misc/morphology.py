from collections.abc import Generator

from morphio import Morphology

SWC = "assets/GolgiCell.swc"
ASC = "assets/GolgiCell.asc"
H5 = "assets/GolgiCell.h5"

swc = Morphology(SWC)
asc = Morphology(ASC)
h5 = Morphology(H5)


def get_samples(morphology: Morphology) -> Generator:
    for point, diameter in zip(morphology.points, morphology.diameters):
        yield (point[0], point[1], point[2], diameter)


for sample in get_samples(asc):
    print(sample)

asc.as_mutable().write("test.swc")
