# Copyright 2015-2024 Blue Brain Project/EPFL
# Responsible Authors: Cyrille Favreau <cyrille.favreau@epfl.ch>
#                      Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSCOMMON_SOURCES
    Timer.cpp
    material/Texture2D.cpp
    propertymap/ConversionRegistry.cpp
    scene/ClipPlane.cpp
    simulation/AbstractSimulationHandler.cpp
    transferFunction/TransferFunction.cpp
)

set(BRAYNSCOMMON_PUBLIC_HEADERS
    BaseObject.h
    Blob.h
    DefaultMaterialIds.h
    Log.h
    MaterialsColorMap.h
    MathTypes.h
    PixelFormat.h
    Progress.h
    PropertyObject.h
    Statistics.h
    TextureType.h
    Timer.h
    Transformation.h
    VolumeDataType.h
    adapters/PropertyMapAdapter.h
    adapters/VolumeDataTypeAdapter.h
    geometry/CommonDefines.h
    geometry/Cone.h
    geometry/Cylinder.h
    geometry/SDFGeometry.h
    geometry/Sphere.h
    geometry/Streamline.h
    geometry/TriangleMesh.h
    material/Texture2D.h
    propertymap/Any.h
    propertymap/Converter.h
    propertymap/ConversionRegistry.h
    propertymap/EnumProperty.h
    propertymap/PropertyMap.h
    scene/ClipPlane.h
    simulation/AbstractSimulationHandler.h
    tasks/Task.h
    tasks/TaskFunctor.h
    tasks/TaskRuntimeError.h
    transferFunction/TransferFunction.h
)

add_library(braynsCommon SHARED ${BRAYNSCOMMON_PUBLIC_HEADERS} ${BRAYNSCOMMON_SOURCES})

target_include_directories(braynsCommon PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:include>")

target_link_libraries(braynsCommon PUBLIC
  glm Async++ braynsUtils braynsJson spdlog::spdlog
)
target_link_libraries(braynsCommon PRIVATE
    braynsParameters ${Boost_PROGRAM_OPTIONS_LIBRARY}
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
