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

from .api import *
from .application import *
from .camera import *
from .clipping import *
from .color_ramp import *
from .geometry import *
from .light import *
from .loader import *
from .material import *
from .model import *
from .renderer import *
from .simulation import *
from .snapshot import *
from .version import *

__all__ = [
    'add_clipping_geometry',
    'add_geometries',
    'add_light',
    'AmbientLight',
    'Application',
    'Box',
    'Camera',
    'Capsule',
    'CarPaintMaterial',
    'check_version',
    'clear_clipping_geometries',
    'clear_lights',
    'clear_models',
    'ClippingGeometry',
    'ClipPlane',
    'ColorRamp',
    'ControlPoint',
    'DefaultMaterial',
    'DirectionalLight',
    'EmissiveMaterial',
    'enable_simulation',
    'Entrypoint',
    'FrameExporter',
    'Geometry',
    'get_application',
    'get_bounds',
    'get_camera_name',
    'get_camera_projection',
    'get_camera_view',
    'get_camera',
    'get_color_ramp',
    'get_entrypoint',
    'get_entrypoints',
    'get_loaders',
    'get_material_name',
    'get_material',
    'get_methods',
    'get_model',
    'get_models',
    'get_renderer_name',
    'get_renderer',
    'get_scene',
    'get_simulation',
    'get_version',
    'GlassMaterial',
    'InteractiveRenderer',
    'JsonSchema',
    'JsonType',
    'KeyFrame',
    'Light',
    'Loader',
    'LoaderInfo',
    'look_at',
    'Material',
    'MatteMaterial',
    'MeshLoader',
    'MetalMaterial',
    'Model',
    'OpacityCurve',
    'OrthographicProjection',
    'PerspectiveProjection',
    'Plane',
    'PlasticMaterial',
    'ProductionRenderer',
    'Projection',
    'QuadLight',
    'remove_models',
    'Renderer',
    'Scene',
    'set_camera_projection',
    'set_camera_view',
    'set_camera',
    'set_color_ramp',
    'set_material',
    'set_renderer',
    'set_simulation_frame',
    'Simulation',
    'Snapshot',
    'Sphere',
    'TimeUnit',
    'update_application',
    'update_model',
    'ValueRange',
    'Version',
    'VersionError',
]
