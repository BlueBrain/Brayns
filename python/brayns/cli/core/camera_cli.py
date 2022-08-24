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
from dataclasses import dataclass, field

from ...core import Camera, OrthographicCamera, PerspectiveCamera
from ...utils import Bounds, Rotation, Vector3, View
from ..cli import Cli
from ..utils import XYZ, rotation


@dataclass
class CameraCli(Cli):

    name: str = PerspectiveCamera.name
    available: list[type[Camera]] = field(default_factory=lambda: [
        PerspectiveCamera,
        OrthographicCamera,
    ])
    translation: Vector3 = Vector3.zero
    rotation: Rotation = Rotation.identity

    def register(self, parser: argparse.ArgumentParser) -> None:
        parser.add_argument(
            '--camera_type',
            choices=[camera.name for camera in self.available],
            default=self.name,
            help='Camera type',
        )
        parser.add_argument(
            '--camera_translation',
            type=float,
            nargs=3,
            default=list(self.translation),
            metavar=XYZ,
            help='Translate the camera XYZ (applied before rotation)',
        )
        parser.add_argument(
            '--camera_rotation',
            type=float,
            nargs=3,
            default=list(self.rotation.euler_degrees),
            metavar=XYZ,
            help='Camera rotation using euler angles XYZ in degrees',
        )

    def load(self, args: argparse.Namespace) -> None:
        self.name = args.camera_type
        self.translation = Vector3(*args.camera_translation)
        self.rotation = rotation(args.camera_rotation)

    def create_camera(self, target: Bounds) -> Camera:
        for camera in self.available:
            if camera.name == self.name:
                return camera.from_target(target)
        raise ValueError(f'Invalid camera type: "{self.name}"')

    def create_view(self, camera: Camera, bounds: Bounds) -> View:
        view = camera.get_front_view(bounds)
        view.position += self.translation
        view.position = self.rotation.apply(view.position, center=view.target)
        return view
