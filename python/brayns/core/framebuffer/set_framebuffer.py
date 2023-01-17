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

from brayns.network import Instance

from .framebuffer import Framebuffer


def set_framebuffer(instance: Instance, framebuffer: Framebuffer) -> None:
    """Stablish the given framebuffer on the engine.

    :param instance: Instance.
    :type instance: Instance
    :param framebuffer: Framebuffer to set on the engine.
    :type framebuffer: Framebuffer
    """
    name = framebuffer.name
    params = framebuffer.get_properties()
    instance.request(f"set-framebuffer-{name}", params)
