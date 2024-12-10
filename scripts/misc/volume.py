import math

import nrrd

PATH = "/gpfs/bbp.cscs.ch/project/proj3/TestData/install/share/BBPTestData/nrrd/density.nrrd"

data, header = nrrd.read(PATH)

x, y, z = data.shape

test = list[tuple[int, int, int, float]]()

for i in range(x):
    for j in range(y):
        for k in range(z):
            value = data[i, j, k]
            if not math.isnan(value):
                test.append((i, j, k, value))

for i in range(3):
    print(min(item[i] for item in test))
    print(max(item[i] for item in test))
