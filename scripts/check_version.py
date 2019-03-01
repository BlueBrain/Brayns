#!/usr/bin/env python3
import configparser
import json
import urllib.request

config = configparser.ConfigParser()
config.read("setup.cfg")
to_check = config["metadata"]["version"]
url = "https://pypi.python.org/pypi/brayns/json"
with urllib.request.urlopen(url) as response:
    data = json.loads(response.read().decode("utf-8"))
    versions = data["releases"].keys()
    if to_check not in versions:
        print(to_check)
