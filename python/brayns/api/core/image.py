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

from enum import Enum

from brayns.network.connection import Connection

from .framebuffer import Framebuffer, FramebufferChannel


async def read_framebuffer(connection: Connection, framebuffer: Framebuffer, channel: FramebufferChannel) -> bytes:
    params = {"id": framebuffer.id, "settings": {"channel": channel.value}}
    response = await connection.request("readFramebuffer", params)
    return response.binary


class JpegChannel(Enum):
    COLOR = FramebufferChannel.COLOR.value
    ALBEDO = FramebufferChannel.ALBEDO.value


async def read_framebuffer_as_jpeg(
    connection: Connection, framebuffer: Framebuffer, channel: JpegChannel, quality: int = 100
) -> bytes:
    params = {"id": framebuffer.id, "settings": {"channel": channel.value, "settings": {"quality": quality}}}
    response = await connection.request("readFramebufferAsJpeg", params)
    return response.binary


PngChannel = JpegChannel


async def read_framebuffer_as_png(connection: Connection, framebuffer: Framebuffer, channel: PngChannel) -> bytes:
    params = {"id": framebuffer.id, "settings": {"channel": channel.value}}
    response = await connection.request("readFramebufferAsPng", params)
    return response.binary


async def read_framebuffer_as_exr(
    connection: Connection, framebuffer: Framebuffer, channels: set[FramebufferChannel]
) -> bytes:
    params = {"id": framebuffer.id, "settings": {"channels": [channel.value for channel in channels]}}
    response = await connection.request("readFramebufferAsExr", params)
    return response.binary
