# Copyright (c) 2015-2021, EPFL/Blue Brain Project
# Responsible Authors: Cyrille Favreau <cyrille.favreau@epfl.ch>
#                      Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSCOMMON_SOURCES
  ImageManager.cpp
  propertymap/ConversionRegistry.cpp
  light/Light.cpp
  loader/LoaderRegistry.cpp
  material/Texture2D.cpp
  scene/ClipPlane.cpp
  simulation/AbstractSimulationHandler.cpp
  transferFunction/TransferFunction.cpp
  utils/ImageGenerator.cpp
  Timer.cpp
)

set(BRAYNSCOMMON_PUBLIC_HEADERS
  adapters/PropertyMapAdapter.h
  BaseObject.h
  ImageManager.h
  Progress.h
  propertymap/Any.h
  propertymap/Converter.h
  propertymap/ConversionRegistry.h
  propertymap/EnumProperty.h
  propertymap/PropertyMap.h
  PropertyObject.h
  Statistics.h
  Timer.h
  Transformation.h
  geometry/CommonDefines.h
  geometry/Cone.h
  geometry/Cylinder.h
  geometry/SDFGeometry.h
  geometry/SDFBezier.h
  geometry/Sphere.h
  geometry/Streamline.h
  geometry/TriangleMesh.h
  light/Light.h
  loader/Loader.h
  loader/LoaderRegistry.h
  Log.h
  material/Texture2D.h
  mathTypes.h
  scene/ClipPlane.h
  simulation/AbstractSimulationHandler.h
  tasks/Task.h
  tasks/TaskFunctor.h
  tasks/TaskRuntimeError.h
  transferFunction/TransferFunction.h
  types.h
  utils/ImageGenerator.h
)

add_library(braynsCommon SHARED ${BRAYNSCOMMON_PUBLIC_HEADERS} ${BRAYNSCOMMON_SOURCES})

target_include_directories(braynsCommon SYSTEM PRIVATE ${FREEIMAGE_INCLUDE_DIRS})
target_include_directories(braynsCommon PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:include>")

target_link_libraries(braynsCommon PUBLIC
  glm Async++ ${LibJpegTurbo_LIBRARIES} braynsUtils braynsJson spdlog::spdlog
)
target_link_libraries(braynsCommon PRIVATE
    braynsParameters ${Boost_PROGRAM_OPTIONS_LIBRARY} ${FREEIMAGE_LIBRARIES}
)

target_compile_options(braynsCommon PRIVATE ${BRAYNS_COMPILE_OPTIONS})

set_target_properties(braynsCommon
    PROPERTIES
        VERSION ${BRAYNS_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

install(TARGETS braynsCommon
    EXPORT Brayns-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
