import json
import shutil
from dataclasses import dataclass
from pathlib import Path
from typing import Any, TypeVar, get_origin

import h5py
import libsonata
import numpy

PATH = "/gpfs/bbp.cscs.ch/project/proj3/cloned_circuits/FULL_BRAIN_WITH_SIM_15_06_2023/circuit_config.json"
PATH = "/gpfs/bbp.cscs.ch/project/proj143/home/arnaudon/proj143/circuit-build/auxiliary/circuit_config_hpc.json"
OUTPUT = "circuit"
COUNT = 10


@dataclass
class NodePopulation:
    type: str
    name: str
    morphology_directory: Path
    morphology_extension: str
    hoc_directory: Path | None = None


@dataclass
class NodeGroup:
    node_file: Path
    populations: list[NodePopulation]


@dataclass
class EdgePopulation:
    type: str
    name: str


@dataclass
class EdgeGroup:
    edge_file: Path
    populations: list[EdgePopulation]


@dataclass
class Circuit:
    node_sets: Path | None
    nodes: list[NodeGroup]
    edges: list[EdgeGroup]


@dataclass
class Report:
    name: str
    population: str
    start: float
    stop: float
    step: float
    nodes: list[int]
    frames: list[list[float]]
    node_set: str = "All"
    variable: str = "v"
    type: str = "compartment"
    unit: str = "mV"
    time_unit: str = "ms"


@dataclass
class Simulation:
    circuit: Path
    node_sets: Path | None
    output_directory: Path
    reports: list[Report]
    start: float
    stop: float
    step: float


@dataclass
class Selection:
    population: str
    indices: list[int] | None = None


T = TypeVar("T")
U = TypeVar("U")


def check_type(value: Any, t: type[T]) -> T:
    origin = get_origin(t)
    if origin is None:
        origin = t
    if not isinstance(value, origin):
        raise TypeError(f"Expected {origin} got {type(value)}")
    return value


def try_get(items: dict[str, Any], key: str, t: type[T], default: U = None) -> T | U:
    if key in items:
        value = items[key]
        return check_type(value, t)
    return default


def get(items: dict[str, Any], key: str, t: type[T]) -> T:
    value = items[key]
    return check_type(value, t)


def substitute(item: str, manifest: dict[str, str]) -> str:
    for key, value in manifest.items():
        item = item.replace(key, value)
    return item


def get_path(value: str, manifest: dict[str, str]) -> Path:
    value = substitute(value, manifest)
    return Path(value)


def make_absolute(path: Path, directory: Path) -> Path:
    if path.is_absolute():
        return path
    return directory / path


def make_relative(path: Path, directory: Path) -> Path:
    if path.is_relative_to(directory):
        return path.relative_to(directory)
    return path


def get_morphology_directory_and_extension(
    population: dict[str, Any]
) -> tuple[str, str]:
    morphology_directory = try_get(population, "morphologies_dir", str)
    if morphology_directory is not None:
        return morphology_directory, "swc"
    alternate = get(population, "alternate_morphologies", dict)
    asc_directory = try_get(alternate, "neurolucida-asc", str)
    if asc_directory is not None:
        return asc_directory, "asc"
    h5_directory = try_get(alternate, "h5v1", str)
    if h5_directory is not None:
        return h5_directory, "h5"
    raise ValueError("Cannot find morphology directory")


def deserialize_node_population(
    name: str, population: dict[str, Any], manifest: dict[str, str]
) -> NodePopulation:
    type = get(population, "type", str)
    directory, extension = get_morphology_directory_and_extension(population)
    directory = get_path(directory, manifest)
    hoc_directory = try_get(population, "biophysical_neuron_models_dir", str, "")
    hoc_directory = get_path(hoc_directory, manifest)
    return NodePopulation(
        type=type,
        name=name,
        morphology_directory=Path(directory),
        morphology_extension=extension,
        hoc_directory=hoc_directory,
    )


def deserialize_node_populations(
    populations: dict[str, Any], manifest: dict[str, str]
) -> list[NodePopulation]:
    result = list[NodePopulation]()
    for name, population in populations.items():
        name = check_type(name, str)
        population = check_type(population, dict)
        item = deserialize_node_population(name, population, manifest)
        result.append(item)
    return result


def deserialize_node(node: dict[str, Any], manifest: dict[str, str]) -> NodeGroup:
    node_file = get(node, "nodes_file", str)
    node_file = get_path(node_file, manifest)
    populations = get(node, "populations", dict)
    return NodeGroup(
        node_file=Path(node_file),
        populations=deserialize_node_populations(populations, manifest),
    )


