import libsonata
import random
import json

PATH = "/gpfs/bbp.cscs.ch/project/proj3/cloned_circuits/FullBrainSynthesized/circuit_config.json"
OUTPUT = "/home/acfleury/source/test/scripts/sfn/colors.json"

circuit = libsonata.CircuitConfig.from_file(PATH)
population = circuit.node_population("default")
regions = population.enumeration_values("region")

colors = {
    region: [random.uniform(0, 1), random.uniform(0, 1), random.uniform(0, 1)]
    for region in regions
}

with open(OUTPUT, "w") as file:
    json.dump(colors, file)
