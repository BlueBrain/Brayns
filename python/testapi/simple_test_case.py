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

import logging
import pathlib

import brayns

from .api_test_case import ApiTestCase
from .image_validator import ImageValidator


class SimpleTestCase(ApiTestCase):

    @property
    def resolution(self) -> brayns.Resolution:
        return brayns.Resolution.full_hd

    @property
    def renderer(self) -> brayns.Renderer:
        return brayns.InteractiveRenderer()

    @property
    def instance(self) -> brayns.Instance:
        return self.__manager.instance

    @property
    def process(self) -> brayns.Process:
        return self.__manager.process

    def setUp(self) -> None:
        service = brayns.Service(
            uri=f'localhost:{self.port}',
            executable=self.executable,
            env=self.env,
        )
        connector = brayns.Connector(
            uri=service.uri,
            logger=brayns.Logger(logging.getLevelName(self.log_level)),
            max_attempts=None,
        )
        self.__manager = brayns.start(service, connector)

    def tearDown(self) -> None:
        self.__manager.stop()

    def get_default_camera(self) -> brayns.Camera:
        controller = brayns.CameraController(
            target=brayns.get_bounds(self.instance),
            aspect_ratio=self.resolution.aspect_ratio,
        )
        return controller.camera

    def add_light(self) -> brayns.Model:
        light = brayns.AmbientLight(2)
        return brayns.add_light(self.instance, light)

    def snapshot(self, frame: int = 0) -> brayns.Snapshot:
        return brayns.Snapshot(
            resolution=self.resolution,
            camera=self.get_default_camera(),
            renderer=self.renderer,
            frame=frame,
        )

    def quick_snapshot(self, path: pathlib.Path, frame: int = 0) -> None:
        self.add_light()
        snapshot = self.snapshot(frame)
        snapshot.save(self.instance, str(path))

    def validate(self, snapshot: brayns.Snapshot, ref: pathlib.Path) -> None:
        format = brayns.parse_image_format(ref)
        data = snapshot.download(self.instance, format)
        validator = ImageValidator()
        validator.validate_data(data, ref)

    def quick_validation(self, ref: pathlib.Path, frame: int = 0) -> None:
        self.add_light()
        snapshot = self.snapshot(frame)
        self.validate(snapshot, ref)

    def add_sphere(self, color: brayns.Color4 = brayns.Color4.red) -> brayns.Model:
        return brayns.add_geometries(self.instance, [
            brayns.Sphere(1).with_color(color),
        ])

    def add_clip_plane(self) -> brayns.Model:
        plane = brayns.ClipPlane(brayns.PlaneEquation(1, 2, 3))
        return brayns.add_clipping_geometry(self.instance, plane)

    def load_circuit(self, dendrites: bool = False, report: bool = False) -> brayns.Model:
        loader = brayns.BbpLoader(
            cells=brayns.BbpCells.all(),
            report=brayns.BbpReport.compartment('somas') if report else None,
            morphology=brayns.Morphology(
                radius_multiplier=10,
                load_soma=True,
                load_dendrites=dendrites,
            ),
        )
        models = loader.load_models(self.instance, self.bbp_circuit)
        self.assertEqual(len(models), 1)
        return models[0]