def deserialize_nodes(nodes: list[Any], manifest: dict[str, str]) -> list[NodeGroup]:
    result = list[NodeGroup]()
    for node in nodes:
        node = check_type(node, dict)
        item = deserialize_node(node, manifest)
        result.append(item)
    return result


def deserialize_edge_population(
    name: str, population: dict[str, Any]
) -> EdgePopulation:
    type = get(population, "type", str)
    return EdgePopulation(
        type=type,
        name=name,
    )


def deserialize_edge_populations(populations: dict[str, Any]) -> list[EdgePopulation]:
    result = list[EdgePopulation]()
    for name, population in populations.items():
        name = check_type(name, str)
        population = check_type(population, dict)
        item = deserialize_edge_population(name, population)
        result.append(item)
    return result


def deserialize_edge(edge: dict[str, Any], manifest: dict[str, str]) -> EdgeGroup:
    edge_file = get(edge, "edges_file", str)
    edge_file = get_path(edge_file, manifest)
    populations = get(edge, "populations", dict)
    return EdgeGroup(
        edge_file=Path(edge_file),
        populations=deserialize_edge_populations(populations),
    )


def deserialize_edges(edges: list[Any], manifest: dict[str, str]) -> list[EdgeGroup]:
    result = list[EdgeGroup]()
    for edge in edges:
        edge = check_type(edge, dict)
        item = deserialize_edge(edge, manifest)
        result.append(item)
    return result


def deserialize_circuit(circuit: dict[str, Any]) -> Circuit:
    manifest = try_get(circuit, "manifest", dict[str, str], {})
    node_sets = try_get(circuit, "node_sets_file", str, None)
    if node_sets is not None:
        node_sets = get_path(node_sets, manifest)
        node_sets = Path(node_sets)
    networks = get(circuit, "networks", dict)
    nodes = get(networks, "nodes", list)
    edges = get(networks, "edges", list)
    return Circuit(
        node_sets=node_sets,
        nodes=deserialize_nodes(nodes, manifest),
        edges=deserialize_edges(edges, manifest),
    )


def make_absolute_paths(circuit: Circuit, directory: Path) -> None:
    if circuit.node_sets is not None:
        circuit.node_sets = make_absolute(circuit.node_sets, directory)
    for node in circuit.nodes:
        node.node_file = make_absolute(node.node_file, directory)
        for population in node.populations:
            population.morphology_directory = make_absolute(
                population.morphology_directory, directory
            )
            if population.hoc_directory is not None:
                population.hoc_directory = make_absolute(
                    population.hoc_directory, directory
                )


def read_circuit(path: Path) -> Circuit:
    with path.open() as file:
        data = json.load(file)
    circuit = deserialize_circuit(data)
    make_absolute_paths(circuit, path.parent)
    return circuit


def get_alternate_morphologies(directory: Path, extension: str) -> dict[str, str]:
    if extension == "asc":
        return {"neurolucida-asc": str(directory)}
    if extension == "h5":
        return {"h5v1": str(directory)}
    raise ValueError(f"Invalid morphology extension: '{extension}'")


def get_morphology_directory(directory: Path, extension: str) -> dict[str, Any]:
    if extension == "swc":
        return {"morphologies_dir": str(directory)}
    return {"alternate_morphologies": get_alternate_morphologies(directory, extension)}


def get_node_properties(population: NodePopulation) -> dict[str, Any]:
    result = {"type": population.type}
    if population.hoc_directory is not None:
        result["biophysical_neuron_models_dir"] = str(population.hoc_directory)
    return result


def serialize_node_population(population: NodePopulation) -> dict[str, Any]:
    return {
        **get_node_properties(population),
        **get_morphology_directory(
            population.morphology_directory,
            population.morphology_extension,
        ),
    }


def serialize_node(node: NodeGroup) -> dict[str, Any]:
    return {
        "nodes_file": str(node.node_file),
        "populations": {
            population.name: serialize_node_population(population)
            for population in node.populations
        },
    }


def serialize_edge(edge: EdgeGroup) -> dict[str, Any]:
    return {
        "edges_file": str(edge.edge_file),
        "populations": {
            population.name: {"type": population.type}
            for population in edge.populations
        },
    }


