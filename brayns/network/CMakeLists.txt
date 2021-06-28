cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

project(braynsNetwork VERSION 0.1.0)

set(BRAYNSNETWORK_HEADERS
  entrypoint/Entrypoint.h
  entrypoint/IEntrypoint.h
  entrypoint/EntrypointException.h
  interface/ActionInterface.h
  messages/ActionMessage.h
  messages/Json.h
  messages/Message.h
  messages/MessageFactory.h
  plugin/NetworkManager.h
)

set(BRAYNSNETWORK_SOURCES
  interface/NetworkInterface.cpp
  interface/ServerInterface.cpp
  messages/JsonSchema.cpp
  messages/JsonSchemaValidator.cpp
  plugin/NetworkManager.cpp
)

set(BRAYNSNETWORK_INCLUDE_DIR ${PROJECT_SOURCE_DIR})

add_library(braynsNetwork SHARED ${BRAYNSNETWORK_SOURCES} ${BRAYNSNETWORK_HEADERS})

target_compile_options(braynsNetwork PRIVATE "-Werror")

target_include_directories(braynsNetwork PUBLIC
  $<BUILD_INTERFACE:${BRAYNSNETWORK_INCLUDE_DIR}>
  $<INSTALL_INTERFACE:include>)

target_link_libraries(braynsNetwork PRIVATE
  braynsCommon
  braynsParameters
  braynsPluginAPI
  Poco::Foundation
  Poco::Net Poco::JSON
)

set_target_properties(braynsNetwork
    PROPERTIES
        VERSION ${Brayns_VERSION}
        SOVERSION ${Brayns_VERSION_MAJOR}.${Brayns_VERSION_MINOR}
)

install(TARGETS braynsNetwork
    EXPORT braynsNetwork-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(EXPORT braynsNetwork-targets
    DESTINATION share/CMake
)
