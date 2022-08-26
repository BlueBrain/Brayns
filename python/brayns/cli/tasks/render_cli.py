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
from abc import abstractmethod
from dataclasses import dataclass, field

from brayns.core import add_light, get_bounds
from brayns.network import Instance
from brayns.utils import Resolution, Vector3

from ..cli import Cli
from ..core import (
    CameraCli,
    LightCli,
    LoaderCli,
    MeshLoaderCli,
    RendererCli,
    ServiceCli,
)
from ..utils import WIDTH_HEIGHT
from .render_command import RenderCommand


@dataclass
class RenderCli(Cli):

    path: str = ''
    resolution: Resolution = Resolution.full_hd
    service: ServiceCli = field(default_factory=ServiceCli)
    loader: LoaderCli = field(default_factory=MeshLoaderCli)
    light: LightCli = field(default_factory=LightCli)
    camera: CameraCli = field(default_factory=CameraCli)
    renderer: RendererCli = field(default_factory=RendererCli)

    @abstractmethod
    def register_additional_args(self, parser: argparse.ArgumentParser) -> None:
        pass

    @abstractmethod
    def load_additional_args(self, args: argparse.Namespace) -> None:
        pass

    @abstractmethod
    def render(self, command: RenderCommand) -> None:
        pass

    def register(self, parser: argparse.ArgumentParser) -> None:
        parser.add_argument(
            '--model_path',
            required=True,
            metavar='PATH',
            help='Path of the model to render',
        )
        parser.add_argument(
            '--resolution',
            type=int,
            nargs=2,
            default=list(self.resolution),
            metavar=WIDTH_HEIGHT,
            help='Snapshot resolution in pixels',
        )
        self.service.register(parser)
        self.loader.register(parser)
        self.light.register(parser)
        self.camera.register(parser)
        self.renderer.register(parser)
        self.register_additional_args(parser)

    def load(self, args: argparse.Namespace) -> None:
        self.path = args.model_path
        self.resolution = Resolution(*args.resolution)
        self.service.load(args)
        self.loader.load(args)
        self.light.load(args)
        self.camera.load(args)
        self.renderer.load(args)
        self.load_additional_args(args)

    def run(self) -> None:
        with self.service.start() as (_, instance):
            self._run(instance)

    def _run(self, instance: Instance) -> None:
        self._load_models(instance)
        command = self._create_command(instance)
        self._add_light(instance, command.view.direction)
        self.render(command)

    def _load_models(self, instance: Instance) -> None:
        loader = self.loader.create_loader()
        loader.load(instance, self.path)

    def _create_command(self, instance: Instance) -> RenderCommand:
        target = get_bounds(instance)
        camera = self.camera.create_camera(target)
        return RenderCommand(
            instance=instance,
            view=self.camera.create_view(camera, target),
            camera=camera,
            renderer=self.renderer.create_renderer(),
        )

    def _add_light(self, instance: Instance, direction: Vector3) -> None:
        light = self.light.create_light(direction)
        add_light(instance, light)
