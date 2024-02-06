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

from .simple_test_case import SimpleTestCase


def add_sphere(context: SimpleTestCase) -> brayns.Model:
    sphere = brayns.Sphere(1)
    color = brayns.Color4.red
    return brayns.add_geometries(context.instance, [(sphere, color)])


def add_light(context: SimpleTestCase) -> brayns.Model:
    light = brayns.AmbientLight(1)
    return brayns.add_light(context.instance, light)


def add_clip_plane(context: SimpleTestCase) -> brayns.Model:
    plane = brayns.Plane(brayns.PlaneEquation(1, 2, 3))
    return brayns.add_clipping_geometries(context.instance, [plane])


def load_sonata_circuit(
    context: SimpleTestCase, dendrites: bool = False, report: bool = False
) -> brayns.Model:
    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name="cerebellum_neurons",
                nodes=brayns.SonataNodes.all(),
                report=brayns.SonataReport.compartment("test") if report else None,
                morphology=brayns.Morphology(load_dendrites=dendrites),
            )
        ]
    )
    models = loader.load_models(context.instance, context.sonata_circuit)
    context.assertEqual(len(models), 1)
    return models[0]
