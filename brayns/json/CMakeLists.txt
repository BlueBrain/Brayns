set(BRAYNSJSON_HEADERS
  Json.h
  JsonAdapter.h
  JsonAdapterMacro.h
  JsonBuffer.h
  JsonExtractor.h
  JsonOptions.h
  JsonSchema.h
  JsonSchemaValidator.h
  JsonType.h
  JsonObjectInfo.h
  JsonObjectMacro.h
)

set(BRAYNSJSON_SOURCES
    Json.cpp
    JsonExtractor.cpp
    JsonObjectInfo.cpp
    JsonOptions.cpp
    JsonSchema.cpp
    JsonSchemaValidator.cpp
    JsonType.cpp
    adapters/JsonSchemaAdapter.cpp
)

add_library(braynsJson SHARED ${BRAYNSJSON_SOURCES} ${BRAYNSJSON_HEADERS})

target_include_directories(braynsJson PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>)

target_link_libraries(braynsJson PUBLIC Poco::JSON glm)
target_link_libraries(braynsJson PRIVATE braynsUtils)

target_compile_options(braynsJson PRIVATE ${BRAYNS_COMPILE_OPTIONS})

set_target_properties(braynsJson
    PROPERTIES
        VERSION ${BRAYNS_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

install(TARGETS braynsJson
    EXPORT Brayns-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
