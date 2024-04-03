# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Authors: Daniel.Nachbaur@epfl.ch
# Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

include(CTest)
enable_testing()

configure_file(paths.h.in ${PROJECT_BINARY_DIR}/tests/paths.h)

file(GLOB_RECURSE TEST_LIST RELATIVE ${CMAKE_CURRENT_LIST_DIR} Test*.cpp)

set(TEST_TARGET_LIST)

foreach(TEST ${TEST_LIST})
    get_filename_component(TEST_NAME ${TEST} NAME_WLE)

    add_executable(${TEST_NAME} ${TEST})
    target_compile_options(${TEST_NAME} PRIVATE ${BRAYNS_COMPILE_OPTIONS})
    target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${PROJECT_BINARY_DIR})
    target_link_libraries(${TEST_NAME} PRIVATE brayns doctest)

    set_target_properties(${TEST_NAME} PROPERTIES FOLDER tests OUTPUT_NAME ${TEST_NAME})

    set(TEST_TARGET_LIST ${TEST_TARGET_LIST} ${TEST_NAME})

    add_test(NAME ${TEST_NAME}
        COMMAND $<TARGET_FILE:${TEST_NAME}> -l test_suite
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(${TEST_NAME} PROPERTIES LABELS "Brayns-unit")
endforeach()

add_custom_target(Brayns-cpptests USES_TERMINAL
    COMMAND ${CMAKE_CTEST_COMMAND} -T test --no-compress-output
    --output-on-failure -L Brayns-unit -C $<CONFIGURATION> \${ARGS}
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/tests
    COMMENT "Running Brayns unit tests")
set_target_properties(Brayns-cpptests PROPERTIES
    EXCLUDE_FROM_DEFAULT_BUILD ON FOLDER "tests")

if(NOT TARGET tests)
    add_custom_target(tests)
    set_target_properties(tests PROPERTIES
        EXCLUDE_FROM_DEFAULT_BUILD ON FOLDER ${PROJECT_BINARY_DIR}/tests)
endif()

if(NOT TARGET test)
    add_custom_target(test)
    set_target_properties(test PROPERTIES
        EXCLUDE_FROM_DEFAULT_BUILD ON FOLDER ${PROJECT_BINARY_DIR}/tests)
endif()

add_custom_target(Brayns-tests)
set_target_properties(Brayns-tests PROPERTIES
    EXCLUDE_FROM_DEFAULT_BUILD ON FOLDER ${PROJECT_BINARY_DIR}/tests)

add_dependencies(test tests)
add_dependencies(tests Brayns-tests)
add_dependencies(Brayns-tests Brayns-cpptests)

add_dependencies(Brayns-cpptests ${TEST_TARGET_LIST})
