# Copyright (c) 2015-2022 EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# Responsible Author: adrien.fleury@epfl.ch
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License version 3.0 as published
# by the Free Software Foundation.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

"""
Brayns core functionalities.

Provides high-level objects to wrap the raw JSON-RPC messages sent and received
with an instance of Brayns service.

Include access to camera, renderer, models, materials, snapshots, etc...
"""

from .api import (
    Entrypoint,
    JsonSchema,
    JsonType,
    deserialize_schema,
    get_entrypoint,
    get_entrypoints,
    get_methods,
    serialize_schema,
)
from .application import Application, get_application, set_resolution, stop
from .camera import (
    Camera,
    CameraController,
    CameraRotation,
    OrthographicProjection,
    PerspectiveProjection,
    Projection,
    get_camera,
    get_camera_name,
    get_camera_projection,
    get_camera_view,
    set_camera,
    set_camera_projection,
    set_camera_view,
)
from .clipping import (
    ClippingGeometry,
    ClipPlane,
    add_clipping_geometry,
    clear_clipping_geometries,
)
from .color_ramp import (
    ColorRamp,
    ControlPoint,
    OpacityCurve,
    ValueRange,
    get_color_ramp,
    set_color_ramp,
)
from .coloring import (
    ColorMethod,
    color_model,
    get_color_methods,
    get_color_values,
    set_model_color,
)
from .framebuffer import (
    Framebuffer,
    ProgressiveFramebuffer,
    StaticFramebuffer,
    set_framebuffer,
)
from .geometry import (
    BoundedPlane,
    Box,
    Capsule,
    Geometry,
    Plane,
    Sphere,
    add_geometries,
)
from .light import (
    AmbientLight,
    DirectionalLight,
    Light,
    QuadLight,
    add_light,
    clear_lights,
)
from .loader import Loader, LoaderInfo, MeshLoader, get_loaders
from .material import (
    CarPaintMaterial,
    EmissiveMaterial,
    GlassMaterial,
    Material,
    MatteMaterial,
    MetalMaterial,
    PhongMaterial,
    PlasticMaterial,
    get_material,
    get_material_name,
    set_material,
)
from .model import (
    Model,
    Scene,
    clear_models,
    clear_renderables,
    deserialize_model,
    get_bounds,
    get_model,
    get_models,
    get_scene,
    instantiate_model,
    remove_models,
    update_model,
)
from .pick import PickResult, pick
from .render import Image, ImageInfo, Snapshot
from .renderer import (
    InteractiveRenderer,
    ProductionRenderer,
    Renderer,
    get_renderer,
    get_renderer_name,
    set_renderer,
)
from .simulation import (
    Simulation,
    TimeUnit,
    enable_simulation,
    get_simulation,
    set_simulation_frame,
)
from .version import Version, VersionError, check_version, get_version

__all__ = [
    "add_clipping_geometry",
    "add_geometries",
    "add_light",
    "AmbientLight",
    "Application",
    "BoundedPlane",
    "Box",
    "Camera",
    "CameraController",
    "CameraRotation",
    "Capsule",
    "CarPaintMaterial",
    "check_version",
    "clear_clipping_geometries",
    "clear_lights",
    "clear_models",
    "clear_renderables",
    "ClippingGeometry",
    "ClipPlane",
    "color_model",
    "ColorMethod",
    "ColorRamp",
    "ControlPoint",
    "deserialize_model",
    "deserialize_schema",
    "DirectionalLight",
    "EmissiveMaterial",
    "enable_simulation",
    "Entrypoint",
    "Framebuffer",
    "Geometry",
    "get_application",
    "get_bounds",
    "get_camera_name",
    "get_camera_projection",
    "get_camera_view",
    "get_camera",
    "get_color_methods",
    "get_color_ramp",
    "get_color_values",
    "get_entrypoint",
    "get_entrypoints",
    "get_loaders",
    "get_material_name",
    "get_material",
    "get_methods",
    "get_model",
    "get_models",
    "get_renderer_name",
    "get_renderer",
    "get_scene",
    "get_simulation",
    "get_version",
    "GlassMaterial",
    "Image",
    "ImageInfo",
    "instantiate_model",
    "InteractiveRenderer",
    "JsonSchema",
    "JsonType",
    "Light",
    "Loader",
    "LoaderInfo",
    "Material",
    "MatteMaterial",
    "MeshLoader",
    "MetalMaterial",
    "Model",
    "OpacityCurve",
    "OrthographicProjection",
    "PerspectiveProjection",
    "PhongMaterial",
    "pick",
    "PickResult",
    "Plane",
    "PlasticMaterial",
    "ProductionRenderer",
    "ProgressiveFramebuffer",
    "Projection",
    "QuadLight",
    "remove_models",
    "Renderer",
    "Scene",
    "serialize_schema",
    "set_camera_projection",
    "set_camera_view",
    "set_camera",
    "set_color_ramp",
    "set_framebuffer",
    "set_material",
    "set_model_color",
    "set_renderer",
    "set_resolution",
    "set_simulation_frame",
    "Simulation",
    "Snapshot",
    "Sphere",
    "StaticFramebuffer",
    "stop",
    "TimeUnit",
    "update_model",
    "ValueRange",
    "Version",
    "VersionError",
]