def serialize_circuit(circuit: Circuit) -> dict[str, Any]:
    result = {
        "version": 2,
        "networks": {
            "nodes": [serialize_node(node) for node in circuit.nodes],
            "edges": [serialize_edge(edge) for edge in circuit.edges],
        },
    }
    if circuit.node_sets is not None:
        result["node_sets_file"] = str(circuit.node_sets)
    return result


def make_circuit_paths_relative(circuit: Circuit, directory: Path) -> None:
    if circuit.node_sets is not None:
        circuit.node_sets = make_relative(circuit.node_sets, directory)
    for group in circuit.nodes:
        group.node_file = make_relative(group.node_file, directory)
        for population in group.populations:
            population.morphology_directory = make_relative(
                population.morphology_directory, directory
            )
            if population.hoc_directory is not None:
                population.hoc_directory = make_relative(
                    population.hoc_directory, directory
                )
    for group in circuit.edges:
        group.edge_file = make_relative(group.edge_file, directory)


def write_circuit(circuit: Circuit, path: Path) -> None:
    data = serialize_circuit(circuit)
    with path.open("w") as file:
        json.dump(data, file, indent=4)


def get_group(file: h5py.File, path: str) -> h5py.Group:
    group = file[path]
    if not isinstance(group, h5py.Group):
        raise TypeError(f"Expected a group at {path}")
    return group


def get_dataset(file: h5py.File, path: str) -> h5py.Dataset:
    data = file[path]
    if not isinstance(data, h5py.Dataset):
        raise TypeError(f"Expected a dataset at {path}")
    return data


def clone(
    source: h5py.File,
    destination: h5py.File,
    path: str,
    indices: list[int] | None = None,
    ignores: set[str] | None = None,
) -> None:
    if ignores is not None and path in ignores:
        return
    data = source[path]
    if isinstance(data, h5py.Dataset):
        shape = data.shape if indices is None else (len(indices),)
        values = destination.create_dataset(path, shape, data.dtype)
        values.attrs.update(data.attrs)
        values[:] = data[:] if indices is None else [data[index] for index in indices]
        return
    if isinstance(data, h5py.Group):
        group = destination.require_group(path)
        group.attrs.update(data.attrs)
        for key in data.keys():
            clone(source, destination, f"{path}/{key}", indices, ignores)
        return


def clone_node_population(
    source: h5py.File, destination: h5py.File, selection: Selection
) -> None:
    root = f"nodes/{selection.population}"
    library = f"{root}/0/@library"
    clone(source, destination, library)
    clone(source, destination, root, selection.indices, ignores={library})


def clone_nodes(node_file: Path, output: Path, selections: list[Selection]) -> None:
    with h5py.File(node_file) as source, h5py.File(output, "w") as destination:
        for selection in selections:
            clone_node_population(source, destination, selection)


def get_morphologies(storage: libsonata.NodeStorage, selection: Selection) -> list[str]:
    population = storage.open_population(selection.population)
    s = population.select_all()
    indices = selection.indices
    if indices is not None:
        s = libsonata.Selection(indices)
    data = population.get_attribute("morphology", s)
    return list(data)


def write_morphologies(
    storage: libsonata.NodeStorage,
    morphology_directory: Path,
    morphology_extension: str,
    output_directory: Path,
    selection: Selection,
) -> None:
    names = get_morphologies(storage, selection)
    for name in names:
        filename = f"{name}.{morphology_extension}"
        source = morphology_directory / filename
        destination = output_directory / filename
        destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy(source, destination)


def clone_morphologies(
    node: NodeGroup, output_directory: Path, selections: list[Selection]
) -> None:
    storage = libsonata.NodeStorage(node.node_file)
    for selection in selections:
        population = next(
            population
            for population in node.populations
            if population.name == selection.population
        )
        directory = population.morphology_directory
        extension = population.morphology_extension
        output = output_directory / population.name
        write_morphologies(storage, directory, extension, output, selection)


def get_data(report: Report) -> numpy.ndarray:
    return numpy.array(report.frames, numpy.float32)


def get_node_ids(report: Report) -> numpy.ndarray:
    return numpy.array(report.nodes, numpy.uint64)


def get_index_pointers(report: Report) -> numpy.ndarray:
    return numpy.array(range(len(report.nodes) + 1), numpy.uint64)


def get_element_ids(report: Report) -> numpy.ndarray:
    return numpy.zeros(len(report.nodes), numpy.uint64)


def get_time(report: Report) -> numpy.ndarray:
    return numpy.array([report.start, report.stop, report.step], numpy.float64)


