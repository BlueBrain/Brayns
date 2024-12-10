import time
from collections.abc import Iterator
from contextlib import contextmanager
from dataclasses import dataclass

import h5py
import libsonata
import numpy

PATH = "/gpfs/bbp.cscs.ch/project/proj3/cloned_circuits/FULL_BRAIN_WITH_SIM_15_06_2023/reports/soma.h5"
NODES = list(range(0, 70_000_000, 1_000))


@contextmanager
def measure(label: str) -> Iterator[None]:
    start = time.time()
    try:
        yield
    finally:
        print(f"{label}: {time.time() - start:.3f}s.")


@dataclass
class Report:
    data: h5py.Dataset
    node_ids: h5py.Dataset
    index_pointers: h5py.Dataset


def parse_report(file: h5py.File) -> Report:
    neurons = file["report/root__neurons"]
    assert isinstance(neurons, h5py.Group)
    data = neurons["data"]
    assert isinstance(data, h5py.Dataset)
    mapping = neurons["mapping"]
    assert isinstance(mapping, h5py.Group)
    node_ids = mapping["node_ids"]
    assert isinstance(node_ids, h5py.Dataset)
    index_pointers = mapping["index_pointers"]
    assert isinstance(index_pointers, h5py.Dataset)
    return Report(data, node_ids, index_pointers)


def average(data: numpy.ndarray) -> float:
    return sum(data) / len(data)


def extract_frame(report: Report, index: int, nodes: list[int]) -> list[float]:
    index_pointers = report.index_pointers
    data = report.data
    by_element = [data[index, index_pointers[i] : index_pointers[i + 1]] for i in nodes]
    return [average(i) for i in by_element]


print("Node count:", len(NODES))

with measure("libsonata open reader"):
    reader = libsonata.SomaReportReader(PATH)

with measure("libsonata open population"):
    population = reader["root__neurons"]

with measure("libsonata read frame"):
    frame = population.get(NODES, 0, 0)

data1 = frame.data[0]

with h5py.File(PATH) as file:
    with measure("h5py open population"):
        report = parse_report(file)

    with measure("h5py read frame naive"):
        frame = extract_frame(report, 0, NODES)

    with measure("h5py read frame from indices"):
        data3 = report.data[0, NODES]

data2 = numpy.array(frame)

assert all(abs(i - j) < 0.0001 for i, j in zip(data1, data2))
assert all(abs(i - j) < 0.0001 for i, j in zip(data1, data3))
