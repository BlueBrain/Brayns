file(GLOB_RECURSE BRAYNSJSON_HEADERS RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.h)
file(GLOB_RECURSE BRAYNSJSON_SOURCES RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.cpp)

add_library(braynsJson SHARED ${BRAYNSJSON_SOURCES} ${BRAYNSJSON_HEADERS})

target_include_directories(braynsJson PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(braynsJson PUBLIC Poco::JSON braynsUtils)

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
