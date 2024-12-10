from collections import defaultdict
from collections.abc import Iterable
from dataclasses import dataclass
from pathlib import Path
from difflib import SequenceMatcher

import h5py
import numpy as np

INPUT = "/gpfs/bbp.cscs.ch/project/proj3/cloned_circuits/FullBrainSynthesized/nodes.h5"
OUTPUT = (
    "/gpfs/bbp.cscs.ch/project/proj3/cloned_circuits/FullBrainSynthesized/nodes_fake.h5"
)
MORPHOLOGIES = (
    "/gpfs/bbp.cscs.ch/project/proj3/cloned_circuits/FullBrainSynthesized/morphologies"
)

FLOAT = "<f8"
INDEX = "uint32"
STRING = "O"

POPULATION = "default"


@dataclass
class Circuit:
    ids: h5py.Dataset
    regions: h5py.Dataset
    region_indices: h5py.Dataset
    xs: h5py.Dataset
    ys: h5py.Dataset
    zs: h5py.Dataset
    qxs: h5py.Dataset
    qys: h5py.Dataset
    qzs: h5py.Dataset
    qws: h5py.Dataset
    morphologies: h5py.Dataset


Morphologies = dict[str, dict[str, Path]]


def get_group(file: h5py.File | h5py.Group, path: str) -> h5py.Group:
    dataset = file[path]
    assert isinstance(dataset, h5py.Group)
    return dataset


def get_dataset(file: h5py.File | h5py.Group, path: str) -> h5py.Dataset:
    dataset = file[path]
    assert isinstance(dataset, h5py.Dataset)
    return dataset


def parse_circuit(file: h5py.File) -> Circuit:
    root = get_group(file, f"nodes/{POPULATION}")

    ids = get_dataset(root, "node_type_id")

    attributes = get_group(root, "0")

    regions = get_dataset(attributes, "@library/region")
    region_indices = get_dataset(attributes, "region")

    xs = get_dataset(attributes, "x")
    ys = get_dataset(attributes, "y")
    zs = get_dataset(attributes, "z")

    qxs = get_dataset(attributes, "orientation_x")
    qys = get_dataset(attributes, "orientation_y")
    qzs = get_dataset(attributes, "orientation_z")
    qws = get_dataset(attributes, "orientation_w")

    morphologies = get_dataset(attributes, "morphology")

    return Circuit(
        ids, regions, region_indices, xs, ys, zs, qxs, qys, qzs, qws, morphologies
    )


def get_available_morphologies() -> Morphologies:
    morphologies = defaultdict[str, dict[str, Path]](dict)

    paths = list(Path(MORPHOLOGIES).glob("*.h5"))

    for path in paths:
        group, subgroup = path.stem.rsplit("_", 1)
        assert subgroup not in morphologies[group]
        morphologies[group][subgroup] = path

    return Morphologies(morphologies)


def closest(values: Iterable[str], ref: str) -> str:
    return max(values, key=lambda x: SequenceMatcher(None, x, ref).ratio())


def get_or_add(morphologies: Morphologies, group: str, subgroup: str) -> Path:
    sublist = morphologies.get(group)

    if sublist is None:
        best = closest(morphologies.keys(), group)
        sublist = morphologies[best]
        morphologies[group] = sublist

    path = sublist.get(subgroup)

    if path is None:
        best = closest(sublist.keys(), subgroup)
        path = sublist[best]
        sublist[subgroup] = path

    return path


def replace_morphologies(
    originals: h5py.Dataset, morphologies: Morphologies
) -> np.ndarray:
    result = np.empty(originals.shape, originals.dtype)

    for index, original in enumerate(originals):
        assert isinstance(original, bytes)
        group, _, _, subgroup, *_ = original.split(b"/")
        group = group.removesuffix(b".h5").decode()
        subgroup = subgroup.decode()
        path = get_or_add(morphologies, group, subgroup)
        result[index] = path.stem

    return result


def create_fake(file: h5py.File, circuit: Circuit, morphologies: np.ndarray) -> None:
    root = file.create_group(f"nodes/{POPULATION}")

    root.create_dataset("node_type_id", data=circuit.ids)

    attributes = root.create_group("0")

    attributes.create_dataset("@library/region", data=circuit.regions)
    attributes.create_dataset("region", data=circuit.region_indices)

    attributes.create_dataset("x", data=circuit.xs)
    attributes.create_dataset("y", data=circuit.ys)
    attributes.create_dataset("z", data=circuit.zs)

    attributes.create_dataset("orientation_x", data=circuit.qxs)
    attributes.create_dataset("orientation_y", data=circuit.qys)
    attributes.create_dataset("orientation_z", data=circuit.qzs)
    attributes.create_dataset("orientation_w", data=circuit.qws)

    attributes.create_dataset("morphology", data=morphologies)


def main() -> None:
    available = get_available_morphologies()

    with h5py.File(INPUT) as file:
        circuit = parse_circuit(file)

        morphologies = replace_morphologies(circuit.morphologies, available)

        with open("test.npy", "wb") as file:
            np.save(file, morphologies)

        with h5py.File(OUTPUT, "w") as file2:
            create_fake(file2, circuit, morphologies)


if __name__ == "__main__":
    main()
