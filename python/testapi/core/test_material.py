# Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

import brayns
from testapi.loading import add_sphere
from testapi.render import RenderSettings, render_and_validate
from testapi.simple_test_case import SimpleTestCase


class TestMaterial(SimpleTestCase):
    def test_car_paint(self) -> None:
        material = brayns.CarPaintMaterial(
            flake_density=0.5,
        )
        self.run_tests(material)

    def test_emissive(self) -> None:
        material = brayns.EmissiveMaterial(
            intensity=5,
            color=brayns.Color3.red,
        )
        self.run_tests(material)

    def test_ghost(self) -> None:
        material = brayns.GhostMaterial()
        self.run_tests(material)

    def test_glass(self) -> None:
        material = brayns.GlassMaterial(
            refraction_index=2,
        )
        self.run_tests(material)

    def test_matte(self) -> None:
        material = brayns.MatteMaterial(
            opacity=0.9,
        )
        self.run_tests(material)

    def test_metal(self) -> None:
        material = brayns.MetalMaterial(
            roughness=0.1,
        )
        self.run_tests(material)

    def test_phong(self) -> None:
        material = brayns.PhongMaterial(
            opacity=0.9,
        )
        self.run_tests(material)

    def test_plastic(self) -> None:
        material = brayns.PlasticMaterial(
            opacity=0.9,
        )
        self.run_tests(material)

    def test_principled(self) -> None:
        material = brayns.PrincipledMaterial(
            edge_color=brayns.Color3(0.8, 0.8, 0.8),
            metallic=0.8,
            roughness=0.6,
            anisotropy=1,
            anisotropy_rotation=0.9,
        )
        self.run_tests(material)

    def run_tests(self, material: brayns.Material) -> None:
        model = add_sphere(self)
        brayns.set_material(self.instance, model.id, material)
        name = brayns.get_material_name(self.instance, model.id)
        self.assertEqual(name, material.name)
        retreived = brayns.get_material(self.instance, model.id, type(material))
        self.assertEqual(material, retreived)
        filename = f"{material.name}_material"
        settings = RenderSettings(renderer=brayns.ProductionRenderer(16, 3))
        render_and_validate(self, filename, settings)
