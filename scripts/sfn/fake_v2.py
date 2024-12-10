import json
import shutil
from pathlib import Path

import h5py
import numpy as np

SOURCE = Path(
    "/gpfs/bbp.cscs.ch/data/scratch/proj134/fullcircuit/full-synth-more-split-v2"
)
NODES = SOURCE / "nodes.h5"
NODE_SETS = SOURCE / "node_sets.json"
CONFIG = SOURCE / "circuit_config.json"
POPULATION = "root__neurons"

OUTPUT = Path(
    "/gpfs/bbp.cscs.ch/project/proj3/cloned_circuits/FullBrainSynthesizedWithAttributes"
)
MORPHOLOGY_OUTPUT = OUTPUT / "morphologies"
MORPHOLOGY_SKIP_FACTOR = 10_000

ROOT = f"nodes/{POPULATION}"
IDS = f"{ROOT}/node_type_id"
ATTRIBUTES = f"{ROOT}/0"
MORPHOLOGIES = f"{ATTRIBUTES}/morphology"


def get_group(file: h5py.File | h5py.Group, path: str) -> h5py.Group:
    dataset = file[path]
    assert isinstance(dataset, h5py.Group)
    return dataset


def get_dataset(file: h5py.File | h5py.Group, path: str) -> h5py.Dataset:
    dataset = file[path]
    assert isinstance(dataset, h5py.Dataset)
    return dataset


def copy_object(source: h5py.File, destination: h5py.File, path: str) -> None:
    item = source[path]

    if isinstance(item, h5py.Dataset):
        destination.create_dataset(path, data=item)
        return

    if isinstance(item, h5py.Group):
        for key in item.keys():
            copy_object(source, destination, f"{path}/{key}")
        return

    raise TypeError("Not a group nor dataset")


def copy_nodes(source: h5py.File, morphologies: np.ndarray) -> None:
    path = OUTPUT / "nodes.h5"

    with h5py.File(path, "w") as destination:
        copy_object(source, destination, IDS)

        destination.create_dataset(MORPHOLOGIES, data=morphologies)

        for attribute in get_group(source, ATTRIBUTES):
            if attribute == "morphology":
                continue

            copy_object(source, destination, f"{ATTRIBUTES}/{attribute}")


def get_source_path(morphology: bytes) -> Path:
    segments = [segment.decode() for segment in morphology.split(b"/")]
    source = SOURCE / Path(*segments)
    return source.with_suffix(".h5")


def get_destination_path(morphology: bytes) -> Path:
    filename = morphology.rsplit(b"/", maxsplit=1)[1].decode()
    destination = MORPHOLOGY_OUTPUT / filename
    return destination.with_suffix(".h5")


def copy_morphologies(file: h5py.File) -> np.ndarray:
    morphologies = get_dataset(file, MORPHOLOGIES)

    selected = morphologies[::MORPHOLOGY_SKIP_FACTOR]

    # sources = [get_source_path(morphology) for morphology in selected]
    destinations = [get_destination_path(morphology) for morphology in selected]

    # for source, destination in zip(sources, destinations):
    #     shutil.copyfile(source, destination)
    #     print(f"Copied {source} to {destination}")

    result = np.array(
        [destination.stem.encode() for destination in destinations],
        dtype=morphologies.dtype,
    )

    return np.repeat(result, MORPHOLOGY_SKIP_FACTOR)


def copy_node_sets() -> None:
    shutil.copyfile(NODE_SETS, OUTPUT / "node_sets.json")


def create_circuit_config() -> None:
    path = OUTPUT / "circuit_config.json"

    config = {
        "manifest": {
            "$BASE_DIR": str(OUTPUT),
        },
        "node_sets_file": "$BASE_DIR/node_sets.json",
        "networks": {
            "nodes": [
                {
                    "nodes_file": "$BASE_DIR/nodes.h5",
                    "populations": {
                        POPULATION: {
                            "type": "biophysical",
                            "alternate_morphologies": {
                                "h5v1": "$BASE_DIR/morphologies",
                            },
                            "biophysical_neuron_models_dir": "someInvalidStuff",
                        }
                    },
                }
            ],
            "edges": [],
        },
    }

    with path.open("w") as file:
        json.dump(config, file)


def main() -> None:
    # shutil.rmtree(OUTPUT, ignore_errors=True)

    # OUTPUT.mkdir(parents=True)
    # MORPHOLOGY_OUTPUT.mkdir(parents=True, exist_ok=True)

    # create_circuit_config()
    # copy_node_sets()

    with h5py.File(NODES) as file:
        morphologies = copy_morphologies(file)
        copy_nodes(file, morphologies)


if __name__ == "__main__":
    main()
