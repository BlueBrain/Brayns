# Copyright (c) 2015-2021, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

# reuse ispc setup and macros from ospray
list(APPEND CMAKE_MODULE_PATH ${OSPRAY_CMAKE_ROOT})
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set(OSPRAY_DEBUG_BUILD ON)
endif()
include(ispc)

set(BRAYNSCIRCUITEXPLORER_SOURCES
    module/CircuitExplorer.cpp
    module/ispc/camera/DOFPerspectiveCamera.cpp
    module/ispc/camera/SphereClippingPerspectiveCamera.cpp
    module/ispc/render/CircuitExplorerMaterial.cpp
    module/ispc/render/CircuitExplorerBasicRenderer.cpp
    module/ispc/render/CircuitExplorerAdvancedRenderer.cpp
    module/ispc/render/VoxelizedSimulationRenderer.cpp
    module/ispc/render/ProximityDetectionRenderer.cpp
    module/ispc/render/CellGrowthRenderer.cpp
    module/ispc/render/utils/CircuitExplorerAbstractRenderer.cpp
    module/ispc/render/utils/CircuitExplorerSimulationRenderer.cpp
    plugin/CircuitExplorerPlugin.cpp
    plugin/api/CameraUtils.cpp
    plugin/api/CircuitColorHandler.cpp
    plugin/api/CircuitColorManager.cpp
    plugin/api/ColorUtils.cpp
    plugin/api/MaterialUtils.cpp
    plugin/api/VasculatureRadiiSimulation.cpp
    plugin/io/BBPLoader.cpp
    plugin/io/NeuronMorphologyLoader.cpp
    plugin/io/SonataLoader.cpp
    plugin/io/SonataNGVLoader.cpp
    plugin/io/bbploader/CellLoader.cpp
    plugin/io/bbploader/ParameterCheck.cpp
    plugin/io/bbploader/SynapseLoader.cpp
    plugin/io/bbploader/colorhandlers/BBPNeuronColorHandler.cpp
    plugin/io/bbploader/colorhandlers/BBPSynapseColorHandler.cpp
    plugin/io/bbploader/simulation/CompartmentSimulation.cpp
    plugin/io/bbploader/simulation/SpikeSimulation.cpp
    plugin/io/bbploader/simulation/handlers/CompartmentHandler.cpp
    plugin/io/bbploader/simulation/handlers/SpikeHandler.cpp
    plugin/io/morphology/neuron/NeuronBuilder.cpp
    plugin/io/morphology/neuron/NeuronMorphology.cpp
    plugin/io/morphology/neuron/NeuronMorphologyPipeline.cpp
    plugin/io/morphology/neuron/builders/PrimitiveNeuronBuilder.cpp
    plugin/io/morphology/neuron/builders/SDFNeuronBuilder.cpp
    plugin/io/morphology/neuron/builders/SampleNeuronBuilder.cpp
    plugin/io/morphology/neuron/instances/PrimitiveNeuronInstance.cpp
    plugin/io/morphology/neuron/instances/SDFNeuronInstance.cpp
    plugin/io/morphology/neuron/instances/SampleNeuronInstance.cpp
    plugin/io/morphology/neuron/pipeline/RadiusMultiplier.cpp
    plugin/io/morphology/neuron/pipeline/RadiusOverride.cpp
    plugin/io/morphology/neuron/pipeline/RadiusSmoother.cpp
    plugin/io/morphology/neuron/pipeline/SectionWelder.cpp
    plugin/io/morphology/vasculature/VasculatureInstance.cpp
    plugin/io/sonataloader/ParameterCheck.cpp
    plugin/io/sonataloader/colorhandlers/PopulationColorManager.cpp
    plugin/io/sonataloader/colorhandlers/edge/CommonEdgeColorHandler.cpp
    plugin/io/sonataloader/colorhandlers/edge/EndFootColorHandler.cpp
    plugin/io/sonataloader/colorhandlers/node/SonataNeuronColorHandler.cpp
    plugin/io/sonataloader/colorhandlers/node/VasculatureColorHandler.cpp
    plugin/io/sonataloader/data/SonataCells.cpp
    plugin/io/sonataloader/data/SonataConfig.cpp
    plugin/io/sonataloader/data/SonataEndFeetReader.cpp
    plugin/io/sonataloader/data/SonataSelection.cpp
    plugin/io/sonataloader/data/SonataSimulationMapping.cpp
    plugin/io/sonataloader/data/SonataSynapses.cpp
    plugin/io/sonataloader/data/SonataVasculature.cpp
    plugin/io/sonataloader/populations/PopulationLoadManager.cpp
    plugin/io/sonataloader/populations/edges/CommonEdgeLoader.cpp
    plugin/io/sonataloader/populations/edges/EndFootPopulationLoader.cpp
    plugin/io/sonataloader/populations/edges/SynapseAstrocytePopulationLoader.cpp
    plugin/io/sonataloader/populations/nodes/AstrocytePopulationLoader.cpp
    plugin/io/sonataloader/populations/nodes/BiophysicalPopulationLoader.cpp
    plugin/io/sonataloader/populations/nodes/CommonNodeLoader.cpp
    plugin/io/sonataloader/populations/nodes/PointNeuronPopulationLoader.cpp
    plugin/io/sonataloader/populations/nodes/VasculaturePopulationLoader.cpp
    plugin/io/sonataloader/reports/PopulationReportManager.cpp
    plugin/io/sonataloader/reports/handlers/SonataReportHandler.cpp
    plugin/io/sonataloader/reports/handlers/SonataSpikeHandler.cpp
    plugin/io/sonataloader/reports/handlers/VasculatureRadiiHandler.cpp
    plugin/io/sonataloader/reports/loaders/EdgeCompartmentLoader.cpp
    plugin/io/sonataloader/reports/loaders/NodeCompartmentLoader.cpp
    plugin/io/sonataloader/reports/loaders/NodeSpikeLoader.cpp
    plugin/io/sonataloader/reports/loaders/NodeVasculatureReportLoader.cpp
    plugin/io/synapse/groups/EndFootGroup.cpp
    plugin/io/synapse/groups/OldSurfaceSynapseGroup.cpp
    plugin/io/synapse/groups/SurfaceSynapseGroup.cpp
    plugin/io/synapse/groups/SynapseAstrocyteGroup.cpp
    plugin/io/util/TransferFunctionUtils.cpp
    plugin/movie/MovieMaker.cpp
)

