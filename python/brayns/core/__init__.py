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
from .geometry import *
from .light import *
from .loader import *
from .material import *
from .model import *
from .renderer import *
from .simulation import *
from .snapshot import *
from .transfer_function import *
from .version import *

__all__ = [
    'add_clip_plane',
    'add_geometries',
    'add_light',
    'AmbientLight',
    'Application',
    'Box',
    'Camera',
    'Capsule',
    'CarPaintMaterial',
    'check_version',
    'clear_clip_planes',
    'clear_lights',
    'clear_models',
    'ClipPlane',
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
    'get_camera_view',
    'get_camera',
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
    'get_transfer_function',
    'get_version',
    'GlassMaterial',
    'InteractiveRenderer',
    'JsonSchema',
    'JsonType',
    'KeyFrame',
    'Light',
    'Loader',
    'LoaderInfo',
    'Material',
    'MatteMaterial',
    'MeshLoader',
    'MetalMaterial',
    'Model',
    'OpacityCurve',
    'OrthographicCamera',
    'PerspectiveCamera',
    'Plane',
    'PlasticMaterial',
    'ProductionRenderer',
    'QuadLight',
    'remove_clip_planes',
    'remove_lights',
    'remove_models',
    'Renderer',
    'Scene',
    'set_camera_view',
    'set_camera',
    'set_material',
    'set_renderer',
    'set_simulation_frame',
    'set_transfer_function',
    'Simulation',
    'Snapshot',
    'Sphere',
    'TimeUnit',
    'TransferFunction',
    'update_application',
    'update_model',
    'ValueRange',
    'Version',
    'VersionError',
]
