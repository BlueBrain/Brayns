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

from ..core import add_light, get_bounds
from ..network.instance import Instance
from ..service import Manager
from .cli import Cli
from .core import CameraCli, LightCli, LoaderCli, RendererCli, ServiceCli
from .render_context import RenderContext


@dataclass
class RenderCli(Cli):

    loader: LoaderCli
    service: ServiceCli = field(default_factory=ServiceCli)
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
    def render(self, context: RenderContext) -> None:
        pass

    def register(self, parser: argparse.ArgumentParser) -> None:
        self.loader.register(parser)
        self.service.register(parser)
        self.light.register(parser)
        self.camera.register(parser)
        self.renderer.register(parser)
        self.register_additional_args(parser)

    def load(self, args: argparse.Namespace) -> None:
        self.loader.load(args)
        self.service.load(args)
        self.light.load(args)
        self.camera.load(args)
        self.renderer.load(args)
        self.load_additional_args(args)

    def run(self) -> None:
        with self._start_service() as manager:
            instance = manager.instance
            self._load_models(instance)
            context = self._create_context(instance)
            self._add_light(instance, context)
            self.render(context)

    def _start_service(self) -> Manager:
        bundle = self.service.create_bundle()
        return bundle.start()

    def _load_models(self, instance: Instance) -> None:
        loader = self.loader.create_loader()
        loader.load(instance, self.loader.path)

    def _create_context(self, instance: Instance) -> RenderContext:
        bounds = get_bounds(instance)
        camera = self.camera.create_camera(bounds)
        view = self.camera.create_view(camera, bounds)
        renderer = self.renderer.create_renderer()
        return RenderContext(
            instance=instance,
            view=view,
            camera=camera,
            renderer=renderer,
        )

    def _add_light(self, instance: Instance, context: RenderContext) -> None:
        direction = context.view.direction
        light = self.light.create_directional_light(direction)
        add_light(instance, light)
