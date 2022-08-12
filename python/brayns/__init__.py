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

Basic example (take a snapshot):

.. code-block:: python

    import brayns

    # Connection settings (at least server URI)
    connector = brayns.Connector('localhost:5000')

    # Connect and use context manager to close it once it is done
    with connector.connect() as instance:

        # Check that the package is compatible with the instance
        brayns.check_version(instance)

        # load some mesh and get models (here there will be only one)
        loader = brayns.MeshLoader()
        models = loader.load(instance, 'cube.ply')
        model = models[0]

        # Choose a camera
        camera = brayns.PerspectiveCamera()

        # Use it to compute a camera position to see the whole model
        view = camera.get_full_screen_view(model.bounds)

        # Put some light to see the model (in camera direction)
        light = brayns.DirectionalLight(direction=view.direction)
        brayns.add_light(instance, light)

        # Take a snapshot and save it locally (download it)
        snapshot = brayns.Snapshot(
            resolution=brayns.Resolution.full_hd,
            camera=camera,
            view=view,
        )
        snapshot.save(instance, 'snapshot.png')
"""

from .core import *
from .error import Error
from .instance import *
from .launcher import *
from .plugins import *
from .version import __version__

__all__ = [
    'AmbientLight',
    'Application',
    'BbpCells',
    'BbpLoader',
    'BbpReport',
    'Bounds',
    'Box',
    'Camera',
    'Capsule',
    'CarPaintMaterial',
    'CellId',
    'ClipPlane',
    'Color3',
    'Color4',
    'ColorMethod',
    'ConnectionClosedError',
    'Connector',
    'ControlPoint',
    'CylindricCamera',
    'DefaultMaterial',
    'DirectionalLight',
    'EmissiveMaterial',
    'Entrypoint',
    'Error',
    'Fovy',
    'FrameExporter',
    'Geometry',
    'GeometryType',
    'GlassMaterial',
    'ImageFormat',
    'Instance',
    'InteractiveRenderer',
    'InvalidServerCertificateError',
    'JsonRpcRequest',
    'JsonSchema',
    'JsonType',
    'KeyFrame',
    'Launcher',
    'Light',
    'Loader',
    'LoaderInfo',
    'LogLevel',
    'Logger',
    'Material',
    'MatteMaterial',
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
    'PerspectiveCamera',
    'Plane',
    'PlasticMaterial',
    'Plugin',
    'Process',
    'ProductionRenderer',
    'ProtocolError',
    'QuadLight',
    'Quaternion',
    'Renderer',
    'RequestError',
    'RequestFuture',
    'RequestProgress',
    'Resolution',
    'Rotation',
    'ServiceUnavailableError',
    'Simulation',
    'Snapshot',
    'SonataEdgePopulation',
    'SonataLoader',
    'SonataNodePopulation',
    'SonataNodes',
    'SonataReport',
    'Sphere',
    'SslClientContext',
    'SslServerContext',
    'TimeUnit',
    'TransferFunction',
    'Transform',
    'ValueRange',
    'Vector',
    'Vector3',
    'Version',
    'VersionError',
    'View',
    'WebSocketError',
    'add_clip_plane',
    'add_geometries',
    'add_light',
    'check_version',
    'clear_clip_planes',
    'clear_lights',
    'clear_models',
    'color_circuit',
    'color_circuit_by_id',
    'color_circuit_by_method',
    'enable_simulation',
    'get_application',
    'get_bounds',
    'get_camera',
    'get_camera_name',
    'get_camera_view',
    'get_color_method_values',
    'get_color_methods',
    'get_entrypoint',
    'get_entrypoints',
    'get_loaders',
    'get_material',
    'get_material_name',
    'get_methods',
    'get_model',
    'get_models',
    'get_renderer',
    'get_renderer_name',
    'get_simulation',
    'get_transfer_function',
    'get_version',
    'parse_hex_color',
    'parse_image_format',
    'remove_clip_planes',
    'remove_lights',
    'remove_models',
    'set_camera',
    'set_camera_view',
    'set_material',
    'set_renderer',
    'set_simulation_frame',
    'set_transfer_function',
    'update_application',
    'update_model',
]
