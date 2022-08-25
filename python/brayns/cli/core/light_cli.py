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

import argparse
from dataclasses import dataclass

from brayns.core import DirectionalLight
from brayns.utils import Color3, Rotation, Vector3

from ..cli import Cli
from ..utils import RGB, XYZ, rotation


@dataclass
class LightCli(Cli):

    color: Color3 = Color3.white
    intensity: float = 1.0
    rotation: Rotation = Rotation.identity

    def register(self, parser: argparse.ArgumentParser) -> None:
        parser.add_argument(
            '--light_color',
            type=float,
            nargs=3,
            default=list(self.color),
            metavar=RGB,
            help='Light color RGB normalized',
        )
        parser.add_argument(
            '--light_intensity',
            type=float,
            default=self.intensity,
            metavar='VALUE',
            help='Light intensity',
        )
        parser.add_argument(
            '--light_rotation',
            type=float,
            nargs=3,
            default=list(self.rotation.euler_degrees),
            metavar=XYZ,
            help='Light rotation (relative to camera direction)',
        )

    def load(self, args: argparse.Namespace) -> None:
        self.color = Color3(*args.light_color)
        self.intensity = args.light_intensity
        self.rotation = rotation(args.light_rotation)

    def create_light(self, direction: Vector3) -> DirectionalLight:
        return DirectionalLight(
            color=self.color,
            intensity=self.intensity,
            direction=self.rotation.apply(direction),
        )
