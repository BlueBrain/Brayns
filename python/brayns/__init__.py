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
Brayns Python package.

This package provides a high level API to interact with an instance of Brayns
instance through websockets.

The low level JSON-RPC API is also available using the instance directly.
"""

from .core import *
from .movie import *
from .network import *
from .plugins import *
from .service import *
from .utils import *
from .version import __version__

__all__ = [
    'add_clipping_geometry',
    'add_geometries',
    'add_light',
    'AmbientLight',
    'Application',
    'BbpCells',
    'BbpLoader',
    'BbpReport',
    'BbpReportType',
    'Bounds',
    'Box',
    'Camera',
    'Capsule',
    'CarPaintMaterial',
    'CellId',
    'check_version',
    'clear_clipping_geometries',
    'clear_lights',
    'clear_models',
    'ClippingGeometry',
    'ClipPlane',
    'color_circuit_by_id',
    'color_circuit_by_method',
    'color_circuit',
    'Color3',
    'Color4',
    'ColorMethod',
    'ColorRamp',
    'ConnectionClosedError',
    'Connector',
    'ControlPoint',
    'CylindricCamera',
    'DefaultMaterial',
    'DirectionalLight',
    'EmissiveMaterial',
    'enable_simulation',
    'Entrypoint',
    'Error',
    'Fovy',
    'FrameExporter',
    'Geometry',
    'GeometryType',
    'get_application',
    'get_bounds',
    'get_camera_name',
    'get_camera_view',
    'get_camera',
    'get_color_method_values',
    'get_color_methods',
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
    'ImageFormat',
    'Instance',
    'InteractiveRenderer',
    'InvalidServerCertificateError',
    'JsonRpcRequest',
    'JsonSchema',
    'JsonType',
    'KeyFrame',
    'Light',
    'Loader',
    'LoaderInfo',
    'Logger',
    'LogLevel',
    'lower_bound',
    'Manager',
    'Material',
    'MatteMaterial',
    'merge_bounds',
    'MeshLoader',
    'MetalMaterial',
    'Model',
    'Morphology',
    'MorphologyLoader',
    'Movie',
    'MovieError',
    'MovieFrames',
    'OpacityCurve',
    'OrthographicCamera',
    'parse_hex_color',
    'parse_image_format',
    'PerspectiveCamera',
    'Plane',
    'PlasticMaterial',
    'Plugin',
    'Process',
    'ProductionRenderer',
    'ProtocolError',
    'QuadLight',
    'Quaternion',
    'remove_clipping_geometries',
    'remove_lights',
    'remove_models',
    'Renderer',
    'RequestError',
    'RequestFuture',
    'RequestProgress',
    'Resolution',
    'Rotation',
    'Scene',
    'Service',
    'ServiceUnavailableError',
    'set_camera_view',
    'set_camera',
    'set_color_ramp',
    'set_material',
    'set_renderer',
    'set_simulation_frame',
    'Simulation',
    'Snapshot',
    'SonataEdgePopulation',
    'SonataLoader',
    'SonataNodePopulation',
    'SonataNodes',
    'SonataReport',
    'SonataReportType',
    'Sphere',
    'SslClientContext',
    'SslServerContext',
    'start',
    'TimeUnit',
    'Transform',
    'update_application',
    'update_model',
    'upper_bound',
    'ValueRange',
    'Vector3',
    'Version',
    'VersionError',
    'View',
    'WebSocketError',
]
