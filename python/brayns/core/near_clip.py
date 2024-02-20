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

from brayns.network import Instance


def get_camera_near_clip(instance: Instance) -> float:
    """Get the current camera near clipping distance.

    :param instance: Instance.
    :type instance: Instance
    :return: Near clipping distance.
    :rtype: float
    """
    result = instance.request("get-camera-near-clip")
    return result["distance"]


def set_camera_near_clip(instance: Instance, distance: float) -> None:
    """Set the current camera near clipping distance.

    :param instance: Instance.
    :type instance: Instance
    :param distance: Near clipping distance.
    :type distance: float
    """
    params = {"distance": distance}
    instance.request("set-camera-near-clip", params)
