# Brayns Python Client

> A client for [Brayns](../README.md) based on the [Rockets client](https://github.com/BlueBrain/Rockets/python/README.md) to provide remote control of a running Brayns instance.

[![Travis CI](https://img.shields.io/travis/BlueBrain/Brayns/master.svg?style=flat-square)](https://travis-ci.org/BlueBrain/Brayns)


# Table of Contents

* [Installation](#installation)
* [Usage](#usage)
    * [Connection](#connection)
    * [Properties](#properties)
    * [Methods](#methods)
    * [Snapshot](#snapshot)
    * [Live rendering](#live)


### Installation
----------------
You can install this package from [PyPI](https://pypi.org/):
```bash
pip install brayns
```

### Usage
---------

#### `Client` vs. `AsyncClient`
Brayns provides to types of clients to support asychronous and synchronous usage.

The `AsyncClient` exposes all of its functionality as `async` functions, hence an `asyncio`
[event loop](https://docs.python.org/3/library/asyncio-eventloop.html) is needed to complete pending
execution via `await` or `run_until_complete()`. The added benefit is to watch progress of pending
tasks or cancel their execution. This is provided by according widgets if the Brayns client is used
from within a Jupyter notebook.

For simplicity, a synchronous `Client` is provided which automagically executes in a synchronous,
blocking fashion.

#### Connection
Create a client:
```py
from brayns import Client

client = Client('localhost:8200')
print(brayns)
`Brayns version 0.8.0.c52dd4b running on http://localhost:8200/`
```

Create an asynchronous client:
```py
from brayns import AsyncClient

client = await AsyncClient('localhost:8200')
print(brayns)
`Brayns version 0.8.0.c52dd4b running on http://localhost:8200/`
```


#### Properties
Exposed properties from Brayns are properties on the python side as well:
```py
from brayns import Client

client = Client('myhost:8080')

print(client.camera)
```

Changes on the properties must be communicated with `commit()` to Brayns:
```py
from brayns import Client

client = Client('myhost:8080')

client.camera.origin = [1,2,3]
client.camera.commit()
```


#### Methods
Calling an RPC on Brayns is as simple as calling a method on the client object:
```py
from brayns import Client

client = Client('myhost:8080')

client.set_camera(current='orthographic')
```

Above method does only return something on error. There are also requests:
```py
from brayns import Client

client = Client('myhost:8080')

clip_planes = client.get_clip_planes()
```


#### Snapshot
Make a snapshot and return a PIL image:
```py
from brayns import Client

client = Client('myhost:8080')

img = client.image(format='png', size=(1920, 1080), samples_per_pixel=64)
```

#### Live rendering
If you are using the client in a Jupyter notebook, you can watch the live rendering of Brayns in a widget:
```py
from brayns import Client

client = Client('myhost:8080')

client.show()
```
