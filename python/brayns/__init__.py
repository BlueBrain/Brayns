# Copyright 2015-2024 Blue Brain Project/EPFL
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

"""Brayns Python module to connect to a renderer.

Example:
.. code-block: python
    import brayns

    with brayns.connect(
        uri='localhost:5000',  # Renderer URI
        secure=True,  # Enable SSL, defaults to False
        cafile='server.pem' # Custom CA if server certificate is self-signed
    ) as client:

        # Raw request with JSON-RPC method and params (defaults to None)
        registry = client.request('registry')
        schema = client.request('schema', {'endpoint': 'get-camera'})

        # Request using auto generated API (here entrypoint get-camera)
        camera = client.get_camera()
"""

from .client.client import Client
from .client.connect import connect
from .client.reply_error import ReplyError
from .doc.doc_builder import build_rst_doc, save_rst_doc
from .utils.camera_path_handler import CameraPathHandler

__all__ = [
    'connect',
    'build_rst_doc',
    'save_rst_doc',
    'Client',
    'CameraPathHandler',
    'ReplyError'
]
