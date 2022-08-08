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

from dataclasses import dataclass
from typing import Any

from brayns.core.version.version_error import VersionError
from brayns.version import DEV_VERSION, __version__


@dataclass
class Version:

    major: int
    minor: int
    patch: int
    revision: str

    @staticmethod
    def deserialize(message: dict[str, Any]) -> Version:
        return Version(
            major=message['major'],
            minor=message['minor'],
            patch=message['patch'],
            revision=message['revision']
        )

    @property
    def release(self) -> tuple[int, int, int]:
        return (self.major, self.minor, self.patch)

    @property
    def tag(self) -> str:
        return '.'.join(str(i) for i in self.release)

    def check(self, local: str = __version__) -> None:
        remote = self.tag
        if local == DEV_VERSION:
            return
        if remote != local:
            raise VersionError(local, remote)
