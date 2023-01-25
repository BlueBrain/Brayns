# Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

from .simple_test_case import SimpleTestCase


def add_sphere(context: SimpleTestCase) -> brayns.Model:
    sphere = brayns.Sphere(1).with_color(brayns.Color4.red)
    return brayns.add_geometries(context.instance, [sphere])


def add_light(context: SimpleTestCase) -> brayns.Model:
    light = brayns.AmbientLight(1)
    return brayns.add_light(context.instance, light)


def add_clip_plane(context: SimpleTestCase) -> brayns.Model:
    plane = brayns.ClippingPlane(brayns.PlaneEquation(1, 2, 3))
    return brayns.add_clipping_geometries(context.instance, [plane])


def load_circuit(
    context: SimpleTestCase, dendrites: bool = False, report: bool = False
) -> brayns.Model:
    loader = brayns.BbpLoader(
        cells=brayns.BbpCells.all(),
        report=brayns.BbpReport.compartment("somas") if report else None,
        morphology=brayns.Morphology(
            radius_multiplier=10,
            load_soma=True,
            load_dendrites=dendrites,
        ),
    )
    models = loader.load_models(context.instance, context.bbp_circuit)
    context.assertEqual(len(models), 1)
    return models[0]


def load_neurons(context: SimpleTestCase, gids: list[int]) -> brayns.Model:
    loader = brayns.BbpLoader(
        cells=brayns.BbpCells.from_gids(gids),
        morphology=brayns.Morphology(
            load_dendrites=True,
        ),
    )
    models = loader.load_models(context.instance, context.bbp_circuit)
    context.assertEqual(len(models), 1)
    return models[0]
