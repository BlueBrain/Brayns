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

from brayns.instance import Instance

from .bounds import Bounds


def get_bounds(instance: Instance) -> Bounds:
    """Retreive the scene boundary of a renderer instance.

    The scene boundaries are computed from all existing lights and models
    in the given instance.

    :param instance: Renderer instance.
    :type instance: Instance
    :return: Bounds of the current scene.
    :rtype: Bounds
    """
    result = instance.request('get-scene')
    return Bounds.deserialize(result['bounds'])