def write_report(file: h5py.File, report: Report) -> None:
    root = f"report/{report.population}"
    mapping = f"{root}/mapping"
    data = file.create_dataset(f"{root}/data", data=get_data(report))
    data.attrs["units"] = report.unit
    file.create_dataset(f"{mapping}/node_ids", data=get_node_ids(report))
    file.create_dataset(f"{mapping}/index_pointers", data=get_index_pointers(report))
    file.create_dataset(f"{mapping}/element_ids", data=get_element_ids(report))
    time = file.create_dataset(f"{mapping}/time", data=get_time(report))
    time.attrs["units"] = report.time_unit


def make_simulation_paths_relative(simulation: Simulation, directory: Path) -> None:
    simulation.circuit = make_relative(simulation.circuit, directory)
    simulation.output_directory = make_relative(simulation.output_directory, directory)
    if simulation.node_sets is not None:
        simulation.node_sets = make_relative(simulation.node_sets, directory)


def serialize_simulation(simulation: Simulation) -> dict[str, Any]:
    data = {
        "network": str(simulation.circuit),
        "run": {
            "tstart": simulation.start,
            "tstop": simulation.stop,
            "dt": simulation.step,
            "spike_threshold": -15,
            "random_seed": 0,
        },
        "output": {
            "output_dir": str(simulation.output_directory),
        },
        "reports": {
            report.name: {
                "cells": report.node_set,
                "variable_name": report.variable,
                "type": report.type,
                "dt": report.step,
                "start_time": report.start,
                "end_time": report.stop,
            }
            for report in simulation.reports
        },
    }
    if simulation.node_sets is not None:
        data["node_sets_file"] = str(simulation.node_sets)
    return data


def write_simulation(simulation: Simulation, path: Path) -> None:
    data = serialize_simulation(simulation)
    with path.open("w") as file:
        json.dump(data, file, indent=4)


def main() -> None:
    source = Path(PATH)

    circuit = read_circuit(source)

    # Don't clone edges for now
    circuit.edges = []

    # Take only one node group for now
    node = circuit.nodes[0]
    circuit.nodes = [node]

    # Take a few nodes of all populations
    selections = [
        Selection(population.name, list(range(COUNT)))
        for population in node.populations
    ]

    # Output settings
    output_directory = Path(OUTPUT).absolute()
    output_circuit = output_directory / "circuit_config.json"
    output_nodes = output_directory / "node.h5"
    output_morphologies = output_directory / "morphologies"
    output_nodesets = output_directory / "node_sets.json"
    output_simulation = output_directory / "simulation_config.json"
    output_report = output_directory / "test.h5"

    # Clean and create output directories if needed
    shutil.rmtree(output_directory, ignore_errors=True)
    output_directory.mkdir(parents=True, exist_ok=True)
    output_morphologies.mkdir(parents=True, exist_ok=True)

    # Node file (H5)
    clone_nodes(node.node_file, output_nodes, selections)

    # Clone nodesets file (JSON)
    if circuit.node_sets is not None:
        shutil.copy(circuit.node_sets, output_nodesets)
        circuit.node_sets = output_nodesets.absolute()

    # Clone morphologies for all populations in selection
    clone_morphologies(node, output_morphologies, selections)
    node.node_file = output_nodes.absolute()
    for population in node.populations:
        directory = output_morphologies / population.name
        population.morphology_directory = directory.absolute()

    # Backup node sets absolute path
    node_sets = circuit.node_sets

    # Clone updated circuit config
    make_circuit_paths_relative(circuit, output_directory)
    write_circuit(circuit, output_circuit)

    # Write fake report
    selection = selections[0]
    indices = selection.indices if selection.indices is not None else []
    report = Report(
        name="test",
        population=selection.population,
        start=0,
        stop=3,
        step=1,
        nodes=indices,
        frames=[
            [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
            [0, 0, 0, 3, 3, 3, 1, 2, 2, 2],
            [0, 0, 2, 2, 4, -1, 1, 1, 3, 3],
        ],
    )

    output_report.parent.mkdir(parents=True, exist_ok=True)
    with h5py.File(output_report, "w") as file:
        write_report(file, report)

    # Write fake simulation config
    simulation = Simulation(
        circuit=output_circuit.absolute(),
        node_sets=None if node_sets is None else node_sets.absolute(),
        output_directory=output_report.parent.absolute(),
        reports=[report],
        start=0,
        stop=3,
        step=1,
    )
    make_simulation_paths_relative(simulation, output_directory)
    write_simulation(simulation, output_simulation)


if __name__ == "__main__":
    main()
