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

from typing import Any, TypeVar

from brayns.utils import Color4

from .renderer import Renderer

T = TypeVar('T', bound=Renderer)


def deserialize_renderer(renderer_type: type[T], message: dict[str, Any]) -> T:
    renderer = renderer_type()
    renderer.samples_per_pixel = message['samples_per_pixel']
    renderer.max_ray_bounces = message['max_ray_bounces']
    renderer.background_color = Color4(*message['background_color'])
    renderer.update_additional_properties(message)
    return renderer
