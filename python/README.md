# Brayns Python Client

> A client for [Brayns](../README.md) based on the a websocket client to provide remote control of a running Brayns instance.

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

#### Connection
Create a client:
```py
import brayns

client = brayns.Client('myhost:5000')
```

#### Methods
Calling an RPC on Brayns is as simple as calling a method on the client object:
```py
import brayns

client = brayns.Client('myhost:5000')

client.set_camera(current='orthographic')
```

Methods throw a brayns.ReplyError on error.

#### Snapshot
Make a snapshot and return a base64 encoded image:
```py
import brayns
import base64

client = brayns.Client('myhost:5000')

result = client.snapshot(
    format='png',
    size=(1920, 1080),
    samples_per_pixel=64
)

base64_data = result['data']

data = base64.b64decode(base64_data)

with open('myImage.png', 'wb') as image:
    image.write(data)

```
