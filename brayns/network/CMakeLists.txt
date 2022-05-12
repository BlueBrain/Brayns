# Copyright (c) 2015-2022, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSNETWORK_HEADERS
  adapters/ApplicationParametersAdapter.h
  adapters/BinaryLoadParametersAdapter.h
  adapters/BoundsAdapter.h
  adapters/CameraAdapter.h
  adapters/FileLoadParametersAdapter.h
  adapters/GeometryAdapter.h
  adapters/LightAdapter.h
  adapters/LoaderInfoAdapter.h
  adapters/MaterialAdapter.h
  adapters/ModelInstanceAdapter.h
  adapters/ModelTransferFunctionAdapter.h
  adapters/RendererAdapter.h
  adapters/SimulationParametersAdapter.h
  adapters/TransferFunctionAdapter.h
  adapters/TransformationAdapter.h
  binary/BinaryManager.h
  client/ClientBuffer.h
  client/ClientManager.h
  client/ClientRef.h
  client/ClientSender.h
  client/RequestBuffer.h
  common/CancellationToken.h
  common/Clock.h
  common/EngineObjectFactory.h
  common/ExtractMaterial.h
  common/ExtractModel.h
  common/ProgressHandler.h
  common/RateLimiter.h
  dispatch/RequestDispatcher.h
  entrypoint/Entrypoint.h
  entrypoint/EntrypointBuilder.h
  entrypoint/EntrypointFinder.h
  entrypoint/EntrypointRef.h
  entrypoint/EntrypointRegistry.h
  entrypoint/EntrypointRequest.h
  entrypoint/IEntrypoint.h
  entrypoint/ObjectEntrypoint.h
  entrypoint/PropertyObjectEntrypoint.h
  entrypoints/AddClipPlaneEntrypoint.h
  entrypoints/AddGeometryEntrypoint.h
  entrypoints/AddLightEntrypoint.h
  entrypoints/AddModelEntrypoint.h
  entrypoints/ApplicationParametersEntrypoint.h
  entrypoints/CameraEntrypoint.h
  entrypoints/CameraLookAtEntrypoint.h
  entrypoints/CancelEntrypoint.h
  entrypoints/ClearLightsEntrypoint.h
  entrypoints/EnableSimulationEntrypoint.h
  entrypoints/ExitLaterEntrypoint.h
  entrypoints/ExportFramesEntrypoint.h
  entrypoints/GetLoadersEntrypoint.h
  entrypoints/GetModelEntrypoint.h
  entrypoints/ImageJpegEntrypoint.h
  entrypoints/ImageStreamingModeEntrypoint.h
  entrypoints/InspectEntrypoint.h
  entrypoints/ModelMaterialEntrypoint.h
  entrypoints/ModelTransferFunctionEntrypoint.h
  entrypoints/QuitEntrypoint.h
  entrypoints/RegistryEntrypoint.h
  entrypoints/RemoveClipPlanesEntrypoint.h
  entrypoints/RemoveLightsEntrypoint.h
  entrypoints/RemoveModelEntrypoint.h
  entrypoints/RendererEntrypoint.h
  entrypoints/RequestModelUploadEntrypoint.h
  entrypoints/SceneEntrypoint.h
  entrypoints/SchemaEntrypoint.h
  entrypoints/SimulationParametersEntrypoint.h
  entrypoints/SnapshotEntrypoint.h
  entrypoints/TriggerJpegStreamEntrypoint.h
  entrypoints/UpdateModelEntrypoint.h
  entrypoints/VersionEntrypoint.h
  interface/INetworkInterface.h
  interface/NetworkInterface.h
  jsonrpc/JsonRpcException.h
  jsonrpc/JsonRpcFactory.h
  jsonrpc/JsonRpcMessages.h
  jsonrpc/JsonRpcParser.h
  jsonrpc/JsonRpcRequest.h
  jsonrpc/JsonRpcSender.h
  jsonrpc/RequestId.h
  messages/ExitLaterMessage.h
  messages/ExportFramesMessage.h
  messages/GetModelMessage.h
  messages/ImageBase64Message.h
  messages/ImageSettingsMessage.h
  messages/ImageStreamingModeMessage.h
  messages/InspectMessage.h
  messages/RemoveClipPlanesMessage.h
  messages/RemoveLightsMessage.h
  messages/RemoveModelMessage.h
  messages/SchemaMessage.h
  messages/SnapshotMessage.h
  messages/VersionMessage.h
  plugin/NetworkManager.h
  socket/ClientSocket.h
  socket/ISocket.h
  socket/ServerSocket.h
  socket/SocketListener.h
  socket/SocketManager.h
  stream/StreamManager.h
  stream/StreamMonitor.h
  task/BinaryTask.h
  task/ITask.h
  task/JsonRpcTask.h
  task/TaskManager.h
  websocket/InputPacket.h
  websocket/OutputPacket.h
  websocket/WebSocket.h
)
  
