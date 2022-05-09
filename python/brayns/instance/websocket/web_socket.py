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

from typing import Protocol, Union


class WebSocket(Protocol):

    def __enter__(self) -> 'WebSocket':
        return self

    def __exit__(self, *_) -> None:
        self.close()

    @property
    def closed(self) -> bool:
        return False

    def close(self) -> None:
        pass

    def receive(self) -> Union[bytes, str]:
        raise NotImplementedError()

    def send(self, data: Union[bytes, str]) -> None:
        raise NotImplementedError()
