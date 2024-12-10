import csv
import json
import sys
from typing import Any

import requests

JSON = "repos.json"
CSV = "repos.csv"
TOKEN = sys.argv[1]
URL = "https://api.github.com/orgs/BlueBrain/repos"
PARAMS = [("type", "all"), ("sort", "full_name"), ("per_page", 100)]


def flatten(
    values: dict[str, Any],
    parent_key: str = "",
    separator: str = ".",
) -> dict[str, Any]:
    result = dict[str, Any]()
    for key, value in values.items():
        new_key = f"{parent_key}{separator}{key}" if parent_key else key
        if not isinstance(value, dict):
            result[new_key] = value
            continue
        items = flatten(value, new_key, separator)
        result.update(items)
    return result


repos = list[dict[str, Any]]()

i = 1

while True:

    params = PARAMS + [("page", str(i))]
    reply = requests.get(URL, params=params, auth=("Bearer", TOKEN))

    page = reply.json()

    if not page:
        break

    repos += page

    i += 1

with open(JSON, "wt", encoding="utf-8") as file:
    json.dump(repos, file, indent=4)

flat = [flatten(repo) for repo in repos]

headers = sorted(set(key for item in flat for key in item.keys()))

with open(CSV, "wt", encoding="utf-8") as file:
    writer = csv.DictWriter(file, headers)
    writer.writeheader()
    writer.writerows(flat)
