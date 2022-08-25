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

import argparse
from dataclasses import dataclass

from brayns.core import Snapshot

from .render_cli import RenderCli
from .render_command import RenderCommand


@dataclass
class SnapshotCli(RenderCli):

    save_as: str = ''
    frame: int | None = None

    def register_additional_args(self, parser: argparse.ArgumentParser) -> None:
        parser.add_argument(
            '--save_as',
            type=str,
            required=True,
            metavar='PATH',
            help='Path to save snapshot as image',
        )
        parser.add_argument(
            '--frame',
            type=int,
            metavar='INDEX',
            help='Simulation frame to render if any',
        )

    def load_additional_args(self, args: argparse.Namespace) -> None:
        self.frame = args.frame
        self.save_as = args.save_as

    def render(self, command: RenderCommand) -> None:
        snapshot = Snapshot(
            resolution=self.resolution,
            frame=self.frame,
            view=command.view,
            camera=command.camera,
            renderer=command.renderer,
        )
        snapshot.save(command.instance, self.save_as)
