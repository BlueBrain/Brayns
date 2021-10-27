# Copyright (c) 2015-2019, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(braynsCircuitExplorer VERSION 1.0.0)
set(braynsCircuitExplorer_VERSION_ABI 1)

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/CMake/common/Common.cmake)
  message(FATAL_ERROR "CMake/common missing, run: git submodule update --init --recursive")
endif()

include(Common)
common_find_package(sonata)
common_find_package(Brion)
common_find_package(HighFive)
common_find_package(MVDTool)
common_find_package(nlohmann_json)
common_find_package(ospray 1.8 SYSTEM)
common_find_package(CGAL)
common_find_package(OpenMP)
common_find_package_post()

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
    plugin/api/FrameExportManager.cpp
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
    plugin/io/morphology/neuron/NeuronMorphology.cpp
    plugin/io/morphology/neuron/NeuronMorphologyImporter.cpp
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
    plugin/api/FrameExportManager.h
    plugin/api/Log.h
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
    plugin/io/morphology/neuron/NeuronMorphologyImporter.h
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
    plugin/network/adapters/FrameExportdapter.h
    plugin/network/adapters/MaterialAdapter.h
    plugin/network/adapters/ODUCameraAdapter.h
    plugin/network/adapters/SetMaterialRangeAdapter.h
    plugin/network/entrypoints/AddBoxEntrypoint.h
    plugin/network/entrypoints/AddColumnEntrypoint.h
    plugin/network/entrypoints/AddCylinderEntrypoint.h
    plugin/network/entrypoints/AddGridEntrypoint.h
    plugin/network/entrypoints/AddPillEntrypoint.h
    plugin/network/entrypoints/AddSphereEntrypoint.h
    plugin/network/entrypoints/ColorCircuitEntryPoint.h
    plugin/network/entrypoints/ExportFramesToDiskEntrypoint.h
    plugin/network/entrypoints/GetExportFramesProgressEntrypoint.h
    plugin/network/entrypoints/GetMaterialIdsEntrypoint.h
    plugin/network/entrypoints/MakeMovieEntrypoint.h
    plugin/network/entrypoints/MaterialEntrypoint.h
    plugin/network/entrypoints/MirrorModelEntrypoint.h
    plugin/network/entrypoints/OduCameraEntrypoint.h
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
    plugin/network/messages/ExportFramesToDiskMessage.h
    plugin/network/messages/GetExportFramesProgressMessage.h
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
set(BRAYNS_SOURCE_DIR ${PROJECT_SOURCE_DIR}/../..)
list(APPEND ALL_ISPC_INCLUDES ${OSPRAY_INCLUDE_DIRS} ${BRAYNS_SOURCE_DIR})
include_directories_ispc(${ALL_ISPC_INCLUDES} )
ospray_ispc_compile(${BRAYNSCIRCUITEXPLORER_ISPC_SOURCES})
list(APPEND BRAYNSCIRCUITEXPLORER_SOURCES ${ISPC_OBJECTS})

# Compile C++ code
include_directories(
    SYSTEM
    ${CMAKE_CURRENT_SOURCE_DIR}/.
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

set(BRAYNSCIRCUITEXPLORER_LINK_LIBRARIES
    PUBLIC ${OSPRAY_LIBRARIES}
    PRIVATE braynsCommon braynsParameters braynsIO braynsEngine braynsPluginAPI braynsNetwork
    braynsOSPRayEngine glm sonata::sonata_shared MorphIO::morphio_shared Brion Brain
    ${FREEIMAGE_LIBRARIES} ${HDF5_LIBRARIES}
    HighFive
    nlohmann_json::nlohmann_json
    MVDTool
)

set(BRAYNSCIRCUITEXPLORER_OMIT_LIBRARY_HEADER ON)
set(BRAYNSCIRCUITEXPLORER_OMIT_VERSION_HEADERS ON)
set(BRAYNSCIRCUITEXPLORER_OMIT_EXPORT ON)

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

if(FFMPEG_FOUND)
    message(STATUS "FFmpeg dev libraries found, enable movie generation")
    list(APPEND BRAYNSCIRCUITEXPLORER_LINK_LIBRARIES
        ${AVFORMAT_LIBRARY}
        ${AVCODEC_LIBRARY}
        ${AVUTIL_LIBRARY}
        ${SWSCALE_LIBRARY}
    )
else()
    message(STATUS "One or more FFmpeg dev libraries not found, disable movie generation")
endif()

list(APPEND BRAYNSCIRCUITEXPLORER_SOURCES plugin/movie/MovieMaker.cpp)

common_library(braynsCircuitExplorer)

if(FFMPEG_FOUND)
    target_compile_definitions(braynsCircuitExplorer PRIVATE BRAYNS_USE_FFMPEG)
endif()
