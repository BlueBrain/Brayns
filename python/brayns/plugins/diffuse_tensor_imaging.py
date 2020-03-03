#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2016-2019, Blue Brain Project
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

"""Provides a class that wraps the API exposed by the braynsDTI plug-in"""


class DiffuseTensorImaging:
    """DiffuseTensorImaging is a class that wraps the API exposed by the braynsDTI plug-in"""

    COLOR_SCHEME_NONE = 0
    COLOR_SCHEME_BY_ID = 1
    COLOR_SCHEME_DIRECTIONAL = 2

    def __init__(self, client):
        """Create a new Diffuse Tensor Imaging instance"""
        self._client = client.rockets_client

    def add_streamlines(self, name, streamlines, radius=1.0, opacity=1.0,
                        color_scheme=COLOR_SCHEME_DIRECTIONAL):
        """
        Adds streamlines to the scene. All streamlines are added to a single model

        :param str name: Name of the model
        :param float streamlines: Streamlines
        :param float radius: Radius of the streamlines
        :param float opacity: Opacity of the streamlines
        :param int color_scheme: The scheme to color the streamlines
        :return: Result of the request submission
        :rtype: str
        """
        count = 0
        indices = list()
        vertices = list()
        for points in streamlines:
            indices.append(count)
            count = count + len(points)
            for point in points:
                for coordinate in point:
                    vertices.append(float(coordinate))

        params = dict()
        params['name'] = name
        params['gids'] = list()  # Not used
        params['indices'] = indices
        params['vertices'] = vertices
        params['radius'] = radius
        params['opacity'] = opacity
        params['colorScheme'] = color_scheme
        return self._client.request("add-streamlines", params=params)
