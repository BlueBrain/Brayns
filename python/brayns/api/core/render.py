# Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

from dataclasses import dataclass
from typing import TypedDict, Unpack

from brayns.network.connection import Connection
from brayns.utils.composing import serialize
from brayns.utils.parsing import deserialize

from .camera import Camera
from .framebuffer import Framebuffer
from .renderer import Renderer
from .scene import World


class RenderParams(TypedDict):
    framebuffer: Framebuffer
    renderer: Renderer
    camera: Camera
    world: World


@dataclass
class RenderResult:
    duration: float


async def render(connection: Connection, **settings: Unpack[RenderParams]) -> RenderResult:
    response = await connection.get_result("render", serialize(settings))
    return deserialize(response, RenderResult)