set(BRAYNSCIRCUITEXPLORER_PUBLIC_HEADERS
    plugin/CircuitExplorerPlugin.h
    plugin/api/CameraUtils.h
    plugin/api/CircuitColorHandler.h
    plugin/api/CircuitColorManager.h
    plugin/api/ColorUtils.h
    plugin/api/MaterialEnums.h
    plugin/api/MaterialUtils.h
    plugin/api/VasculatureRadiiSimulation.h
    plugin/io/BBPLoader.h
    plugin/io/BBPLoaderParameters.h
    plugin/io/NeuronMorphologyLoader.h
    plugin/io/NeuronMorphologyLoaderParameters.h
    plugin/io/SonataLoader.h
    plugin/io/SonataLoaderParameters.h
    plugin/io/SonataNGVLoader.h
    plugin/io/SonataNGVLoaderParameters.h
    plugin/io/bbploader/CellLoader.h
    plugin/io/bbploader/ParameterCheck.h
    plugin/io/bbploader/SynapseLoader.h
    plugin/io/bbploader/colorhandlers/BBPNeuronColorHandler.h
    plugin/io/bbploader/colorhandlers/BBPSynapseColorHandler.h
    plugin/io/bbploader/simulation/CompartmentSimulation.h
    plugin/io/bbploader/simulation/Simulation.h
    plugin/io/bbploader/simulation/SimulationType.h
    plugin/io/bbploader/simulation/SpikeSimulation.h
    plugin/io/bbploader/simulation/handlers/CompartmentHandler.h
    plugin/io/bbploader/simulation/handlers/SpikeHandler.h
    plugin/io/morphology/MorphologyInstance.h
    plugin/io/morphology/neuron/NeuronBuilder.h
    plugin/io/morphology/neuron/NeuronColorHandler.h
    plugin/io/morphology/neuron/NeuronGeometryType.h
    plugin/io/morphology/neuron/NeuronMaterialMap.h
    plugin/io/morphology/neuron/NeuronMorphology.h
    plugin/io/morphology/neuron/NeuronMorphologyPipeline.h
    plugin/io/morphology/neuron/NeuronSection.h
    plugin/io/morphology/neuron/builders/PrimitiveNeuronBuilder.h
    plugin/io/morphology/neuron/builders/SDFNeuronBuilder.h
    plugin/io/morphology/neuron/builders/SampleNeuronBuilder.h
    plugin/io/morphology/neuron/instances/PrimitiveNeuronInstance.h
    plugin/io/morphology/neuron/instances/SDFNeuronInstance.h
    plugin/io/morphology/neuron/instances/SampleNeuronInstance.h
    plugin/io/morphology/neuron/pipeline/RadiusMultiplier.h
    plugin/io/morphology/neuron/pipeline/RadiusOverride.h
    plugin/io/morphology/neuron/pipeline/RadiusSmoother.h
    plugin/io/morphology/neuron/pipeline/SectionWelder.h
    plugin/io/morphology/vasculature/VasculatureInstance.h
    plugin/io/morphology/vasculature/VasculatureMaterialMap.h
    plugin/io/morphology/vasculature/VasculatureSection.h
    plugin/io/sonataloader/ParameterCheck.h
    plugin/io/sonataloader/colorhandlers/PopulationColorManager.h
    plugin/io/sonataloader/colorhandlers/edge/CommonEdgeColorHandler.h
    plugin/io/sonataloader/colorhandlers/edge/EndFootColorHandler.h
    plugin/io/sonataloader/colorhandlers/node/SonataNeuronColorHandler.h
    plugin/io/sonataloader/colorhandlers/node/VasculatureColorHandler.h
    plugin/io/sonataloader/data/SonataCells.h
    plugin/io/sonataloader/data/SonataConfig.h
    plugin/io/sonataloader/data/SonataEndFeetReader.h
    plugin/io/sonataloader/data/SonataSelection.h
    plugin/io/sonataloader/data/SonataSimulationMapping.h
    plugin/io/sonataloader/data/SonataSynapses.h
    plugin/io/sonataloader/data/SonataVasculature.h
    plugin/io/sonataloader/populations/EdgePopulationLoader.h
    plugin/io/sonataloader/populations/NodePopulationLoader.h
    plugin/io/sonataloader/populations/PopulationLoadManager.h
    plugin/io/sonataloader/populations/edges/ChemicalSynapsePopulationLoader.h
    plugin/io/sonataloader/populations/edges/CommonEdgeLoader.h
    plugin/io/sonataloader/populations/edges/ElectricalSynapsePopulationLoader.h
    plugin/io/sonataloader/populations/edges/EndFootPopulationLoader.h
    plugin/io/sonataloader/populations/edges/GlialGlialPopulationLoader.h
    plugin/io/sonataloader/populations/edges/SynapseAstrocytePopulationLoader.h
    plugin/io/sonataloader/populations/nodes/AstrocytePopulationLoader.h
    plugin/io/sonataloader/populations/nodes/BiophysicalPopulationLoader.h
    plugin/io/sonataloader/populations/nodes/CommonNodeLoader.h
    plugin/io/sonataloader/populations/nodes/PointNeuronPopulationLoader.h
    plugin/io/sonataloader/populations/nodes/VasculaturePopulationLoader.h
    plugin/io/sonataloader/reports/EdgeReportLoader.h
    plugin/io/sonataloader/reports/NodeReportLoader.h
    plugin/io/sonataloader/reports/PopulationReportManager.h
    plugin/io/sonataloader/reports/ReportType.h
    plugin/io/sonataloader/reports/handlers/SonataReportHandler.h
    plugin/io/sonataloader/reports/handlers/SonataSpikeHandler.h
    plugin/io/sonataloader/reports/handlers/VasculatureRadiiHandler.h
    plugin/io/sonataloader/reports/loaders/EdgeCompartmentLoader.h
    plugin/io/sonataloader/reports/loaders/NodeCompartmentLoader.h
    plugin/io/sonataloader/reports/loaders/NodeSpikeLoader.h
    plugin/io/sonataloader/reports/loaders/NodeVasculatureReportLoader.h
    plugin/io/synapse/SynapseGroup.h
    plugin/io/synapse/SynapseMaterialMap.h
    plugin/io/synapse/groups/EndFootGroup.h
    plugin/io/synapse/groups/OldSurfaceSynapseGroup.h
    plugin/io/synapse/groups/SurfaceSynapseGroup.h
    plugin/io/synapse/groups/SynapseAstrocyteGroup.h
    plugin/io/util/TransferFunctionUtils.h
    plugin/movie/MovieMaker.h
    plugin/network/adapters/ColoringInfoAdapter.h
    plugin/network/adapters/MaterialAdapter.h
    plugin/network/adapters/SetMaterialRangeAdapter.h
    plugin/network/entrypoints/AddBoxEntrypoint.h
    plugin/network/entrypoints/AddColumnEntrypoint.h
    plugin/network/entrypoints/AddCylinderEntrypoint.h
    plugin/network/entrypoints/AddGridEntrypoint.h
    plugin/network/entrypoints/AddPillEntrypoint.h
    plugin/network/entrypoints/AddSphereEntrypoint.h
    plugin/network/entrypoints/ColorCircuitEntryPoint.h
    plugin/network/entrypoints/GetMaterialIdsEntrypoint.h
    plugin/network/entrypoints/MakeMovieEntrypoint.h
    plugin/network/entrypoints/MaterialEntrypoint.h
    plugin/network/entrypoints/MirrorModelEntrypoint.h
    plugin/network/entrypoints/SetCircuitThicknessEntrypoint.h
    plugin/network/entrypoints/SetMaterialExtraAttributesEntrypoint.h
    plugin/network/entrypoints/SetMaterialRangeEntrypoint.h
    plugin/network/entrypoints/SetMaterialsEntrypoint.h
    plugin/network/entrypoints/SimulationColorEntryPoint.h
    plugin/network/entrypoints/TraceAnterogradeEntrypoint.h
    plugin/network/messages/AddBoxMessage.h
    plugin/network/messages/AddColumnMessage.h
    plugin/network/messages/AddCylinderMessage.h
    plugin/network/messages/AddGridMessage.h
    plugin/network/messages/AddPillMessage.h
    plugin/network/messages/AddShapeMessage.h
    plugin/network/messages/AddSphereMessage.h
    plugin/network/messages/AvailableColoringInfoMessage.h
    plugin/network/messages/ColorCircuitMessage.h
    plugin/network/messages/GetMaterialIdsMessage.h
    plugin/network/messages/GetMaterialMessage.h
    plugin/network/messages/MakeMovieMessage.h
    plugin/network/messages/MirrorModelMessage.h
    plugin/network/messages/SetCircuitThicknessMessage.h
    plugin/network/messages/SetMaterialsMessage.h
    plugin/network/messages/SimulationColorMessage.h
    plugin/network/messages/TraceAnterogradeMessage.h
)

