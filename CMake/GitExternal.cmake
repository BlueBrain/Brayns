# Configures an external git repository
#
# Usage:
#  * Automatically reads, parses and updates a .gitexternals file if it only
#    contains lines in the form "# <directory> <giturl> <gittag>".
#    This function parses the file for this pattern and then calls
#    git_external on each found entry. Additionally it provides an
#    update target to bump the tag to the master revision by
#    recreating .gitexternals.
#  * Provides function
#      git_external(<directory> <giturl> <gittag> [VERBOSE,SHALLOW]
#        [RESET <files>])
#    which will check out directory in CMAKE_SOURCE_DIR (if relative)
#    or in the given absolute path using the given repository and tag
#    (commit-ish).
#
# Options which can be supplied to the function:
#  VERBOSE, when present, this option tells the function to output
#    information about what operations are being performed by git on
#    the repo.
#  SHALLOW, when present, causes a shallow clone of depth 1 to be made
#    of the specified repo. This may save considerable memory/bandwidth
#    when only a specific branch of a repo is required and the full history
#    is not required. Note that the SHALLOW option will only work for a branch
#    or tag and cannot be used for an arbitrary SHA.
#  OPTIONAL, when present, this option makes this operation optional.
#    The function will output a warning and return if the repo could not be
#    cloned.
#
# Targets:
#  * <directory>-rebase: fetches latest updates and rebases the given external
#    git repository onto it.
#  * rebase: Rebases all git externals, including sub projects
#
# Options (global) which control behaviour:
#  COMMON_GIT_EXTERNAL_VERBOSE
#    This is a global option which has the same effect as the VERBOSE option,
#    with the difference that output information will be produced for all
#    external repos when set.
#  GIT_EXTERNAL_TAG
#    If set, git external tags referring to a SHA1 (not a branch) will be
#    overwritten by this value.
#
# CMake or environment variables:
#  GITHUB_USER
#    If set, a remote called 'user' is set up for github repositories, pointing
#    to git@github.com:<user>/<project>. Also, this remote is used by default
#    for 'git push'.


if(NOT GIT_FOUND)
  find_package(Git QUIET)
endif()
if(NOT GIT_EXECUTABLE)
  return()
endif()

include(CMakeParseArguments)
option(COMMON_GIT_EXTERNAL_VERBOSE "Print git commands as they are executed" OFF)

if(NOT GITHUB_USER AND DEFINED ENV{GITHUB_USER})
  set(GITHUB_USER $ENV{GITHUB_USER} CACHE STRING
    "Github user name used to setup remote for 'user' forks")
endif()

macro(GIT_EXTERNAL_MESSAGE msg)
  if(COMMON_GIT_EXTERNAL_VERBOSE)
    message(STATUS "${NAME}: ${msg}")
  endif()
endmacro()

# utility function for printing a list with custom separator
function(JOIN VALUES GLUE OUTPUT)
  string (REGEX REPLACE "([^\\]|^);" "\\1${GLUE}" _TMP_STR "${VALUES}")
  string (REGEX REPLACE "[\\](.)" "\\1" _TMP_STR "${_TMP_STR}") #fixes escaping
  set (${OUTPUT} "${_TMP_STR}" PARENT_SCOPE)
endfunction()

