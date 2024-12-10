from dataclasses import dataclass

import libsonata

PATH = "/gpfs/bbp.cscs.ch/data/scratch/proj134/home/king/BBPP134-917/gpfs_simulation_config2.json"


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

index = 0

start, end = population.times

spikes = list[Spike]()

while True:
    t = start + index * dt

    if t >= end:
        break

    frame = population.get(tstart=t, tstop=t)

    if not frame:
        print(f"No data for {index=}")
        index += 1
        continue

    spike = Spike(index, t, set(gid for gid, _ in frame))
    spikes.append(spike)

    print(f"{index=}, {t=}: {len(spike.nodes)}")

    index += 1

print(max(len(spike.nodes) for spike in spikes))
