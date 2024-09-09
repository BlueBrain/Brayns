![Banner](/doc/banner.jpg?raw=true "Brayns banner")

[![DOI](https://zenodo.org/badge/61363694.svg)](https://zenodo.org/badge/latestdoi/61363694)

Brayns is a large-scale scientific visualization platform. It is based on Intel OSPRAY to perform CPU Ray-tracing, which allows it to take full advantage of the underlying hardware where it runs.

Brayns comes with a main application:

 * **braynsService**: A rendering backend which can be accessed over the internet and streams images to the connected clients.

## Building

TODO

### System dependencies

TODO

### Build command

Once the given dependencies are installed, Brayns can be cloned and built as follows:

    $ git clone https://github.com/BlueBrain/Brayns.git
    $ cd Brayns && mkdir build && cd build
    $ cmake .. -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_PREFIX_PATH=/path/to/OSPRay/cmake/config/folder
    $ make -j

This will build the core of Brayns, the braynsService application, the circuit and atlas libraries and the unit tests.

The following cmake options (shown with their default value) can be used during CMake build run to customize the components to build as *-DVARIABLE=ON|OFF* :

* **BRAYNS_ENABLE_TESTS** (Default ON) - Activate unit tests.
* **BRAYNS_ENABLE_SERVICE** (Default ON) - Activate braynsService app.
* **BRAYNS_ENABLE_CIRCUITS** (Default ON) - Activate circuit support.
* **BRAYNS_ENABLE_ATLAS** - (Default ON) Activate atlas support.

### Run linters

Run clang format as follows:

    $ SOURCES=$(find apps src tests \( -name "*.h" -or -name "*.cpp" \))
    $ clang-format-20 --Werror $SOURCES

Run clang tidy as follows:

Use cmake config to generate build commands:

"cacheVariables": {
    "CMAKE_EXPORT_COMPILE_COMMANDS": true,
    "CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES": "/usr/include/c++/11;/usr/include/x86_64-linux-gnu/c++/11"
}

Run with

    $ run-clang-tidy-20 -p build/user-debug -quiet -line-filter='["name": "CHECK_"]'


## Running

**Important:** All the dynamic libraries on which Brayns depends must be reachable through the **LD_LIBRARY_PATH** environmental variable.

### braynsService application

To run the braynsService app, execute the following command (The command assumes braynsService executable is available on the system **PATH**):

    $ braynsService --host 0.0.0.0 --port 5000

The ***--uri*** parameter allows to specify an address and a port to bind to. In the example, the service is binding to all available addresses and the port 5000.

Use the following command to get more details about command line arguments.

    $ braynsService --help

### Using the Docker image

Brayns is available as a docker image at https://hub.docker.com/r/bluebrain/brayns. The image allows to launch the braynsService application.

It is built with every commit merged into the main repository branch (master), and deployed into docker hub as brayns:latest. Furthermore, when a new release is made, and a new tag created, an additional image is built and deployed with the same tag.

To get Brayns docker image, you will need to have docker installed. Then execute the following command to download it:

    $ docker pull bluebrain/brayns:latest

To run it, simply execute the following command:

    $ docker run -ti --rm -p 5000:5000 bluebrain/brayns --host 0.0.0.0 --port 5000

Additional parameters, can be specified in a similar fashion as in the **braynsService** application.

## Python and JSON-RPC API

A running instance of braynsService can be monitored using ***brayns*** Python package or a websocket client and the JSON-RPC API.

For more details about the client APIs, see the [python client doc](python/README.md).

## Known Bugs

Please file a [Bug Report](https://github.com/BlueBrain/Brayns/issues) if you
find new issues which have not been reported already. If you find an
already reported problem, please update the corresponding issue with your inputs
and outputs.

## Funding & Acknowledgment

The development of this software was supported by funding to the Blue Brain Project,
a research center of the École polytechnique fédérale de Lausanne (EPFL), from the
Swiss government’s ETH Board of the Swiss Federal Institutes of Technology.

This project has received funding from the European Union’s FP7-ICT programme
under Grant Agreement No. 604102 (Human Brain Project RUP).

This project has received funding from the European Union's Horizon 2020 Framework
Programme for Research and Innovation under the Specific Grant Agreement No. 720270
(Human Brain Project SGA1).


## License

Brayns is licensed under the LGPL, unless noted otherwise, e.g., for external dependencies. See file LICENSE.txt for the full license.

Copyright (c) 2008-2023 Blue Brain Project/EPFL

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License version 3 as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
