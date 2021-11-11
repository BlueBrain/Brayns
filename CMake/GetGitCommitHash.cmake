# FROM https://jonathanhamberg.com/post/cmake-embedding-git-hash/
# Get the latest abbreviated commit hash of the working branch

execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    OUTPUT_VARIABLE BRAYNS_GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
