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

from typing import Any, Protocol

from brayns.instance.request_future import RequestFuture


class Instance(Protocol):

    def __enter__(self) -> 'Instance':
        return self

    def __exit__(self, *_) -> None:
        self.disconnect()

    def disconnect(self) -> None:
        pass

    def request(self, method: str, params: Any = None) -> Any:
        return self.task(method, params).wait_for_result()

    def task(self, method: str, params: Any = None) -> RequestFuture:
        raise NotImplementedError()
