import csv
import json

CSV = "people.csv"
JSON = "people.json"
OUTPUT = "output.json"
UNKNOWN = "unknown.json"
ENTRIES = "entries.json"

with open(JSON) as file:
    data: dict[str, list[str]] = json.load(file)

with open(CSV, encoding="latin-1") as file:
    reader = csv.reader(file)
    i = iter(reader)
    next(i)
    ref = [(row[0], "000" + row[1]) for row in i]

entries = list[tuple[str, str]]()
unknown = list[str]()

for name, id in ref:
    if id not in data:
        entries.append((name, id))
        data[id] = []

    names = data[id]

    if name not in names:
        unknown.append(name)
        names.append(name)

    names.sort()

with open(OUTPUT, "w") as file:
    json.dump(data, file, indent=4, sort_keys=True)

with open(UNKNOWN, "w") as file:
    json.dump(unknown, file)

with open(ENTRIES, "w") as file:
    json.dump(entries, file)
