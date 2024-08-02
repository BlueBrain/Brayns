# Brayns Python API

> A client for [Brayns](../README.md) based on a websocket client to provide
remote control on a running braynsService instance.

# Table of Contents

* [Installation](#installation)
* [Usage](#usage)
    * [Connection](#connection)
    * [Raw requests](#rawrequests)
    * [API](#api)
* [Documentation](#documentation)

## Installation
---------------

You can install this package from [PyPI](https://pypi.org/):

```bash
pip install brayns
```

The package can also be installed from source:

```bash
git clone https://github.com/BlueBrain/Brayns.git
cd Brayns/python
pip install .
```

To install packages that are only required for development:

```bash
pip install -r requirements-dev.txt
```

## Usage
--------

TODO

## Tests

To run the tests, use

```bash
pytest tests
```

## Lint

To format and lint use

```bash
ruff format brayns
ruff check brayns
mypy brayns
```

## VSCode integration

From the current directory (Brayns/python):

1. Create a venv

```bash
python3.11 -m venv venv
source venv/bin/activate
```

2. Install requirements for development:

```bash
pip install -r requirements.txt
pip install -r requirements-dev.txt
```

3. For integration testing, create a `.env` file:

```bash
BRAYNS_HOST=localhost
BRAYNS_PORT=5000
BRAYNS_SSL=0
```

Note: integration testing can be disable using the pytest --without-integration flag.

4. Create a .vscode folder and create a `launch.json` inside to use to debug tests:

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Python: Debug Tests",
            "type": "debugpy",
            "request": "launch",
            "program": "${file}",
            "purpose": [
                "debug-test"
            ],
            "console": "integratedTerminal",
            "justMyCode": false
        }
    ]
}
```

5. In the same folder, create a `settings.json` to configure pytest:

```json
{
    "python.analysis.typeCheckingMode": "basic",
    "python.testing.pytestArgs": [
        "tests"
    ],
    "python.envFile": "${workspaceFolder}/.env",
    "python.testing.unittestEnabled": false,
    "python.testing.pytestEnabled": true
}
```

## Documentation
-----------------

A more complete documentation is available
[here](https://brayns.readthedocs.io/en/latest/).
