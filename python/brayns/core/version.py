# Copyright (c) 2015-2023 EPFL/Blue Brain Project
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
from typing import Any

from brayns.version import VERSION

from brayns.network import Instance
from brayns.utils import Error


@dataclass
class VersionError(Error):
    """Error raised when a version mismatch occurs.

    :param local: Local (Python API) version.
    :type local: str
    :param local: Remote (Binary) version.
    :type local: str
    """

    local: str
    remote: str

    def __str__(self) -> str:
        """Format exception.

        :return: Exception description.
        :rtype: str
        """
        local, remote = self.local, self.remote
        return f"Version mismatch with server {local=}, {remote=}"


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
        return ".".join(str(i) for i in self.release)

    def check(self, local: str = VERSION) -> None:
        """Check that self is compatible with local version.

        :param local: API version, defaults to VERSION.
        :type local: str, optional
        :raises VersionError: Version mismatch.
        """
        remote = self.tag
        if remote != local:
            raise VersionError(local, remote)


def get_version(instance: Instance) -> Version:
    """Retreive the version of a backend instance.

    :param instance: Instance.
    :type instance: Instance
    :return: Version info.
    :rtype: Version
    """
    result = instance.request("get-version")
    return _deserialize_version(result)


def check_version(instance: Instance) -> None:
    """Check that the version of the instance is compatible with the API.

    :param instance: Instance to check.
    :type instance: Instance
    """
    version = get_version(instance)
    version.check()


def _deserialize_version(message: dict[str, Any]) -> Version:
    return Version(
        major=message["major"],
        minor=message["minor"],
        patch=message["patch"],
        revision=message["revision"],
    )
