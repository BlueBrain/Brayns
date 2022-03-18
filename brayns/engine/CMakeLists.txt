# Copyright (c) 2015-2022, EPFL/Blue Brain Project
# Responsible Authors: Daniel Nachbaur <daniel.nachbaur@epfl.ch>
#                      Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSENGINE_SOURCES
    Camera.cpp
    Engine.cpp
    FrameBuffer.cpp
    FrameRenderer.cpp
    Light.cpp
    Material.cpp
    Model.cpp
    ModelComponents.cpp
    ModelGroup.cpp
    ModelInstance.cpp
    Renderer.cpp
    Scene.cpp
    cameras/OrthographicCamera.cpp
    cameras/PerspectiveCamera.cpp
    defaultcomponents/MaterialComponent.cpp
    defaultcomponents/TransferFunctionComponent.cpp
    defaultcomponents/TransferFunctionRendererComponent.cpp
    geometries/Box.cpp
    geometries/Plane.cpp
    geometries/Primitive.cpp
    geometries/Sphere.cpp
    geometries/TriangleMesh.cpp
    lights/AmbientLight.cpp
    lights/DirectionalLight.cpp
    lights/QuadLight.cpp
    materials/CarPaintMaterial.cpp
    materials/DefaultMaterial.cpp
    materials/EmissiveMaterial.cpp
    materials/GlassMaterial.cpp
    materials/MatteMaterial.cpp
    materials/MetalMaterial.cpp
    materials/PlasticMaterial.cpp
    renderers/InteractiveRenderer.cpp
    renderers/ProductionRenderer.cpp
    scenecomponents/SceneClipManager.cpp
    scenecomponents/SceneLightManager.cpp
    scenecomponents/SceneModelManager.cpp
)

set(BRAYNSENGINE_PUBLIC_HEADERS
    Camera.h
    Engine.h
    FrameBuffer.h
    FrameRenderer.h
    Light.h
    Material.h
    Model.h
    ModelComponents.h
    ModelGroup.h
    ModelInstance.h
    Renderer.h
    RenderableType.h
    Scene.h
    cameras/OrthographicCamera.h
    cameras/PerspectiveCamera.h
    defaultcomponents/ClippingComponent.h
    defaultcomponents/GeometryRendererComponent.h
    defaultcomponents/MaterialComponent.h
    defaultcomponents/TransferFunctionComponent.h
    defaultcomponents/TransferFunctionRendererComponent.h
    defaultcomponents/VolumeRendererComponent.h
    geometries/Box.h
    geometries/Plane.h
    geometries/Primitive.h
    geometries/Sphere.h
    lights/AmbientLight.h
    lights/DirectionalLight.h
    lights/QuadLight.h
    materials/CarPaintMaterial.h
    materials/DefaultMaterial.h
    materials/EmissiveMaterial.h
    materials/GlassMaterial.h
    materials/MatteMaterial.h
    materials/MetalMaterial.h
    materials/PlasticMaterial.h
    renderers/InteractiveRenderer.h
    scenecomponents/SceneClipManager.h
    scenecomponents/SceneLightManager.h
    scenecomponents/SceneModelManager.h
)

add_library(braynsEngine SHARED ${BRAYNSENGINE_SOURCES} ${BRAYNSENGINE_PUBLIC_HEADERS})

target_include_directories(braynsEngine PUBLIC
  "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
  "$<INSTALL_INTERFACE:include>")

target_link_libraries(braynsEngine PUBLIC
    braynsParameters braynsCommon ospray::ospray)

# No strict aliassing for ospray common -> ospray and ospray -> ispc type casting
target_compile_options(braynsEngine PRIVATE ${BRAYNS_COMPILE_OPTIONS}) # -Wno-strict-aliasing)

set_target_properties(braynsEngine
    PROPERTIES
        VERSION ${BRAYNS_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

install(TARGETS braynsEngine
    EXPORT Brayns-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
