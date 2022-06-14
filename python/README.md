# Brayns Python Client

> A client for [Brayns](../README.md) based on a websocket client to provide remote control of a running Brayns instance.

# Table of Contents

* [Installation](#installation)
* [Usage](#usage)
    * [Connection](#connection)
    * [Raw requests](#rawrequests)
    * [API](#api)
* [Documentation](#documentation)

### Installation
----------------

You can install this package from [PyPI](https://pypi.org/):

```bash
pip install brayns
```

Or from source:

```bash
git clone https://github.com/BlueBrain/Brayns.git
cd Brayns/python
python setup.py sdist
pip install dist/brayns-<version>.tar.gz # Replace with current version
```

### Usage
---------

#### Connection

Connect to a renderer backend instance:

```py
import brayns

with brayns.connect('localhost:5000') as instance: # Renderer host and port
    instance.request('registry') # Some requests
```

#### Raw requests

Raw JSON-RPC requests can be sent using the instance:

```py
result = instance.request('schema', {'endpoint': 'get-version'})
```

It will throw a brayns.RequestError if an error occurs.

#### API

As raw requests can be tedious, a higher level API is also provided.

```py
loader = brayns.BbpLoader(
    cells=brayns.BbpCells.all(),
    radius_multiplier=10
)

models = loader.loader(instance, 'path/to/BlueConfig')

model = models[0]

camera = brayns.PerspectiveCamera()

view = camera.get_full_screen_view(model.bounds)

renderer = brayns.InteractiveRenderer()

snapshot = brayns.Snapshot(
    resolution=brayns.Resolution.full_hd,
    camera=camera,
    view=view,
    renderer=renderer
)

snapshot.save(instance, 'path/to/snapshot.png')
```

### Documentation
-----------------

The JSON-RPC and Python APIs are documented [here](https://brayns.readthedocs.io/en/latest/).