function(GIT_EXTERNAL DIR REPO tag)
  cmake_parse_arguments(GIT_EXTERNAL_LOCAL "VERBOSE;SHALLOW;OPTIONAL" "" "RESET" ${ARGN})
  set(TAG ${tag})
  if(GIT_EXTERNAL_TAG AND "${tag}" MATCHES "^[0-9a-f]+$")
    set(TAG ${GIT_EXTERNAL_TAG})
  endif()

  # check if we had a previous external of the same name
  string(REGEX REPLACE "[:/]" "_" TARGET "${DIR}")
  get_property(OLD_TAG GLOBAL PROPERTY ${TARGET}_GITEXTERNAL_TAG)
  if(OLD_TAG)
    if(NOT OLD_TAG STREQUAL TAG)
      string(REPLACE "${CMAKE_SOURCE_DIR}/" "" PWD
        "${CMAKE_CURRENT_SOURCE_DIR}")
      git_external_message("${DIR}: already configured with ${OLD_TAG}, ignoring requested ${TAG} in ${PWD}")
      return()
    endif()
  else()
    set_property(GLOBAL PROPERTY ${TARGET}_GITEXTERNAL_TAG ${TAG})
  endif()

  if(NOT IS_ABSOLUTE "${DIR}")
    set(DIR "${CMAKE_SOURCE_DIR}/${DIR}")
  endif()
  get_filename_component(NAME "${DIR}" NAME)
  get_filename_component(GIT_EXTERNAL_DIR "${DIR}/.." ABSOLUTE)

  if(NOT EXISTS "${DIR}")
    # clone
    set(_clone_options --recursive)
    if(GIT_EXTERNAL_LOCAL_SHALLOW)
      list(APPEND _clone_options --depth 1 --branch ${TAG})
    else()
      set(_msg_tag "[${TAG}]")
    endif()
    JOIN("${_clone_options}" " " _msg_text)
    message(STATUS "git clone ${_msg_text} ${REPO} ${DIR} ${_msg_tag}")
    execute_process(
      COMMAND "${GIT_EXECUTABLE}" clone ${_clone_options} ${REPO} ${DIR}
      RESULT_VARIABLE nok ERROR_VARIABLE error
      WORKING_DIRECTORY "${GIT_EXTERNAL_DIR}")
    if(nok)
      if(GIT_EXTERNAL_LOCAL_OPTIONAL)
        message(STATUS "${DIR} clone failed: ${error}\n")
        return()
      else()
        message(FATAL_ERROR "${DIR} clone failed: ${error}\n")
      endif()
    endif()

    # checkout requested tag
    if(NOT GIT_EXTERNAL_LOCAL_SHALLOW)
      execute_process(
        COMMAND "${GIT_EXECUTABLE}" checkout -q "${TAG}"
        RESULT_VARIABLE nok ERROR_VARIABLE error
        WORKING_DIRECTORY "${DIR}")
      if(nok)
        message(FATAL_ERROR "git checkout ${TAG} in ${DIR} failed: ${error}\n")
      endif()
    endif()

    # checkout requested tag
    execute_process(
      COMMAND "${GIT_EXECUTABLE}" checkout -q "${TAG}"
      RESULT_VARIABLE nok ERROR_VARIABLE error
      WORKING_DIRECTORY "${DIR}")
    if(nok)
      message(FATAL_ERROR "git checkout ${TAG} in ${DIR} failed: ${error}\n")
    endif()
  endif()

  # set up "user" remote for github forks and make it default for 'git push'
  if(GITHUB_USER AND REPO MATCHES ".*github.com.*")
    string(REGEX REPLACE ".*(github.com)[\\/:]().*(\\/.*)" "git@\\1:\\2${GITHUB_USER}\\3"
      GIT_EXTERNAL_USER_REPO ${REPO})
    execute_process(
      COMMAND "${GIT_EXECUTABLE}" remote add user ${GIT_EXTERNAL_USER_REPO}
      OUTPUT_QUIET ERROR_QUIET WORKING_DIRECTORY "${DIR}")
    execute_process(
      COMMAND "${GIT_EXECUTABLE}" config remote.pushdefault user
      OUTPUT_QUIET ERROR_QUIET WORKING_DIRECTORY "${DIR}")
  endif()

  if(COMMON_SOURCE_DIR)
    file(RELATIVE_PATH __dir ${COMMON_SOURCE_DIR} ${DIR})
  else()
    file(RELATIVE_PATH __dir ${CMAKE_SOURCE_DIR} ${DIR})
  endif()
  string(REGEX REPLACE "[:/\\.]" "-" __target "${__dir}")
  if(TARGET ${__target}-rebase)
    return()
  endif()

  set(__rebase_cmake "${CMAKE_CURRENT_BINARY_DIR}/${__target}-rebase.cmake")
  file(WRITE ${__rebase_cmake}
    "if(NOT IS_DIRECTORY \"${DIR}/.git\")\n"
    "  message(FATAL_ERROR \"Can't update git external ${__dir}: Not a git repository\")\n"
    "endif()\n"
    # check if we are already on the requested tag (nothing to do)
    "execute_process(COMMAND \"${GIT_EXECUTABLE}\" rev-parse --short HEAD\n"
    "  OUTPUT_VARIABLE currentref OUTPUT_STRIP_TRAILING_WHITESPACE\n"
    "  WORKING_DIRECTORY \"${DIR}\")\n"
    "if(currentref STREQUAL ${TAG}) # nothing to do\n"
    "  return()\n"
    "endif()\n"
    "\n"
    # reset generated files
    "foreach(GIT_EXTERNAL_RESET_FILE ${GIT_EXTERNAL_RESET})\n"
    "  execute_process(\n"
    "    COMMAND \"${GIT_EXECUTABLE}\" reset -q \"\${GIT_EXTERNAL_RESET_FILE}\"\n"
    "    ERROR_QUIET OUTPUT_QUIET\n"
    "    WORKING_DIRECTORY \"${DIR}\")\n"
    "  execute_process(\n"
    "    COMMAND \"${GIT_EXECUTABLE}\" checkout -q -- \"${GIT_EXTERNAL_RESET_FILE}\"\n"
    "    ERROR_QUIET OUTPUT_QUIET\n"
    "    WORKING_DIRECTORY \"${DIR}\")\n"
    "endforeach()\n"
    "\n"
    # fetch updates
    "execute_process(COMMAND \"${GIT_EXECUTABLE}\" fetch origin -q\n"
    "  RESULT_VARIABLE nok ERROR_VARIABLE error\n"
    "  WORKING_DIRECTORY \"${DIR}\")\n"
    "if(nok)\n"
    "  message(FATAL_ERROR \"Fetch for ${__dir} failed:\n   \${error}\")\n"
    "endif()\n"
    "\n"
  )
  if("${TAG}" MATCHES "^[0-9a-f]+$")
    # requested TAG is a SHA1, just switch to it
    file(APPEND ${__rebase_cmake}
      # checkout requested tag
      "execute_process(\n"
      "  COMMAND \"${GIT_EXECUTABLE}\" checkout -q \"${TAG}\"\n"
      "  RESULT_VARIABLE nok ERROR_VARIABLE error\n"
      "  WORKING_DIRECTORY \"${DIR}\")\n"
      "if(nok)\n"
      "  message(FATAL_ERROR \"git checkout ${TAG} in ${__dir} failed: ${error}\n\")\n"
      "endif()\n"
    )
  else()
    # requested TAG is a branch
    file(APPEND ${__rebase_cmake}
      # switch to requested branch
      "execute_process(\n"
      "  COMMAND \"${GIT_EXECUTABLE}\" checkout -q \"${TAG}\"\n"
      "  OUTPUT_QUIET ERROR_QUIET WORKING_DIRECTORY \"${DIR}\")\n"
      # try to rebase it
      "execute_process(COMMAND \"${GIT_EXECUTABLE}\" rebase FETCH_HEAD\n"
      "  RESULT_VARIABLE nok ERROR_VARIABLE error OUTPUT_VARIABLE output\n"
      "  WORKING_DIRECTORY \"${DIR}\")\n"
      "if(nok)\n"
      "  execute_process(COMMAND \"${GIT_EXECUTABLE}\" rebase --abort\n"
      "    WORKING_DIRECTORY \"${DIR}\" ERROR_QUIET OUTPUT_QUIET)\n"
      "  message(FATAL_ERROR \"Rebase ${__dir} failed:\n\${output}\${error}\")\n"
      "endif()\n"
    )
  endif()

  if(NOT GIT_EXTERNAL_SCRIPT_MODE)
    add_custom_target(${__target}-rebase
      COMMAND ${CMAKE_COMMAND} -P ${__rebase_cmake}
      COMMENT "Rebasing ${__dir} [${TAG}]")
    set_target_properties(${__target}-rebase PROPERTIES
      EXCLUDE_FROM_DEFAULT_BUILD ON FOLDER git)
    if(NOT TARGET rebase)
      add_custom_target(rebase)
      set_target_properties(rebase PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD ON)
    endif()
    add_dependencies(rebase ${__target}-rebase)
  endif()
