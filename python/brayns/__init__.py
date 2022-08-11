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
