import csv
import json

DATA = "data.csv"
REF = "ref.json"
FOUND = "found.json"
NOT_FOUND = "not_found.json"
OUTPUT = "output.csv"
NAME = 2

with open(DATA, encoding="latin-1") as file:
    reader = csv.reader(file, delimiter=";")
    data = list(reader)[1:]

with open(REF) as file:
    ref: dict[str, list[str]] = json.load(file)

found = set[tuple[str, str]]()
not_found = set[str]()

mapping = {name: id for id, names in ref.items() for name in names}

for row in data:
    name = row[NAME]

    if name.isdigit():
        continue

    id = mapping.get(name)

    if id is None:
        not_found.add(name)
        continue

    if not id.isdigit():
        continue

    row[NAME] = id
    found.add((id, name))

with open(FOUND, "w") as file:
    json.dump(sorted(found), file)

with open(NOT_FOUND, "w") as file:
    json.dump(sorted(not_found), file)

with open(OUTPUT, "w") as file:
    writer = csv.writer(file)
    writer.writerows(data)