endfunction()

set(GIT_EXTERNALS ${GIT_EXTERNALS_FILE})
if(NOT GIT_EXTERNALS)
  set(GIT_EXTERNALS "${CMAKE_CURRENT_SOURCE_DIR}/.gitexternals")
endif()

if(EXISTS ${GIT_EXTERNALS} AND NOT GIT_EXTERNAL_SCRIPT_MODE)
  include(${GIT_EXTERNALS})
  file(READ ${GIT_EXTERNALS} GIT_EXTERNAL_FILE)
  string(REGEX REPLACE "\n" ";" GIT_EXTERNAL_FILE "${GIT_EXTERNAL_FILE}")
  foreach(LINE ${GIT_EXTERNAL_FILE})
    if(NOT LINE MATCHES "^#.*$")
      message(FATAL_ERROR "${GIT_EXTERNALS} contains non-comment line: ${LINE}")
    endif()
    string(REGEX REPLACE "^#[ ]*(.+[ ]+.+[ ]+.+)$" "\\1" DATA ${LINE})
    if(NOT LINE STREQUAL DATA)
      string(REGEX REPLACE "[ ]+" ";" DATA "${DATA}")
      list(LENGTH DATA DATA_LENGTH)
      if(DATA_LENGTH EQUAL 3)
        list(GET DATA 0 DIR)
        list(GET DATA 1 REPO)
        list(GET DATA 2 TAG)

        # Create a unique, flat name
        string(REPLACE "/" "-" GIT_EXTERNAL_NAME ${DIR}_${PROJECT_NAME})

        if(NOT TARGET update-gitexternal-${GIT_EXTERNAL_NAME}) # not done
          # pull in identified external
          git_external(${DIR} ${REPO} ${TAG})

          # Create update script and target to bump external spec
          if(NOT TARGET update)
            add_custom_target(update)
          endif()
          if(NOT TARGET update-gitexternal)
            add_custom_target(update-gitexternal)
            add_custom_target(flatten-gitexternal)
            add_dependencies(update update-gitexternal)
          endif()
          if(NOT TARGET ${PROJECT_NAME}-flatten-gitexternal)
            add_custom_target(${PROJECT_NAME}-flatten-gitexternal)
          endif()

          # Create a unique, flat name
          file(RELATIVE_PATH GIT_EXTERNALS_BASE ${CMAKE_SOURCE_DIR}
            ${GIT_EXTERNALS})
          string(REPLACE "/" "_" GIT_EXTERNAL_TARGET ${GIT_EXTERNALS_BASE})

          set(GIT_EXTERNAL_TARGET update-gitexternal-${GIT_EXTERNAL_TARGET})
          if(NOT TARGET ${GIT_EXTERNAL_TARGET})
            set(GIT_EXTERNAL_SCRIPT
              "${CMAKE_CURRENT_BINARY_DIR}/${GIT_EXTERNAL_TARGET}.cmake")
            file(WRITE "${GIT_EXTERNAL_SCRIPT}"
              "file(WRITE ${GIT_EXTERNALS} \"# -*- mode: cmake -*-\n\")\n")
            add_custom_target(${GIT_EXTERNAL_TARGET}
              COMMAND "${CMAKE_COMMAND}" -DGIT_EXTERNAL_SCRIPT_MODE=1 -P ${GIT_EXTERNAL_SCRIPT}
              COMMENT "Recreate ${GIT_EXTERNALS_BASE}"
              WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
          endif()

          set(GIT_EXTERNAL_SCRIPT
            "${CMAKE_CURRENT_BINARY_DIR}/gitupdate${GIT_EXTERNAL_NAME}.cmake")
          file(WRITE "${GIT_EXTERNAL_SCRIPT}" "
include(\"${CMAKE_CURRENT_LIST_DIR}/GitExternal.cmake\")
execute_process(COMMAND \"${GIT_EXECUTABLE}\" fetch origin -q
  WORKING_DIRECTORY \"${CMAKE_SOURCE_DIR}/${DIR}\")
execute_process(
  COMMAND \"${GIT_EXECUTABLE}\" show-ref --hash=7 refs/remotes/origin/master
  OUTPUT_VARIABLE newref OUTPUT_STRIP_TRAILING_WHITESPACE
  WORKING_DIRECTORY \"${CMAKE_SOURCE_DIR}/${DIR}\")
if(newref)
  file(APPEND ${GIT_EXTERNALS} \"# ${DIR} ${REPO} \${newref}\\n\")
  git_external(${DIR} ${REPO} \${newref})
else()
  file(APPEND ${GIT_EXTERNALS} \"# ${DIR} ${REPO} ${TAG}\n\")
endif()")
          add_custom_target(update-gitexternal-${GIT_EXTERNAL_NAME}
            COMMAND "${CMAKE_COMMAND}" -DGIT_EXTERNAL_SCRIPT_MODE=1 -P ${GIT_EXTERNAL_SCRIPT}
            COMMENT "Update ${REPO} in ${GIT_EXTERNALS_BASE}"
            DEPENDS ${GIT_EXTERNAL_TARGET}
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
          add_dependencies(update-gitexternal
            update-gitexternal-${GIT_EXTERNAL_NAME})

          # Flattens a git external repository into its parent repo:
          # * Clean any changes from external
          # * Unlink external from git: Remove external/.git and .gitexternals
          # * Add external directory to parent
          # * Commit with flattened repo and tag info
          # - Depend on release branch checked out
          add_custom_target(flatten-gitexternal-${GIT_EXTERNAL_NAME}
            COMMAND "${GIT_EXECUTABLE}" clean -dfx
            COMMAND "${CMAKE_COMMAND}" -E remove_directory .git
            COMMAND "${CMAKE_COMMAND}" -E remove -f "${CMAKE_CURRENT_SOURCE_DIR}/.gitexternals"
            COMMAND "${GIT_EXECUTABLE}" add -f .
            COMMAND "${GIT_EXECUTABLE}" commit -m "Flatten ${REPO} into ${DIR} at ${TAG}" . "${CMAKE_CURRENT_SOURCE_DIR}/.gitexternals"
            COMMENT "Flatten ${REPO} into ${DIR}"
            DEPENDS ${PROJECT_NAME}-make-branch
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${DIR}")
          add_dependencies(flatten-gitexternal
            flatten-gitexternal-${GIT_EXTERNAL_NAME})
          add_dependencies(${PROJECT_NAME}-flatten-gitexternal
            flatten-gitexternal-${GIT_EXTERNAL_NAME})

          foreach(_target flatten-gitexternal-${GIT_EXTERNAL_NAME} ${PROJECT_NAME}-flatten-gitexternal flatten-gitexternal update-gitexternal-${GIT_EXTERNAL_NAME} ${GIT_EXTERNAL_TARGET} update-gitexternal update)
            set_target_properties(${_target} PROPERTIES
              EXCLUDE_FROM_DEFAULT_BUILD ON FOLDER git)
          endforeach()
        endif()
      endif()
    endif()
  endforeach()
endif()
