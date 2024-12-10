import libsonata

path = "/gpfs/bbp.cscs.ch/data/scratch/proj134/home/king/BBPP134-917/gpfs_simulation_config2.json"

# Parse JSON
simulation = libsonata.SimulationConfig.from_file(path)

# Circuit config
circuit = libsonata.CircuitConfig.from_file(simulation.network)

# Population names
population_names = circuit.node_populations
print(population_names)

# Pick one
population_name = next(population_names)
node_population = circuit.node_population(population_name)

# Pick all nodes
selection = node_population.select_all()

print(selection.flat_size())

# All but spikes :)
report_names = simulation.list_report_names

print(report_names)

# Holds report info
report = simulation.report(next(report_names))

print(dir(report))

# Read report H5
reader = libsonata.SomaReportReader(report.file_name)

print(reader.get_population_names())

# Read report for given population
population = reader[population_name]

print(population.times)
print(population.get_node_ids())

# Read one frame
frame = population.get(node_ids=selection, tstart=0, tstop=1)

# Spikes
spikes_file = f"{simulation.output.output_dir}/{simulation.output.spikes_file}"

reader = libsonata.SpikeReader(spikes_file)

# Same as for somas
population = reader[population_name]
population.get(tstart=0, tstop=1)
