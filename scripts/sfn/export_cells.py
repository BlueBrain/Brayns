import json

CELLS = "/home/acfleury/source/test/scripts/sfn/cells.json"
PRIMITIVES = "/home/acfleury/source/repos/Brayns2/ids.json"
OUTPUT = "/home/acfleury/source/test/scripts/sfn/cerebellum_cells.json"

NULL = 4294967295

with open(CELLS) as file:
    cells = json.load(file)

with open(PRIMITIVES) as file:
    primitives = json.load(file)

indices = {primitive for primitive in primitives if primitive < NULL}

visible_cells = sorted({cells[index] for index in indices})

with open(OUTPUT, "w") as file:
    json.dump(visible_cells, file)
