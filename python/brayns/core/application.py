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

from brayns.network import Instance
from brayns.utils import Resolution


@dataclass
class Application:
    """Store the application parameters of an instance.

    :param plugins: List of plugins loaded in the instance.
    :type plugins: list[str]
    :param resolution: Framebuffer resolution.
    :type resolution: Resolution
    """

    plugins: list[str]
    resolution: Resolution


def get_application(instance: Instance) -> Application:
    """Retreive the application parameters from an instance.

    :param instance: Instance.
    :type instance: Instance
    :return: Current application parameters.
    :rtype: Application
    """
    result = instance.request("get-application-parameters")
    return _deserialize_application(result)


def set_resolution(instance: Instance, resolution: Resolution) -> None:
    """Update the framebuffer resolution of the given instance.

    :param instance: Instance.
    :type instance: Instance
    :param resolution: Framebuffer resolution.
    :type resolution: Resolution
    """
    params = {"viewport": list(resolution)}
    instance.request("set-application-parameters", params)


def stop(instance: Instance) -> None:
    """Stop a running instance.

    :param instance: Instance.
    :type instance: Instance
    """
    instance.request("quit")


def _deserialize_application(message: dict[str, Any]) -> Application:
    return Application(
        plugins=message["plugins"],
        resolution=Resolution(*message["viewport"]),
    )
