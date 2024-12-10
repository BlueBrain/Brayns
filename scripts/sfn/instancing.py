from collections import defaultdict
from collections.abc import Iterable
from dataclasses import dataclass
from difflib import SequenceMatcher
from pathlib import Path
import brayns
import libsonata
import numpy as np

NODES = Path(
    "/gpfs/bbp.cscs.ch/project/proj3/cloned_circuits/FullBrainSynthesized/nodes.h5"
)

POPULATION = "default"

MORPHOLOGIES = Path(
    "/gpfs/bbp.cscs.ch/project/proj3/cloned_circuits/FullBrainSynthesized/morphologies"
)

RESAMPLING = 0.99
SUBSAMPLING = 2


@dataclass
class Circuit:
    ids: np.ndarray
    regions: np.ndarray
    region_indices: np.ndarray
    xs: np.ndarray
    ys: np.ndarray
    zs: np.ndarray
    qxs: np.ndarray
    qys: np.ndarray
    qzs: np.ndarray
    qws: np.ndarray


class Deck:
    def __init__(self) -> None:
        self._data = list[Path]()
        self._index = 0

    def add(self, item: Path) -> None:
        self._data.append(item)

    def get(self) -> Path:
        item = self._data[self._index]
        self._index = (self._index + 1) % len(self._data)
        return item


def get_region(path: Path) -> str:
    return path.stem.rsplit("_", 1)[0]


def get_available_morphologies_per_region() -> dict[str, Deck]:
    result = defaultdict[str, Deck](Deck)

    for path in MORPHOLOGIES.glob("*.h5"):
        region = get_region(path)
        result[region].add(path)

    return result


def closest(values: Iterable[str], ref: str) -> str:
    return max(values, key=lambda x: SequenceMatcher(None, x, ref).ratio())


def pick_morphology(morphologies: dict[str, Deck], region: str) -> Path:
    available = morphologies.get(region)

    if available is None:
        best = closest(morphologies.keys(), region)
        available = morphologies[best]

    return available.get()


def dispatch_morphologies(
    morphologies: dict[str, Deck], regions: Iterable[str]
) -> dict[str, Path]:
    return {region: pick_morphology(morphologies, region) for region in regions}


def parse_circuit(ids: list[int]) -> Circuit:
    nodes = libsonata.NodeStorage(str(NODES))
    population = nodes.open_population(POPULATION)
    selection = libsonata.Selection(ids)

    return Circuit(
        ids=selection.flatten(),
        regions=population.enumeration_values("region"),
        region_indices=population.get_attribute("region", selection),
        xs=population.get_attribute("x", selection),
        ys=population.get_attribute("y", selection),
        zs=population.get_attribute("z", selection),
        qxs=population.get_attribute("orientation_x", selection),
        qys=population.get_attribute("orientation_y", selection),
        qzs=population.get_attribute("orientation_z", selection),
        qws=population.get_attribute("orientation_w", selection),
    )


def get_transforms(circuit: Circuit) -> list[brayns.Transform]:
    return [
        brayns.Transform(
            translation=brayns.Vector3(x, y, z),
            rotation=brayns.Rotation.from_quaternion(brayns.Quaternion(qx, qy, qz, qw)),
        )
        for x, y, z, qx, qy, qz, qw in zip(
            circuit.xs,
            circuit.ys,
            circuit.zs,
            circuit.qxs,
            circuit.qys,
            circuit.qzs,
            circuit.qws,
        )
    ]


def get_morphologies_per_region(circuit: Circuit) -> dict[str, Path]:
    available = get_available_morphologies_per_region()
    return dispatch_morphologies(available, circuit.regions)


def load_morphologies(
    instance: brayns.Instance, paths: dict[str, Path], growth: float
) -> dict[str, brayns.Model]:
    morphology = brayns.Morphology(
        radius_multiplier=1,
        load_soma=True,
        load_dendrites=True,
        resampling=RESAMPLING,
        subsampling=SUBSAMPLING,
        growth=growth,
    )
    loader = brayns.MorphologyLoader(morphology)

    return {
        key: loader.load_models(instance, str(path))[0] for key, path in paths.items()
    }


def main() -> None:
    circuit = parse_circuit()
    morphologies = get_morphologies_per_region(circuit)


if __name__ == "__main__":
    main()