set(BRAYNSNETWORK_SOURCES
  adapters/ClipPlaneAdapter.cpp
  adapters/ModelInstanceAdapter.cpp
  adapters/ModelTransferFunctionAdapter.cpp
  adapters/SceneAdapter.cpp
  binary/BinaryManager.cpp
  client/ClientBuffer.cpp
  client/ClientManager.cpp
  client/ClientRef.cpp
  client/ClientRequest.cpp
  client/ClientSender.cpp
  client/RequestBuffer.cpp
  common/CancellationToken.cpp
  common/EngineObjectFactory.cpp
  common/ExtractMaterial.cpp
  common/ExtractModel.cpp
  common/RateLimiter.cpp
  dispatch/RequestDispatcher.cpp
  entrypoint/EntrypointFinder.cpp
  entrypoint/EntrypointRef.cpp
  entrypoint/EntrypointRegistry.cpp
  entrypoints/AddClipPlaneEntrypoint.cpp
  entrypoints/AddGeometryEntrypoint.cpp
  entrypoints/AddLightEntrypoint.cpp
  entrypoints/AddModelEntrypoint.cpp
  entrypoints/ApplicationParametersEntrypoint.cpp
  entrypoints/CameraEntrypoint.cpp
  entrypoints/CameraLookAtEntrypoint.cpp
  entrypoints/CancelEntrypoint.cpp
  entrypoints/ClearLightsEntrypoint.cpp
  entrypoints/EnableSimulationEntrypoint.cpp
  entrypoints/ExitLaterEntrypoint.cpp
  entrypoints/ExportFramesEntrypoint.cpp
  entrypoints/GetLoadersEntrypoint.cpp
  entrypoints/GetModelEntrypoint.cpp
  entrypoints/ImageJpegEntrypoint.cpp
  entrypoints/ImageStreamingModeEntrypoint.cpp
  entrypoints/InspectEntrypoint.cpp
  entrypoints/ModelMaterialEntrypoint.cpp
  entrypoints/ModelTransferFunctionEntrypoint.cpp
  entrypoints/QuitEntrypoint.cpp
  entrypoints/RegistryEntrypoint.cpp
  entrypoints/RemoveClipPlanesEntrypoint.cpp
  entrypoints/RemoveLightsEntrypoint.cpp
  entrypoints/RemoveModelEntrypoint.cpp
  entrypoints/RendererEntrypoint.cpp
  entrypoints/RequestModelUploadEntrypoint.cpp
  entrypoints/SceneEntrypoint.cpp
  entrypoints/SchemaEntrypoint.cpp
  entrypoints/SimulationParametersEntrypoint.cpp
  entrypoints/SnapshotEntrypoint.cpp
  entrypoints/TriggerJpegStreamEntrypoint.cpp
  entrypoints/UpdateModelEntrypoint.cpp
  entrypoints/VersionEntrypoint.cpp
  interface/NetworkInterface.cpp
  jsonrpc/JsonRpcException.cpp
  jsonrpc/JsonRpcFactory.cpp
  jsonrpc/JsonRpcParser.cpp
  jsonrpc/JsonRpcRequest.cpp
  jsonrpc/JsonRpcSender.cpp
  jsonrpc/RequestId.cpp
  plugin/NetworkManager.cpp
  socket/ClientSocket.cpp
  socket/ServerSocket.cpp
  socket/SocketListener.cpp
  socket/SocketManager.cpp
  stream/StreamManager.cpp
  stream/StreamMonitor.cpp
  task/BinaryTask.cpp
  task/JsonRpcTask.cpp
  task/TaskManager.cpp
  websocket/InputPacket.cpp
  websocket/OutputPacket.cpp
  websocket/WebSocket.cpp
)

set(BRAYNSNETWORK_INCLUDE_DIR ${PROJECT_SOURCE_DIR})

add_library(braynsNetwork SHARED ${BRAYNSNETWORK_SOURCES} ${BRAYNSNETWORK_HEADERS})

target_include_directories(braynsNetwork PUBLIC
  $<BUILD_INTERFACE:${BRAYNSNETWORK_INCLUDE_DIR}>
  $<INSTALL_INTERFACE:include>)

target_link_libraries(braynsNetwork PRIVATE
  braynsCommon
  braynsPluginAPI
  braynsUtils
)

target_link_libraries(braynsNetwork PUBLIC
  braynsJson
  braynsIO
  Poco::Foundation
  Poco::Net
  Poco::NetSSL
)

target_compile_options(braynsNetwork PRIVATE ${BRAYNS_COMPILE_OPTIONS})

set_target_properties(braynsNetwork
    PROPERTIES
        VERSION ${BRAYNS_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

install(TARGETS braynsNetwork
    EXPORT Brayns-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
