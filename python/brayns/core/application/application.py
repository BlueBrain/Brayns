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

from dataclasses import dataclass
from typing import Optional

from brayns.core.common.resolution import Resolution
from brayns.instance.instance import Instance


@dataclass
class Application:

    plugins: list[str]
    resolution: Resolution
    jpeg_stream_quality: int

    @staticmethod
    def from_instance(instance: Instance) -> 'Application':
        result = instance.request('get-application-parameters')
        return Application.deserialize(result)

    @staticmethod
    def deserialize(message: dict) -> 'Application':
        return Application(
            plugins=message['plugins'],
            resolution=Resolution(*message['viewport']),
            jpeg_stream_quality=message['jpeg_quality']
        )

    @staticmethod
    def update(
        instance: Instance,
        resolution: Optional[Resolution] = None,
        jpeg_stream_quality: Optional[int] = None
    ) -> None:
        params = {}
        if resolution is not None:
            params['viewport'] = list(resolution)
        if jpeg_stream_quality is not None:
            params['jpeg_quality'] = jpeg_stream_quality
        instance.request('set-application-parameters', params)
