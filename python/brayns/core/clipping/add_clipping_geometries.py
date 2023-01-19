# Copyright (c) 2015-2023 EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

from typing import TypeVar

from brayns.network import Instance

from ..model import Model, deserialize_model
from .clipping_geometry import ClippingGeometry

T = TypeVar("T", bound=ClippingGeometry)


def add_clipping_geometries(instance: Instance, cliping_geometries: list[T]) -> Model:
    """Create a model from a list of clipping geometries.

    All geometries must have the same type.

    Model witout geometries are not supported.

    :param instance: Instance.
    :type instance: Instance
    :param cliping_geometries: Clipping geometries to add (boxes, capsules, etc...).
    :type cliping_geometries: list[T]
    :raises ValueError: List is empty.
    :return: Model created from the geometries.
    :rtype: Model
    """
    if not cliping_geometries:
        raise ValueError("Cannot create a model with no clipping geometries")
    method = cliping_geometries[0].method
    params = [geometry.get_properties() for geometry in cliping_geometries]
    result = instance.request(method, params)
    return deserialize_model(result)
