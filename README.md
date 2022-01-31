# Brayns

[![DOI](https://zenodo.org/badge/61363694.svg)](https://zenodo.org/badge/latestdoi/61363694)

Brayns is a large-scale scientific visualization platform. It is based on Intel OSPRAY to perform CPU Ray-tracing, which allows it to take full advantage of the underlying hardware where it runs.

It is based on a extension-plugin architecture. The core provide basic functionalities that can be reused and/or extended on plugins, which are independent and can be loaded or disabled at start-up. This simplifies the process of adding support for new scientific visualization use cased, without compromising the realiability of the rest of the software.

Brayns counts with two main applications for its usage:

 * **braynsViewer**: An interactive desktop application to perform local rendering.
 * **braynsService**: A rendering backed which can be accessed over the internet and streams images to the connected clients.

Brayns also comes with some already-made plugins:

* **CircuitExplorer**: The main plugin used at BBP. Allows for neuronal circuit visualization and simulation rendering.
* **CircuitInfo (Deprecated)**: A plugin which accept queries to extract information from neuronal circuits.
* **DTI**: A diffuse-tensor imaging visualization plugin.

## Building

Brayns is developed, maintained and run on Linux-based operating systems, being tested mainly on RHEL and Ubuntu. The following platforms and build environments have been tested:

* Linux: Ubuntu 16.04, Ubuntu 18.04, Ubuntu 20.04, Debian 9, RHEL 7 (Makefile, x64)

### System dependencies

The following components must be installed on the system where Brayns will be built:

* GCC 9 or higher (Requires C++ 17 support)
* CMake 3.15 or higher
* Make or Ninja build systems
* Git
* Intel TBB development files
* Package config
* SSL Development files
* ISPC compiler 1.10.0b (https://github.com/ispc/ispc/releases/tag/v1.10.0)
* Embree 3.5.2 (https://github.com/embree/embree/tree/v3.5.2)
* OSPRay 1.8.5 (https://github.com/ospray/OSPRay/tree/v1.8.5)

To build braynsViewer, additionally, the following components must be installed:

* OpenGL 3.3 or higher
* GLEW

Optionally, to build the core plugins of Brayns, the following components are required.

* HDF5 development files

Brayns uses further dependencies, but if the are not present on the system, it will download them by itself during build. If they are present on the system, the build and compiling process will be speed up:

* GLFW 3.3.5 (https://github.com/glfw/glfw/tree/3.3.5)
* ImGui (https://github.com/ocornut/imgui)
* glm 0.9.9.8 (https://github.com/g-truc/glm/tree/0.9.9.8)
* Poco libraries 1.11.1 (https://github.com/pocoproject/poco/tree/poco-1.11.1-release)
* spdlog 1.9.2 (https://github.com/gabime/spdlog/tree/v1.9.2)
* stb (https://github.com/nothings/stb)
* libsonata 0.1.9 (https://github.com/BlueBrain/libsonata/tree/v0.1.9)
* MVDTool 2.4.2 (https://github.com/BlueBrain/MVDTool/tree/v2.4.2)
* MorphIO 3.3.2 (https://github.com/BlueBrain/MorphIO/tree/v3.3.2)
* Brion 3.3.7 (https://github.com/BlueBrain/Brion/tree/3.3.7)

### Build command

Once the given dependencies are installed, Brayns can be cloned and built as follows:

    $ git clone https://github.com/BlueBrain/Brayns.git
    $ cd Brayns && mkdir build && cd build
    $ cmake .. -DCMAKE_BUILD_TYPE=Release \
      -Dospray_DIR=/path/to/OSPRay/cmake/config/folder \
      -Dembree_DIR=/path/to/Embree/cmake/config/folder \
      -DISPC_EXECUTABLE=/path/to/ispc/compiler/binary/file
    $ make -j

This will build the core of Brayns, the braynsService application, the CircuitExplorer plugin and the unit tests.

The following cmake options (shown with their default value) can be used during CMake build run to customize the components to build as *-DVARIABLE=ON|OFF* :

* **BRAYNS_TESTS_ENABLED** (Default ON) - Activate unit tests
* **BRAYNS_VIEWER_ENABLED** (Default OFF) - Activate braynsViewer app
* **BRAYNS_SERVICE_ENABLED** (Default ON) - Activate braynsService app
* **BRAYNS_CIRCUITEXPLORER_ENABLED** (Default ON) - Activate CircuitExplorer plugin
* **BRAYNS_CIRCUITINFO_ENABLED** (Default OFF) - Activate CircuitInfo plugin
* **BRAYNS_DTI_ENABLED** (Default OFF) - Activate Diffusion-Tensor Imaging plugin
* **BRAYNS_HARDWARE_RANDOMIZER_ENABLED** (Default OFF) - Activates hardware randomizer for raytracing


## Running

**Important:** All the libraries on which Brayns depends must be reachable through the **LD_LIBRARY_PATH** environmental variable, including plugin libraries.

### braynsService application

To run the braynsService app, execute the following command (The command assumes braynsService executable is available on the system **PATH**):

    $ braynsService --uri 0.0.0.0:5000

The ***--uri*** parameter allows to specify an address and a port to bind to. In the example, the service is binding to all available addresses and the port 5000.

This command will launch the braynsService app with only core functionality. To also add the functionality of any plugin, the ***--plugin*** option can be used to load plugins:

    $ braynsService --uri 0.0.0.0:5000 --plugin braynsCircuitExplorer --plugin braynsCircuitInfo

The name that must be used when specifying a plugin will depend on the name of the library of the plugin (stripping the extension **.so** from it)

### braynsViewer application

The braynsViewer application is executed in a similar fashin as the braynsService app:

    $ braynsViewer --plugin braynsCircuitExplorer --plugin braynsCircuitInfo

Although not required, it also allows to specify the ***--uri*** parameter, although it is only used to connect to the renderer through the Python client.

### Using the Docker image

Brayns is available as a docker image at https://hub.docker.com/r/bluebrain/brayns. The image allows to launch the braynsService application. 

It is built with every commit merged into the main repository branch (develoop), and deployed into docker hub as brayns:latest. Furthermore, when a new release is made, and a new tag created, an additional image is built and deployed with the same tag.

To get Brayns docker image, you will need to have docker installed. Then execute the following command to download it:

    $ docker pull bluebrain/brayns:latest

To run it, simply execute the following command:

    $ docker run -ti --rm -p 5000:5000 bluebrain/brayns --uri 0.0.0.0:5000

Additional parameters, such as ***--plugin***, can be specified in a similar fashion as in the **braynsService** application.

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

Copyright (c) 2008-2021 Blue Brain Project/EPFL

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License version 3 as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
