# Brayns

[![C++ Build Status](https://bbpcode.epfl.ch/ci/buildStatus/icon?job=oss.Brayns)](https://bbpcode.epfl.ch/ci/job/oss.Brayns/)
[![Docker Build Status](https://bbpcode.epfl.ch/ci/buildStatus/icon?job=oss.Brayns.docker)](https://bbpcode.epfl.ch/ci/job/oss.Brayns.docker/)
[![Python Build Status](https://travis-ci.org/BlueBrain/Brayns.svg)](https://travis-ci.org/BlueBrain/Brayns)
[![Read the Docs](https://readthedocs.org/projects/brayns/badge/?version=latest)](http://brayns.readthedocs.io/en/latest/?badge=latest)
[![Docker Pulls](https://img.shields.io/docker/pulls/bluebrain/brayns.svg)](https://hub.docker.com/r/bluebrain/brayns/)
[![GitHub release](https://img.shields.io/github/release/BlueBrain/Brayns.svg)](https://github.com/BlueBrain/Brayns/releases)

![Brayns](doc/images/Brayns.jpg)

One of the keys towards understanding how the brain works as a whole is
visualisation of how the individual cells function. In particular, the more
morphologically accurate the visualisation can be, the easier it is for experts
in the biological field to validate cell structures; photo-realistic rendering
is therefore important.

The Blue Brain Project has made major efforts to create morphologically accurate
neurons to simulate sub-cellular and electrical activities, e.g. molecular
simulations of neuron biochemistry or multi-scale simulations of neuronal
function. Ray-tracing can help to highlight areas of the circuits where cells
touch each other and where synapses are being created. In combination with
‘global illumination’, which uses light, shadow, and depth of field effects to
simulate photo-realistic images, this technique makes it easier to visualise how
the neurons function.

Brayns is a minimalistic visualiser that can perform ray-traced rendering of
neurons. It provides an abstraction of the underlying rendering engines, so that
the best possible acceleration libraries can be used for the relevant hardware.
(https://github.com/BlueBrain/Brayns.git).

To keep track of the changes between releases check the [changelog](Changelog.md).

Contact: bbp-open-source@googlegroups.com

## Architecture

![Architecture](doc/images/Architecture.png)

## User guide

Command line arguments are documented in the [User Guide](doc/UserGuide.md).

## About

The following platforms and build environments are tested:

* Linux: Ubuntu 16.04, Debian 9, RHEL 7 (Makefile, x64)

## Using Docker images

### Start Brayns as a service

```
docker run -ti --rm -p 8200:8200 bluebrain/brayns
```

More documentation is available on DockerHub: https://hub.docker.com/r/bluebrain/brayns/

### Start the Brayns web UI

```
docker run -ti --rm -p 8080:8080 bluebrain/brayns-ui
```

More documentation is available on DockerHub: https://hub.docker.com/r/bluebrain/brayns-ui/

## Building from Source

### Ubuntu 18.04

To install all dependencies needed by Brayns on Ubuntu 18.04 run:

```
sudo apt install git cmake g++ libtbb-dev libgl1-mesa-dev libxrandr-dev \
libxinerama-dev libxcursor-dev libboost-all-dev libfreeimage-dev libglew-dev \
libwebsockets-dev libturbojpeg libturbojpeg0-dev libassimp-dev libhdf5-dev
```

### Prerequisites

In order to ease the application compilation process, we recommend using the
following tree structure:

```
- src
  +- ispc-v1.10.0-linux
  +- OSPRay
  +- Brayns
```

#### Intel ISPC compiler

Download and extract [ISPC compiler 1.10.0 archive](https://ispc.github.io/downloads.html).

#### Embree

Clone embree in the same folder level as ISPC compiler

```
  git clone https://github.com/embree/embree.git
  mkdir embree/Build
  cd embree/Build
  git checkout v3.5.2
  cmake .. -DCMAKE_INSTALL_PREFIX=<Brayns_installation_folder>
  make install
```

#### OSPRay

Clone OSPRay in the same folder level as ISPC compiler

```
  git clone https://github.com/ospray/OSPRay.git
  mkdir OSPRay/Build
  cd OSPRay/Build
  git checkout v1.8.5
  export CMAKE_PREFIX_PATH=<Brayns_installation_folder>
  cmake .. -DCMAKE_INSTALL_PREFIX=<Brayns_installation_folder>
  make install
```

### Brayns

```
  git clone --recursive https://github.com/BlueBrain/Brayns.git
  mkdir Brayns/Build
  cd Brayns/Build
  cmake .. -DCLONE_SUBPROJECTS=ON -DCMAKE_INSTALL_PREFIX=<Brayns_installation_folder>
  make install
```

A number of dependencies are optional, and are related to some specific Brayns
features:

#### Enable/Disable [OptiX](https://developer.nvidia.com/optix)
```
cmake .. -DOptiX_INSTALL_DIR=<OptiX_installation_folder> -DBRAYNS_OPTIX_ENABLED=On
```

#### Enable/Disable [assimp](https://github.com/assimp/assimp) supported mesh file loader (.obj, .ply, etc.)
```
cmake .. -DBRAYNS_ASSIMP_ENABLED=ON:OFF
```

#### Enable/Disable [Brion](https://github.com/BlueBrain/Brion) supported morphology file loader (.h5, .swc, BlueConfig, CircuitConfig)
```
cmake .. -DBRAYNS_CIRCUITVIEWER_ENABLED=ON:OFF
```

#### Enable/Disable [Deflect](https://github.com/BlueBrain/Deflect) for streaming to [Tide](https://github.com/BlueBrain/Tide), the Tiled Interactive DisplayWall environment.
```
cmake .. -DBRAYNS_DEFLECT_ENABLED=ON:OFF
```

#### Enable/Disable HTTP/REST/Websockets interface.
 [LibJpegTurbo](http://libjpeg-turbo.virtualgl.org) and
 [Rockets](https://github.com/BlueBrain/Rockets).
 See documentation for REST API details.
```
cmake .. -DBRAYNS_NETWORKING_ENABLED=ON:OFF
```

#### Enable/Disable [VRPN](https://github.com/vrpn/vrpn) tracking plugin.
```
cmake .. -DBRAYNS_VRPN_ENABLED=ON:OFF
```

#### Enable/Disable optional OSPRay modules.
```
cmake .. -DBRAYNS_OPTIX_ENABLED=ON:OFF
cmake .. -DBRAYNS_OPENDECK_ENABLED=ON:OFF
```

## Running Brayns viewer

```
export PATH=<Brayns_installation_folder>/bin:$PATH
export LD_LIBRARY_PATH=<Brayns_installation_folder>/lib:$LD_LIBRARY_PATH
braynsViewer
```

## Running Brayns off-screen service

```
export PATH=<Brayns_installation_folder>/bin:$PATH
export LD_LIBRARY_PATH=<Brayns_installation_folder>/lib:$LD_LIBRARY_PATH
braynsService
```

## Known Bugs

Please file a [Bug Report](https://github.com/BlueBrain/Brayns/issues) if you
find new issues which have not already been reported in
[Bug Report](https://github.com/BlueBrain/Brayns/issues) page. If you find an
already reported problem, please update the corresponding issue with your inputs
and outputs.
