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

Or from source:

```bash
git clone https://github.com/BlueBrain/Brayns.git
cd Brayns/python
python setup.py sdist
pip install dist/* # Output in dist folder
```

## Usage
--------

### Connection

Connect to a renderer backend instance:

```py
import brayns

connector = brayns.Connector('localhost:5000')

with connector.connect() as instance:
    print(brayns.get_version(instance))
```

### Raw requests

Raw JSON-RPC requests can be sent using the instance:

```py
result = instance.request('schema', {'endpoint': 'get-version'})
```

It will throw a brayns.RequestError if an error occurs.

### API

As raw requests can be tedious, a higher level API is also provided.

```py
models = brayns.get_models(instance)
```

## Documentation
-----------------

A more complete documentation is available
[here](https://brayns.readthedocs.io/en/latest/).
