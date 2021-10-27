#cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

#project(braynsJson VERSION 0.1.0)

set(BRAYNSJSON_HEADERS
  Json.h
  JsonAdapter.h
  JsonBuffer.h
  JsonExtractor.h
  JsonOptions.h
  JsonSchema.h
  JsonSchemaValidator.h
  JsonType.h
  Message.h
  MessageAdapter.h
)

set(BRAYNSJSON_SOURCES
    JsonSchemaValidator.cpp
)

set(BRAYNSJSON_INCLUDE_NAME brayns/json)
set(BRAYNSJSON_LINK_LIBRARIES
  PUBLIC Poco::Foundation Poco::JSON glm
  PRIVATE braynsUtils
)

#add_library(braynsJson SHARED ${BRAYNSJSON_SOURCES} ${BRAYNSJSON_HEADERS})
common_library(braynsJson)


#target_include_directories(braynsJson PUBLIC
#  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
#  $<INSTALL_INTERFACE:include>)

#target_link_libraries(braynsJson PUBLIC
#  Poco::Foundation
#  Poco::JSON
#)

#set_target_properties(braynsJson
#    PROPERTIES
#        VERSION ${Brayns_VERSION}
#        SOVERSION ${Brayns_VERSION_MAJOR}.${Brayns_VERSION_MINOR}
#)

#install(TARGETS braynsJson
#    EXPORT braynsJson-targets
#    LIBRARY DESTINATION lib
#    ARCHIVE DESTINATION lib
#)

#install(EXPORT braynsJson-targets
#    DESTINATION share/CMake
#)
