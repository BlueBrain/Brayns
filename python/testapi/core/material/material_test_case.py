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

import pathlib

import brayns
from testapi.simple_test_case import SimpleTestCase


class MaterialTestCase(SimpleTestCase):

    @property
    def renderer(self) -> brayns.Renderer:
        return brayns.ProductionRenderer(32)

    @property
    def ref(self) -> pathlib.Path:
        name = self.filename.replace('test_', '')
        name = name.replace('_material', '')
        name += '.png'
        return self.folder / name

    def run_tests(self, material: brayns.Material) -> None:
        model = self.add_sphere()
        brayns.set_material(self.instance, model.id, material)
        self._check_name(material, model)
        self._check_get(material, model)
        self.quick_validation(self.ref)

    def _check_name(self, material: brayns.Material, model: brayns.Model) -> None:
        name = brayns.get_material_name(self.instance, model.id)
        self.assertEqual(name, material.name)

    def _check_get(self, material: brayns.Material, model: brayns.Model) -> None:
        ref = brayns.get_material(self.instance, model.id, type(material))
        self.assertEqual(material, ref)
