# Copyright (c) 2015-2022, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSCIRCUITEXPLORER_SOURCES
    CircuitExplorerPlugin.cpp
    api/ColorIDParser.cpp
    api/ColorUtils.cpp
    components/CircuitColorComponent.cpp
    io/BBPLoader.cpp
    io/NeuronMorphologyLoader.cpp
    io/SonataNGVLoader.cpp
    io/bbploader/CellLoader.cpp
    io/bbploader/GIDLoader.cpp
    io/bbploader/ParameterCheck.cpp
    io/bbploader/SimulationLoader.cpp
    io/bbploader/SynapseLoader.cpp
    io/bbploader/colordata/BBPNeuronColorData.cpp
    io/bbploader/colordata/BBPSynapseColorData.cpp
    io/bbploader/simulation/CompartmentReportComponent.cpp
    io/bbploader/simulation/SpikeReportComponent.cpp
    io/circuit/MorphologyCircuitLoader.cpp
    io/circuit/SomaCircuitLoader.cpp
    io/circuit/colorhandlers/MorphologyColorHandler.cpp
    io/circuit/colorhandlers/SomaColorHandler.cpp
    io/circuit/components/MorphologyCircuitComponent.cpp
    io/circuit/components/SomaCircuitComponent.cpp
    io/morphology/neuron/NeuronGeometryBuilder.cpp
    io/morphology/neuron/NeuronMorphology.cpp
    io/morphology/neuron/NeuronMorphologyProcessor.cpp
    io/morphology/vasculature/VasculatureColorHandler.cpp
    io/morphology/vasculature/VasculatureComponent.cpp
    io/simulation/SimulationFrameIndexer.cpp
    io/simulation/SimulationFrameTime.cpp
    io/simulation/SimulationMapping.cpp
    io/simulation/SimulationTransferFunction.cpp
    io/synapse/colorhandlers/EndfeetColorHandler.cpp
    io/synapse/colorhandlers/SynapseColorHandler.cpp
    io/synapse/components/EndfeetComponent.cpp
    io/synapse/components/SynapseComponent.cpp
    io/util/ProgressUpdater.cpp
    movie/MovieMaker.cpp
    network/entrypoints/ColorCircuitEntrypoint.cpp
    network/entrypoints/MakeMovieEntrypoint.cpp
    network/entrypoints/TraceAnterogradeEntrypoint.cpp
)

set(BRAYNSCIRCUITEXPLORER_PUBLIC_HEADERS
    CircuitExplorerPlugin.h
    api/ColorIDParser.h
    api/ColorUtils.h
    api/IColorData.h
    api/IColorHandler.h
    components/CircuitColorComponent.h
    io/BBPLoader.h
    io/BBPLoaderParameters.h
    io/NeuronMorphologyLoader.h
    io/NeuronMorphologyLoaderParameters.h
    io/SonataNGVLoader.h
    io/SonataNGVLoaderParameters.h
    io/bbploader/CellLoader.h
    io/bbploader/GIDLoader.h
    io/bbploader/LoadContext.h
    io/bbploader/ParameterCheck.h
    io/bbploader/SimulationLoader.h
    io/bbploader/SynapseLoader.h
    io/bbploader/colordata/BBPNeuronColorData.h
    io/bbploader/colordata/BBPSynapseColorData.h
    io/bbploader/simulation/CompartmentReportComponent.h
    io/bbploader/simulation/SimulationType.h
    io/bbploader/simulation/SpikeReportComponent.h
    io/circuit/MorphologyCircuitLoader.h
    io/circuit/SomaCircuitLoader.h
    io/circuit/colorhandlers/MorphologyColorHandler.h
    io/circuit/colorhandlers/SomaColorHandler.h
    io/circuit/components/MorphologyCircuitComponent.h
    io/circuit/components/SomaCircuitComponent.h
    io/morphology/neuron/NeuronColorMethods.h
    io/morphology/neuron/NeuronGeometry.h
    io/morphology/neuron/NeuronGeometryBuilder.h
    io/morphology/neuron/NeuronMorphology.h
    io/morphology/neuron/NeuronMorphologyProcessor.h
    io/morphology/neuron/NeuronSection.h
    io/morphology/vasculature/VasculatureColorHandler.h
    io/morphology/vasculature/VasculatureComponent.h
    io/morphology/vasculature/VasculatureColorMethods.h
    io/morphology/vasculature/VasculatureSection.h
    io/simulation/SimulationFrameIndexer.h
    io/simulation/SimulationFrameTime.h
    io/simulation/SimulationMapping.h
    io/simulation/SimulationTransferFunction.h
    io/synapse/colorhandlers/EndfeetColorHandler.h
    io/synapse/colorhandlers/SynapseColorHandler.h
    io/synapse/components/EndfeetComponent.h
    io/synapse/components/SynapseComponent.h
    io/util/ProgressUpdater.h
    movie/MovieMaker.h
    network/adapters/ColoringInfoAdapter.h
    network/entrypoints/ColorCircuitEntrypoint.h
    network/entrypoints/MakeMovieEntrypoint.h
    network/entrypoints/TraceAnterogradeEntrypoint.h
    network/messages/AvailableColoringInfoMessage.h
    network/messages/ColorCircuitMessage.h
    network/messages/MakeMovieMessage.h
    network/messages/TraceAnterogradeMessage.h
)
# Movie generation with FFmpeg
find_library(AVFORMAT_LIBRARY avformat)
find_library(AVCODEC_LIBRARY avcodec)
find_library(AVUTIL_LIBRARY avutil)
find_library(SWSCALE_LIBRARY swscale)

