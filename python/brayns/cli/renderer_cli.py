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

from ..core import InteractiveRenderer, ProductionRenderer, Renderer
from ..utils import Color4
from .common import RGBA


@dataclass
class RendererCli:

    name: str = InteractiveRenderer.name
    available: list[type[Renderer]] = field(default_factory=lambda: [
        InteractiveRenderer,
        ProductionRenderer,
    ])
    background: Color4 = Color4.bbp_background
    samples: int = 1

    def register(self, parser: argparse.ArgumentParser) -> None:
        parser.add_argument(
            '--renderer_type',
            choices=[renderer.name for renderer in self.available],
            default=self.name,
            help='Renderer type',
        )
        parser.add_argument(
            '--background',
            type=float,
            nargs=4,
            default=list(self.background),
            metavar=RGBA,
            help='Background color RGBA',
        )
        parser.add_argument(
            '--samples',
            type=int,
            default=self.samples,
            metavar='COUNT',
            help='Samples per pixels (antialiasing)',
        )

    def parse(self, args: argparse.Namespace) -> None:
        self.name = args.renderer_type
        self.background = Color4(*args.background)
        self.samples = args.samples

    def create_renderer(self) -> Renderer:
        for renderer in self.available:
            if renderer == self.name:
                return renderer(
                    samples_per_pixel=self.samples,
                    background_color=self.background,
                )
        raise ValueError(f'Renderer type not supported: "{self.name}"')