set(BRAYNSCIRCUITEXPLORER_ISPC_SOURCES
    module/ispc/camera/utils.ispc
    module/ispc/camera/DOFPerspectiveCamera.ispc
    module/ispc/camera/SphereClippingPerspectiveCamera.ispc
    module/ispc/render/CircuitExplorerMaterial.ispc
    module/ispc/render/CircuitExplorerBasicRenderer.ispc
    module/ispc/render/CircuitExplorerAdvancedRenderer.ispc
    module/ispc/render/VoxelizedSimulationRenderer.ispc
    module/ispc/render/ProximityDetectionRenderer.ispc
    module/ispc/render/CellGrowthRenderer.ispc
    module/ispc/render/utils/CircuitExplorerSimulationRenderer.ispc
    module/ispc/render/utils/CircuitExplorerRandomGenerator.ispc
)

# Compile ispc code
list(APPEND ALL_ISPC_INCLUDES ${OSPRAY_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR})
include_directories_ispc(${ALL_ISPC_INCLUDES})
ospray_ispc_compile(${BRAYNSCIRCUITEXPLORER_ISPC_SOURCES})
list(APPEND BRAYNSCIRCUITEXPLORER_SOURCES ${ISPC_OBJECTS})

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

set(CIRCUITEXPLORER_INCLUDEDIR_LIST ${CMAKE_CURRENT_LIST_DIR} ${OSPRAY_INCLUDE_DIRS})

target_include_directories(braynsCircuitExplorer PUBLIC
  "$<BUILD_INTERFACE:${CIRCUITEXPLORER_INCLUDEDIR_LIST}>"
  "$<INSTALL_INTERFACE:include>")

target_link_libraries(braynsCircuitExplorer PUBLIC ${OSPRAY_LIBRARIES})
target_link_libraries(braynsCircuitExplorer PRIVATE
    braynsCommon braynsParameters braynsIO braynsEngine braynsPluginAPI braynsNetwork
    sonata::sonata_shared MorphIO::morphio Brion Brain ${HDF5_LIBRARIES}
    HighFive MVDTool braynsOSPRayEngine
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
# Temporary, for ISPC Renderers
target_compile_options(braynsCircuitExplorer PRIVATE -Wno-strict-aliasing)
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

