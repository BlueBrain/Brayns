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

from __future__ import annotations

from brayns.network import Instance
from brayns.utils import Resolution

from .serialize_application import serialize_application


def update_application(
    instance: Instance,
    resolution: Resolution | None = None,
    jpeg_quality: int | None = None,
) -> None:
    """Update application parameters of the instance.

    None parameters are left to their current value.

    :param instance: Instance.
    :type instance: Instance
    :param resolution: Streaming resolution, defaults to None
    :type resolution: Resolution | None, optional
    :param jpeg_quality: JPEG stream quality (0-100), defaults to None
    :type jpeg_quality: int | None, optional
    """
    params = serialize_application(resolution, jpeg_quality)
    instance.request('set-application-parameters', params)
