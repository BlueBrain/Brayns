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

from brayns.network import Instance

from ..model import Model, deserialize_model
from .clipping_geometry import ClippingGeometry


def add_clipping_geometry(instance: Instance, geometry: ClippingGeometry) -> Model:
    """Add a clipping geometry to the given instance and return its model.

    :param instance: Instance.
    :type instance: Instance
    :param geometry: Clipping geometry to add.
    :type geometry: ClippingGeometry
    :return: Clipping model.
    :rtype: Model
    """
    method = geometry.method
    params = geometry.get_properties()
    result = instance.request(method, params)
    return deserialize_model(result)
