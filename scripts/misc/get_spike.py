from dataclasses import dataclass

import libsonata

PATH = "/gpfs/bbp.cscs.ch/data/scratch/proj134/home/king/BBPP134-917/gpfs_simulation_config2.json"
FRAME = 970


@dataclass
class Spike:
    index: int
    t: float
    nodes: set[int]


config = libsonata.SimulationConfig.from_file(PATH)

spikes_file = f"{config.output.output_dir}/{config.output.spikes_file}"

reader = libsonata.SpikeReader(spikes_file)

population = reader["root__neurons"]

dt = 0.01

index = FRAME

start, end = population.times

t = start + index * dt

frame = population.get(tstart=t, tstop=t)

spike = Spike(index, t, set(gid for gid, _ in frame))

print(list(spike.nodes))
