# Copyright (c) 2015-2022, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSCIRCUITEXPLORER_SOURCES
    CircuitExplorerPlugin.cpp
    api/CircuitColorHandler.cpp
    api/CircuitColorManager.cpp
    api/ColorUtils.cpp
    api/VasculatureRadiiSimulation.cpp
    components/CircuitComponent.cpp
    components/EndfeetComponent.cpp
    components/SynapsesComponent.cpp
    components/VasculatureComponent.cpp
    components/circuits/PrimitiveMorphologyCircuitComponent.cpp
    components/circuits/SomaCircuitComponent.cpp
    io/BBPLoader.cpp
    io/NeuronMorphologyLoader.cpp
    io/SonataLoader.cpp
    io/SonataNGVLoader.cpp
    io/bbploader/CellLoader.cpp
    io/bbploader/ParameterCheck.cpp
    io/bbploader/SynapseLoader.cpp
    io/bbploader/colorhandlers/BBPNeuronColorHandler.cpp
    io/bbploader/colorhandlers/BBPSynapseColorHandler.cpp
    io/bbploader/simulation/CompartmentSimulation.cpp
    io/bbploader/simulation/SpikeSimulation.cpp
    io/bbploader/simulation/handlers/CompartmentHandler.cpp
    io/bbploader/simulation/handlers/SpikeHandler.cpp
    io/morphology/neuron/NeuronBuilder.cpp
    io/morphology/neuron/NeuronMorphology.cpp
    io/morphology/neuron/NeuronMorphologyPipeline.cpp
    io/morphology/neuron/builders/PrimitiveNeuronBuilder.cpp
    io/morphology/neuron/instances/PrimitiveNeuronInstance.cpp
    io/morphology/neuron/instances/SomaNeuronInstance.cpp
    io/morphology/neuron/pipeline/RadiusMultiplier.cpp
    io/morphology/neuron/pipeline/RadiusOverride.cpp
    io/morphology/neuron/pipeline/RadiusSmoother.cpp
    io/morphology/neuron/pipeline/SectionWelder.cpp
    io/morphology/vasculature/VasculatureInstance.cpp
    io/sonataloader/ParameterCheck.cpp
    io/sonataloader/colorhandlers/PopulationColorManager.cpp
    io/sonataloader/colorhandlers/edge/CommonEdgeColorHandler.cpp
    io/sonataloader/colorhandlers/edge/EndFootColorHandler.cpp
    io/sonataloader/colorhandlers/node/SonataNeuronColorHandler.cpp
    io/sonataloader/colorhandlers/node/VasculatureColorHandler.cpp
    io/sonataloader/data/SonataCells.cpp
    io/sonataloader/data/SonataConfig.cpp
    io/sonataloader/data/SonataEndFeetReader.cpp
    io/sonataloader/data/SonataSelection.cpp
    io/sonataloader/data/SonataSimulationMapping.cpp
    io/sonataloader/data/SonataSynapses.cpp
    io/sonataloader/data/SonataVasculature.cpp
    io/sonataloader/populations/PopulationLoadManager.cpp
    io/sonataloader/populations/edges/CommonEdgeLoader.cpp
    io/sonataloader/populations/edges/EndFootPopulationLoader.cpp
    io/sonataloader/populations/edges/SynapseAstrocytePopulationLoader.cpp
    io/sonataloader/populations/nodes/AstrocytePopulationLoader.cpp
    io/sonataloader/populations/nodes/BiophysicalPopulationLoader.cpp
    io/sonataloader/populations/nodes/CommonNodeLoader.cpp
    io/sonataloader/populations/nodes/PointNeuronPopulationLoader.cpp
    io/sonataloader/populations/nodes/VasculaturePopulationLoader.cpp
    io/sonataloader/reports/PopulationReportManager.cpp
    io/sonataloader/reports/handlers/SonataReportHandler.cpp
    io/sonataloader/reports/handlers/SonataSpikeHandler.cpp
    io/sonataloader/reports/handlers/VasculatureRadiiHandler.cpp
    io/sonataloader/reports/loaders/EdgeCompartmentLoader.cpp
    io/sonataloader/reports/loaders/NodeCompartmentLoader.cpp
    io/sonataloader/reports/loaders/NodeSpikeLoader.cpp
    io/sonataloader/reports/loaders/NodeVasculatureReportLoader.cpp
    io/synapse/groups/EndFootGroup.cpp
    io/synapse/groups/OldSurfaceSynapseGroup.cpp
    io/synapse/groups/SurfaceSynapseGroup.cpp
    io/synapse/groups/SynapseAstrocyteGroup.cpp
    io/util/TransferFunctionUtils.cpp
    movie/MovieMaker.cpp
    network/entrypoints/ColorCircuitEntrypoint.cpp
    network/entrypoints/MakeMovieEntrypoint.cpp
    network/entrypoints/MirrorModelEntrypoint.cpp
    network/entrypoints/SetCircuitThicknessEntrypoint.cpp
    network/entrypoints/SimulationColorEntrypoint.cpp
    network/entrypoints/TraceAnterogradeEntrypoint.cpp
)

