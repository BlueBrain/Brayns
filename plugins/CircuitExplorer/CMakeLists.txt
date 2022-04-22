# Copyright (c) 2015-2022, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSCIRCUITEXPLORER_SOURCES
    CircuitExplorerPlugin.cpp
    api/circuit/MorphologyCircuitBuilder.cpp
    api/circuit/SomaCircuitBuilder.cpp
    api/circuit/colorhandlers/MorphologyColorHandler.cpp
    api/circuit/colorhandlers/SomaColorHandler.cpp
    api/coloring/ColorByIDAlgorithm.cpp
    api/coloring/ColorIDParser.cpp
    api/coloring/ColorUtils.cpp
    api/neuron/NeuronGeometryBuilder.cpp
    api/neuron/NeuronMorphology.cpp
    api/neuron/NeuronMorphologyProcessor.cpp
    api/neuron/NeuronMorphologyReader.cpp
    api/reports/ReportMapping.cpp
    api/reports/TransferFunctionUtils.cpp
    api/reports/common/FrameTimeCalculator.cpp
    api/reports/common/SpikeUtils.cpp
    api/reports/indexers/OffsetIndexer.cpp
    api/reports/indexers/SpikeIndexer.cpp
    api/synapse/EndfeetColorHandler.cpp
    api/synapse/SynapseColorHandler.cpp
    api/vasculature/VasculatureColorHandler.cpp
    components/CircuitColorComponent.cpp
    components/EndfeetComponent.cpp
    components/MorphologyCircuitComponent.cpp
    components/ReportComponent.cpp
    components/SomaCircuitComponent.cpp
    components/SynapseComponent.cpp
    components/VasculatureComponent.cpp
    components/VasculatureRadiiReportComponent.cpp
    io/BBPLoader.cpp
    io/NeuronMorphologyLoader.cpp
    io/SonataLoader.cpp
    io/SonataNGVLoader.cpp
    io/bbploader/CellLoader.cpp
    io/bbploader/GIDLoader.cpp
    io/bbploader/ParameterCheck.cpp
    io/bbploader/ReportLoader.cpp
    io/bbploader/SynapseLoader.cpp
    io/bbploader/colordata/BBPNeuronColorData.cpp
    io/bbploader/colordata/BBPSynapseColorData.cpp
    io/bbploader/colordata/ColorDataExtractor.cpp
    io/bbploader/reports/CompartmentData.cpp
    io/bbploader/reports/SpikeData.cpp
    io/sonataloader/EdgeLoader.cpp
    io/sonataloader/LoaderTable.cpp
    io/sonataloader/NodeLoader.cpp
    io/sonataloader/ParameterCheck.cpp
    io/sonataloader/Selector.cpp
    io/sonataloader/colordata/ColorDataExtractor.cpp
    io/sonataloader/colordata/edge/CommonEdgeColorData.cpp
    io/sonataloader/colordata/node/AstrocyteColorData.cpp
    io/sonataloader/colordata/node/BiophysicalColorData.cpp
    io/sonataloader/colordata/node/PointNeuronColorData.cpp
    io/sonataloader/colordata/node/VasculatureColorData.cpp
    io/sonataloader/data/SonataCells.cpp
    io/sonataloader/data/SonataConfig.cpp
    io/sonataloader/data/SonataEndFeetReader.cpp
    io/sonataloader/data/SonataSimulationMapping.cpp
    io/sonataloader/data/SonataSynapses.cpp
    io/sonataloader/data/SonataVasculature.cpp
    io/sonataloader/populations/edges/ChemicalPopulationLoader.cpp
    io/sonataloader/populations/edges/ElectricalSynapsePopulationLoader.cpp
    io/sonataloader/populations/edges/EndFootPopulationLoader.cpp
    io/sonataloader/populations/edges/GlialGlialPopulationLoader.cpp
    io/sonataloader/populations/edges/SynapseAstrocytePopulationLoader.cpp
    io/sonataloader/populations/edges/common/SynapseImporter.cpp
    io/sonataloader/populations/nodes/AstrocytePopulationLoader.cpp
    io/sonataloader/populations/nodes/BiophysicalPopulationLoader.cpp
    io/sonataloader/populations/nodes/PointNeuronPopulationLoader.cpp
    io/sonataloader/populations/nodes/VasculaturePopulationLoader.cpp
    io/sonataloader/populations/nodes/common/MorphologyImporter.cpp
    io/sonataloader/populations/nodes/common/NeuronReportFactory.cpp
    io/sonataloader/populations/nodes/common/SomaImporter.cpp
    io/sonataloader/reports/SonataReportData.cpp
    io/sonataloader/reports/SonataSpikeData.cpp
    io/util/ProgressUpdater.cpp
    movie/MovieMaker.cpp
    network/entrypoints/ColorCircuitEntrypoint.cpp
    network/entrypoints/MakeMovieEntrypoint.cpp
    network/entrypoints/SetCircuitThicknessEntrypoint.cpp
    network/entrypoints/TraceAnterogradeEntrypoint.cpp
)

