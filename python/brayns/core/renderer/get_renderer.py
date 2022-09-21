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

from typing import TypeVar

from brayns.network import Instance

from .renderer import Renderer

T = TypeVar('T', bound=Renderer)


def get_renderer(instance: Instance, renderer_type: type[T]) -> T:
    """Retreive the current renderer from an instance.

    Current renderer and given renderer type must be the same.

    Current renderer can be queried using ``get_rendere_name``.

    :param instance: Instance.
    :type instance: Instance
    :param renderer_type: Type of the current renderer.
    :type renderer_type: type[T]
    :return: Current renderer.
    :rtype: T
    """
    name = renderer_type.name
    result = instance.request(f'get-renderer-{name}')
    return renderer_type.from_properties(result)