set(BRAYNSCIRCUITEXPLORER_PUBLIC_HEADERS
    CircuitExplorerPlugin.h
    api/CircuitColorHandler.h
    api/CircuitColorManager.h
    api/ColorUtils.h
    api/VasculatureRadiiSimulation.h
    components/CircuitComponent.h
    components/EndfeetComponent.h
    components/SynapsesComponent.h
    components/VasculatureComponent.h
    components/circuits/PrimitiveMorphologyCircuitComponent.h
    components/circuits/SomaCircuitComponent.h
    io/BBPLoader.h
    io/BBPLoaderParameters.h
    io/NeuronMorphologyLoader.h
    io/NeuronMorphologyLoaderParameters.h
    io/SonataLoader.h
    io/SonataLoaderParameters.h
    io/SonataNGVLoader.h
    io/SonataNGVLoaderParameters.h
    io/bbploader/CellLoader.h
    io/bbploader/ParameterCheck.h
    io/bbploader/SynapseLoader.h
    io/bbploader/colorhandlers/BBPNeuronColorHandler.h
    io/bbploader/colorhandlers/BBPSynapseColorHandler.h
    io/bbploader/simulation/CompartmentSimulation.h
    io/bbploader/simulation/Simulation.h
    io/bbploader/simulation/SimulationType.h
    io/bbploader/simulation/SpikeSimulation.h
    io/bbploader/simulation/handlers/CompartmentHandler.h
    io/bbploader/simulation/handlers/SpikeHandler.h
    io/morphology/MorphologyInstance.h
    io/morphology/neuron/NeuronBuilder.h
    io/morphology/neuron/NeuronColorHandler.h
    io/morphology/neuron/NeuronGeometryType.h
    io/morphology/neuron/NeuronMaterialMap.h
    io/morphology/neuron/NeuronMorphology.h
    io/morphology/neuron/NeuronMorphologyPipeline.h
    io/morphology/neuron/NeuronSection.h
    io/morphology/neuron/builders/PrimitiveNeuronBuilder.h
    io/morphology/neuron/instances/PrimitiveNeuronInstance.h
    io/morphology/neuron/instances/SomaNeuronInstance.h
    io/morphology/neuron/pipeline/RadiusMultiplier.h
    io/morphology/neuron/pipeline/RadiusOverride.h
    io/morphology/neuron/pipeline/RadiusSmoother.h
    io/morphology/neuron/pipeline/SectionWelder.h
    io/morphology/vasculature/VasculatureInstance.h
    io/morphology/vasculature/VasculatureMaterialMap.h
    io/morphology/vasculature/VasculatureSection.h
    io/sonataloader/ParameterCheck.h
    io/sonataloader/colorhandlers/PopulationColorManager.h
    io/sonataloader/colorhandlers/edge/CommonEdgeColorHandler.h
    io/sonataloader/colorhandlers/edge/EndFootColorHandler.h
    io/sonataloader/colorhandlers/node/SonataNeuronColorHandler.h
    io/sonataloader/colorhandlers/node/VasculatureColorHandler.h
    io/sonataloader/data/SonataCells.h
    io/sonataloader/data/SonataConfig.h
    io/sonataloader/data/SonataEndFeetReader.h
    io/sonataloader/data/SonataSelection.h
    io/sonataloader/data/SonataSimulationMapping.h
    io/sonataloader/data/SonataSynapses.h
    io/sonataloader/data/SonataVasculature.h
    io/sonataloader/populations/EdgePopulationLoader.h
    io/sonataloader/populations/NodePopulationLoader.h
    io/sonataloader/populations/PopulationLoadManager.h
    io/sonataloader/populations/edges/ChemicalSynapsePopulationLoader.h
    io/sonataloader/populations/edges/CommonEdgeLoader.h
    io/sonataloader/populations/edges/ElectricalSynapsePopulationLoader.h
    io/sonataloader/populations/edges/EndFootPopulationLoader.h
    io/sonataloader/populations/edges/GlialGlialPopulationLoader.h
    io/sonataloader/populations/edges/SynapseAstrocytePopulationLoader.h
    io/sonataloader/populations/nodes/AstrocytePopulationLoader.h
    io/sonataloader/populations/nodes/BiophysicalPopulationLoader.h
    io/sonataloader/populations/nodes/CommonNodeLoader.h
    io/sonataloader/populations/nodes/PointNeuronPopulationLoader.h
    io/sonataloader/populations/nodes/VasculaturePopulationLoader.h
    io/sonataloader/reports/EdgeReportLoader.h
    io/sonataloader/reports/NodeReportLoader.h
    io/sonataloader/reports/PopulationReportManager.h
    io/sonataloader/reports/ReportType.h
    io/sonataloader/reports/handlers/SonataReportHandler.h
    io/sonataloader/reports/handlers/SonataSpikeHandler.h
    io/sonataloader/reports/handlers/VasculatureRadiiHandler.h
    io/sonataloader/reports/loaders/EdgeCompartmentLoader.h
    io/sonataloader/reports/loaders/NodeCompartmentLoader.h
    io/sonataloader/reports/loaders/NodeSpikeLoader.h
    io/sonataloader/reports/loaders/NodeVasculatureReportLoader.h
    io/synapse/SynapseGroup.h
    io/synapse/SynapseMaterialMap.h
    io/synapse/groups/EndFootGroup.h
    io/synapse/groups/OldSurfaceSynapseGroup.h
    io/synapse/groups/SurfaceSynapseGroup.h
    io/synapse/groups/SynapseAstrocyteGroup.h
    io/util/TransferFunctionUtils.h
    movie/MovieMaker.h
    network/adapters/ColoringInfoAdapter.h
    network/entrypoints/ColorCircuitEntrypoint.h
    network/entrypoints/MakeMovieEntrypoint.h
    network/entrypoints/MirrorModelEntrypoint.h
    network/entrypoints/SetCircuitThicknessEntrypoint.h
    network/entrypoints/SimulationColorEntrypoint.h
    network/entrypoints/TraceAnterogradeEntrypoint.h
    network/messages/AvailableColoringInfoMessage.h
    network/messages/ColorCircuitMessage.h
    network/messages/MakeMovieMessage.h
    network/messages/MirrorModelMessage.h
    network/messages/SetCircuitThicknessMessage.h
    network/messages/SimulationColorMessage.h
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
    braynsCommon braynsParameters braynsIO braynsEngine braynsPluginAPI braynsNetwork
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

