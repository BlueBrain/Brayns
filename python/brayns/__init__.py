#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2016-2018, Blue Brain Project
#                          Raphael Dumusc <raphael.dumusc@epfl.ch>
#                          Daniel Nachbaur <daniel.nachbaur@epfl.ch>
#                          Cyrille Favreau <cyrille.favreau@epfl.ch>
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
# All rights reserved. Do not distribute without further notice.

"""The Brayns python client library provides access to remote control a running Brayns instance."""

from .async_client import AsyncClient
from .client import Client
from .plugins.circuit_explorer import CircuitExplorer
from .plugins.diffuse_tensor_imaging import DiffuseTensorImaging
from .helpers.camera_path_handler import CameraPathHandler
from .version import VERSION as __version__

__all__ = ['AsyncClient', 'Client', 'CircuitExplorer', 'DiffuseTensorImaging', 'CameraPathHandler',
           'Fluorescence', 'PBRVolumes', '__version__']
