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

from brayns.version import __version__

from .version_error import VersionError


@dataclass
class Version:
    """Instance version info.

    :param major: Major part.
    :type major: int
    :param minor: Minor part.
    :type minor: int
    :param patch: Patch part.
    :type patch: int
    :param revision: Commit hash.
    :type revision: str
    """

    major: int
    minor: int
    patch: int
    revision: str

    @property
    def release(self) -> tuple[int, int, int]:
        """Return a tuple with major, minor and patch.

        :return: Release tuple.
        :rtype: tuple[int, int, int]
        """
        return (self.major, self.minor, self.patch)

    @property
    def tag(self) -> str:
        """Format version tag 'major.minor.patch'.

        Can be compared to package __version__.

        :return: Version tag.
        :rtype: str
        """
        return '.'.join(str(i) for i in self.release)

    def check(self, local: str = __version__) -> None:
        """Check that self is compatible with local version.

        :param local: API version, defaults to __version__
        :type local: str, optional
        :raises VersionError: Version mismatch.
        """
        remote = self.tag
        if remote != local:
            raise VersionError(local, remote)