set(FFMPEG_FOUND ON)

if(NOT AVFORMAT_LIBRARY)
    message(STATUS "libavformat not found")
    set(FFMPEG_FOUND OFF)
endif()

if(NOT AVCODEC_LIBRARY)
    message(STATUS "libavcodec not found")
    set(FFMPEG_FOUND OFF)
endif()

if(NOT AVUTIL_LIBRARY)
    message(STATUS "libavutil not found")
    set(FFMPEG_FOUND OFF)
endif()

if(NOT SWSCALE_LIBRARY)
    message(STATUS "libswscale not found")
    set(FFMPEG_FOUND OFF)
endif()

add_library(braynsCircuitExplorer SHARED ${BRAYNSCIRCUITEXPLORER_SOURCES} ${BRAYNSCIRCUITEXPLORER_PUBLIC_HEADERS})

set(CIRCUITEXPLORER_INCLUDEDIR_LIST ${CMAKE_CURRENT_LIST_DIR})

target_include_directories(braynsCircuitExplorer PUBLIC
  "$<BUILD_INTERFACE:${CIRCUITEXPLORER_INCLUDEDIR_LIST}>"
  "$<INSTALL_INTERFACE:include>")

target_link_libraries(braynsCircuitExplorer PRIVATE
    braynsCommon braynsPluginAPI braynsNetwork
    sonata::sonata_shared MorphIO::morphio Brion Brain ${HDF5_LIBRARIES}
    HighFive MVDTool
)

if(FFMPEG_FOUND)
    target_compile_definitions(braynsCircuitExplorer PRIVATE BRAYNS_USE_FFMPEG)
    target_link_libraries(braynsCircuitExplorer PRIVATE
        ${AVFORMAT_LIBRARY}
        ${AVCODEC_LIBRARY}
        ${AVUTIL_LIBRARY}
        ${SWSCALE_LIBRARY})
endif()

set_target_properties(braynsCircuitExplorer
    PROPERTIES
        VERSION ${BRAYNS_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

target_compile_options(braynsCircuitExplorer PRIVATE ${BRAYNS_COMPILE_OPTIONS})
# For Brion uint128_t
target_compile_options(braynsCircuitExplorer PRIVATE -Wno-deprecated-copy)

install(TARGETS braynsCircuitExplorer
    EXPORT braynsCircuitExplorer-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h")

install(EXPORT braynsCircuitExplorer-targets
    DESTINATION share/braynsCircuitExplorer/CMake
)

