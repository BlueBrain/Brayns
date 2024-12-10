import time
from collections.abc import Iterator
from contextlib import contextmanager
from dataclasses import dataclass

import h5py
import libsonata
import numpy

PATH = "/gpfs/bbp.cscs.ch/project/proj3/cloned_circuits/FULL_BRAIN_WITH_SIM_15_06_2023/reports/soma.h5"
NODES = list(range(0, 70_000_000, 1_000))
START_FRAME = 0
NUM_FRAMES = 1


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
    time: h5py.Dataset


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
    time = mapping["time"]
    assert isinstance(time, h5py.Dataset)
    return Report(data, node_ids, index_pointers, time)


print("Node count:", len(NODES))
print(f"START_FRAME: {START_FRAME}, NUM_FRAMES: {NUM_FRAMES}")

# Create indices list and new nodes ordered acording to that indices
with h5py.File(PATH) as file:
    report = parse_report(file)
    with measure("get indices"):
        selected_indices = numpy.isin(report.node_ids[:], NODES).nonzero()[0]

    new_nodes = [report.node_ids[i] for i in selected_indices]

# Read the data with libsonata
with measure("libsonata open reader"):
    reader = libsonata.SomaReportReader(PATH)

with measure("libsonata open population"):
    population = reader["root__neurons"]

with measure("libsonata read frame"):
    report_dt = 0.1
    full_data = population.get(
        new_nodes,
        tstart=START_FRAME * report_dt,
        tstop=(START_FRAME + NUM_FRAMES - 1) * report_dt,
    )

# Read the data with h5py
with h5py.File(PATH) as file:
    report = parse_report(file)
    with measure("h5py read frame from indices"):
        full_data_h5py = report.data[
            START_FRAME : START_FRAME + NUM_FRAMES, selected_indices
        ]

assert numpy.allclose(
    full_data_h5py, full_data.data, atol=0.0001
), "indices_h5py vs libsonata. Arrays are not equal"
print("All frames are equal!")