set(BRAYNSCIRCUITEXPLORER_PUBLIC_HEADERS
    CircuitExplorerPlugin.h
    api/circuit/MorphologyCircuitBuilder.h
    api/circuit/SomaCircuitBuilder.h
    api/circuit/colorhandlers/MorphologyColorHandler.h
    api/circuit/colorhandlers/SomaColorHandler.h
    api/coloring/ColorByIDAlgorithm.h
    api/coloring/ColorIDParser.h
    api/coloring/ColorUtils.h
    api/coloring/IColorData.h
    api/coloring/IColorHandler.h
    api/neuron/NeuronColorMethods.h
    api/neuron/NeuronGeometry.h
    api/neuron/NeuronGeometryBuilder.h
    api/neuron/NeuronMorphology.h
    api/neuron/NeuronMorphologyProcessor.h
    api/neuron/NeuronMorphologyReader.h
    api/neuron/NeuronSection.h
    api/reports/IColormapIndexer.h
    api/reports/IReportData.h
    api/reports/ReportMapping.h
    api/reports/TransferFunctionUtils.h
    api/reports/common/FrameTimeCalculator.h
    api/reports/common/SpikeUtils.h
    api/reports/indexers/OffsetIndexer.h
    api/reports/indexers/SpikeIndexer.h
    api/synapse/EndfeetColorHandler.h
    api/synapse/SynapseColorHandler.h
    api/vasculature/VasculatureColorHandler.h
    api/vasculature/VasculatureColorMethods.h
    api/vasculature/VasculatureSection.h
    components/CircuitColorComponent.h
    components/EndfeetComponent.h
    components/MorphologyCircuitComponent.h
    components/ReportComponent.h
    components/SomaCircuitComponent.h
    components/SynapseComponent.h
    components/VasculatureComponent.h
    components/VasculatureRadiiReportComponent.h
    io/BBPLoader.h
    io/BBPLoaderParameters.h
    io/NeuronMorphologyLoader.h
    io/NeuronMorphologyLoaderParameters.h
    io/SonataLoader.h
    io/SonataLoaderParameters.h
    io/SonataNGVLoader.h
    io/SonataNGVLoaderParameters.h
    io/bbploader/CellLoader.h
    io/bbploader/GIDLoader.h
    io/bbploader/LoadContext.h
    io/bbploader/ParameterCheck.h
    io/bbploader/ReportLoader.h
    io/bbploader/SynapseLoader.h
    io/bbploader/colordata/BBPNeuronColorData.h
    io/bbploader/colordata/BBPSynapseColorData.h
    io/bbploader/colordata/ColorDataExtractor.h
    io/bbploader/reports/CompartmentData.h
    io/bbploader/reports/ReportType.h
    io/bbploader/reports/SpikeData.h
    io/sonataloader/EdgeLoader.h
    io/sonataloader/LoadContext.h
    io/sonataloader/LoaderTable.h
    io/sonataloader/NodeLoader.h
    io/sonataloader/ParameterCheck.h
    io/sonataloader/Selector.h
    io/sonataloader/colordata/ColorDataExtractor.h
    io/sonataloader/colordata/edge/CommonEdgeColorData.h
    io/sonataloader/colordata/node/AstrocyteColorData.h
    io/sonataloader/colordata/node/BiophysicalColorData.h
    io/sonataloader/colordata/node/PointNeuronColorData.h
    io/sonataloader/colordata/node/VasculatureColorData.h
    io/sonataloader/data/SonataCells.h
    io/sonataloader/data/SonataConfig.h
    io/sonataloader/data/SonataEndFeetReader.h
    io/sonataloader/data/SonataSimulationMapping.h
    io/sonataloader/data/SonataSynapses.h
    io/sonataloader/data/SonataVasculature.h
    io/sonataloader/populations/EdgePopulationLoader.h
    io/sonataloader/populations/NodePopulationLoader.h
    io/sonataloader/populations/edges/ChemicalPopulationLoader.h
    io/sonataloader/populations/edges/ElectricalSynapsePopulationLoader.h
    io/sonataloader/populations/edges/EndFootPopulationLoader.h
    io/sonataloader/populations/edges/GlialGlialPopulationLoader.h
    io/sonataloader/populations/edges/SynapseAstrocytePopulationLoader.h
    io/sonataloader/populations/edges/common/SynapseImporter.h
    io/sonataloader/populations/nodes/AstrocytePopulationLoader.h
    io/sonataloader/populations/nodes/BiophysicalPopulationLoader.h
    io/sonataloader/populations/nodes/PointNeuronPopulationLoader.h
    io/sonataloader/populations/nodes/VasculaturePopulationLoader.h
    io/sonataloader/populations/nodes/common/ColorDataFactory.h
    io/sonataloader/populations/nodes/common/MorphologyImporter.h
    io/sonataloader/populations/nodes/common/NeuronReportFactory.h
    io/sonataloader/populations/nodes/common/SomaImporter.h
    io/sonataloader/reports/ReportType.h
    io/sonataloader/reports/SonataReportData.h
    io/sonataloader/reports/SonataSpikeData.h
    io/util/ProgressUpdater.h
    movie/MovieMaker.h
    network/adapters/ColoringInfoAdapter.h
    network/entrypoints/ColorCircuitEntrypoint.h
    network/entrypoints/MakeMovieEntrypoint.h
    network/entrypoints/SetCircuitThicknessEntrypoint.h
    network/entrypoints/TraceAnterogradeEntrypoint.h
    network/messages/AvailableColoringInfoMessage.h
    network/messages/ColorCircuitMessage.h
    network/messages/MakeMovieMessage.h
    network/messages/SetCircuitThicknessMessage.h
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

