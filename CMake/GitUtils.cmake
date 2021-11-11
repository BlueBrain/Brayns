# Copyright (c) 2015-2021, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

# FROM https://jonathanhamberg.com/post/cmake-embedding-git-hash/
# Get the latest abbreviated commit hash of the working branch
function (get_commit_hash OUTPUT_VAR)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} log -1 --format=%h
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(${OUTPUT_VAR} ${COMMIT_HASH} PARENT_SCOPE)
endfunction()


function (initialize_submodule SUBMODULE_PATH)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive --depth 1 ${SUBMODULE_PATH}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_QUIET
    )
endfunction()
