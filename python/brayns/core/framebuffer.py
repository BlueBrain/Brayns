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

from abc import ABC, abstractmethod
from dataclasses import dataclass
from typing import Any

from brayns.network import Instance


@dataclass
class Framebuffer(ABC):
    """Base class for all framebuffer types.

    Framebuffers define how the rendered image is stored and processed.
    """

    @classmethod
    @property
    @abstractmethod
    def name(cls) -> str:
        """Get the framebuffer name.

        :return: Framebuffer name
        :rtype: str
        """
        pass

    @abstractmethod
    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        pass


@dataclass
class ProgressiveFramebuffer(Framebuffer):
    """Progressive resolution framebuffer.

    :param scale: First-frame reduction factor over original resolution.
    :type scale: int, optional
    """

    scale: int = 4

    @classmethod
    @property
    def name(cls) -> str:
        """Get the framebuffer name.

        :return: Framebuffer name
        :rtype: str
        """
        return "progressive"

    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {"scale": self.scale}


@dataclass
class StaticFramebuffer(Framebuffer):
    """Static resolution framebuffer."""

    @classmethod
    @property
    def name(cls) -> str:
        """Get the framebuffer name.

        :return: Framebuffer name
        :rtype: str
        """
        return "static"

    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {}


def set_framebuffer(instance: Instance, framebuffer: Framebuffer) -> None:
    """Stablish the given framebuffer on the engine.

    :param instance: Instance.
    :type instance: Instance
    :param framebuffer: Framebuffer to set on the engine.
    :type framebuffer: Framebuffer
    """
    name = framebuffer.name
    params = framebuffer.get_properties()
    if not params:
        params = None
    instance.request(f"set-framebuffer-{name}", params)
