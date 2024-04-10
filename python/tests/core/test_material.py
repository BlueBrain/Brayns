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

import unittest

import brayns
from tests.mock_instance import MockInstance


class TestMaterial(unittest.TestCase):
    def test_get_material_name(self) -> None:
        name = "test"
        instance = MockInstance(name)
        test = brayns.get_material_name(instance, 0)
        self.assertEqual(test, name)
        self.assertEqual(instance.method, "get-material-type")
        self.assertEqual(instance.params, {"id": 0})

    def test_get_material(self) -> None:
        material = brayns.PhongMaterial()
        reply = material.get_properties()
        instance = MockInstance(reply)
        test = brayns.get_material(instance, 0, brayns.PhongMaterial)
        self.assertEqual(test, material)
        self.assertEqual(instance.method, "get-material-phong")
        self.assertEqual(instance.params, {"id": 0})

    def test_set_material(self) -> None:
        instance = MockInstance()
        material = brayns.PhongMaterial()
        brayns.set_material(instance, 0, material)
        self.assertEqual(instance.method, "set-material-phong")
        self.assertEqual(
            instance.params, {"model_id": 0, "material": material.get_properties()}
        )

    def test_car_paint(self) -> None:
        self.assertEqual(brayns.CarPaintMaterial.name, "carpaint")
        test = brayns.CarPaintMaterial(3)
        self.assertEqual(test.get_properties(), {"flake_density": 3})
        test = brayns.CarPaintMaterial()
        test.update_properties({"flake_density": 3})
        self.assertEqual(test.flake_density, 3)

    def test_emissive(self) -> None:
        self.assertEqual(brayns.EmissiveMaterial.name, "emissive")
        test = brayns.EmissiveMaterial(2, brayns.Color3.red)
        self.assertEqual(
            test.get_properties(),
            {
                "intensity": 2,
                "color": [1, 0, 0],
            },
        )
        test = brayns.EmissiveMaterial()
        test.update_properties({"intensity": 2, "color": [1, 0, 0]})
        self.assertEqual(test.intensity, 2)
        self.assertEqual(test.color, brayns.Color3.red)

    def test_glass(self) -> None:
        self.assertEqual(brayns.GlassMaterial.name, "glass")
        test = brayns.GlassMaterial(2.5)
        self.assertEqual(test.get_properties(), {"index_of_refraction": 2.5})
        test = brayns.GlassMaterial()
        test.update_properties({"index_of_refraction": 2.5})
        self.assertEqual(test.refraction_index, 2.5)

    def test_matte(self) -> None:
        self.assertEqual(brayns.MatteMaterial.name, "matte")
        test = brayns.MatteMaterial(0.5)
        self.assertEqual(test.get_properties(), {"opacity": 0.5})
        test = brayns.MatteMaterial()
        test.update_properties({"opacity": 0.5})
        self.assertEqual(test.opacity, 0.5)

    def test_metal(self) -> None:
        self.assertEqual(brayns.MetalMaterial.name, "metal")
        test = brayns.MetalMaterial(0.5)
        self.assertEqual(test.get_properties(), {"roughness": 0.5})
        test = brayns.MetalMaterial()
        test.update_properties({"roughness": 0.5})
        self.assertEqual(test.roughness, 0.5)

    def test_phong(self) -> None:
        self.assertEqual(brayns.PhongMaterial.name, "phong")
        test = brayns.PhongMaterial(0.5)
        self.assertEqual(test.get_properties(), {"opacity": 0.5})
        test = brayns.PhongMaterial()
        test.update_properties({"opacity": 0.5})
        self.assertEqual(test.opacity, 0.5)

    def test_plastic(self) -> None:
        self.assertEqual(brayns.PlasticMaterial.name, "plastic")
        test = brayns.PlasticMaterial(0.5)
        self.assertEqual(test.get_properties(), {"opacity": 0.5})
        test = brayns.PlasticMaterial()
        test.update_properties({"opacity": 0.5})
        self.assertEqual(test.opacity, 0.5)

    def test_principled(self) -> None:
        self.assertEqual(brayns.PrincipledMaterial.name, "principled")
        test = brayns.PrincipledMaterial(
            edge_color=brayns.Color3(1, 0, 1),
            metallic=0.5,
            diffuse=0.6,
            specular=0.7,
            ior=1.78,
            transmission=0.3,
            transmission_color=brayns.Color3(0, 1, 1),
            transmission_depth=10,
            roughness=0.7,
            anisotropy=1,
            anisotropy_rotation=0.45,
            thin=True,
            thickness=58,
            back_light=1.5,
            coat=0.89,
            coat_ior=1.7,
            coat_color=brayns.Color3(1, 1, 0),
            coat_thickness=63,
            coat_roughness=0.25,
            sheen=0.1,
            sheen_color=brayns.Color3(0, 0, 0),
            sheen_tint=11,
            sheen_roughness=1,
        )
        self.assertEqual(
            test.get_properties(),
            {
                "edge_color": [1, 0, 1],
                "metallic": 0.5,
                "diffuse": 0.6,
                "specular": 0.7,
                "ior": 1.78,
                "transmission": 0.3,
                "transmission_color": [0, 1, 1],
                "transmission_depth": 10,
                "roughness": 0.7,
                "anisotropy": 1,
                "anisotropy_rotation": 0.45,
                "thin": True,
                "thickness": 58,
                "back_light": 1.5,
                "coat": 0.89,
                "coat_ior": 1.7,
                "coat_color": [1, 1, 0],
                "coat_thickness": 63,
                "coat_roughness": 0.25,
                "sheen": 0.1,
                "sheen_color": [0, 0, 0],
                "sheen_tint": 11,
                "sheen_roughness": 1,
            },
        )
        test.update_properties(
            {
                "edge_color": [1, 0, 1],
                "metallic": 0.5,
                "diffuse": 0.6,
                "specular": 0.7,
                "ior": 1.78,
                "transmission": 0.3,
                "transmission_color": [0, 1, 1],
                "transmission_depth": 10,
                "roughness": 0.7,
                "anisotropy": 1,
                "anisotropy_rotation": 0.45,
                "thin": False,
                "thickness": 58,
                "back_light": 1.5,
                "coat": 0.89,
                "coat_ior": 1.7,
                "coat_color": [1, 1, 0],
                "coat_thickness": 63,
                "coat_roughness": 0.25,
                "sheen": 0.1,
                "sheen_color": [0, 0, 0],
                "sheen_tint": 11,
                "sheen_roughness": 1,
            }
        )
        self.assertEqual(test.thin, False)
