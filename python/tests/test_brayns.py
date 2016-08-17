#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2016, Blue Brain Project
#                     Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of brayns
# <https://github.com/BlueBrain/Brayns>
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
# All rights reserved. Do not distribute without further notice.

from brayns.brayns import *

import unittest
import json

class BraynsTestCase(unittest.TestCase):

    def test_camera_serialization(self):

        camera = Camera()
        camera.origin = [0.0, 0.0, -3.0]
        camera.look_at = [0.0, 0.0, 0.0]
        camera.up = [0, 0, 1]
        camera.aperture = 0.5
        camera.focal_length = 100.0

        obj = json.loads(camera.serialize())
        self.assertEqual(camera.origin,
            [obj['origin']['x'],obj['origin']['y'],obj['origin']['z']])
        self.assertEqual(camera.look_at,
            [obj['lookAt']['x'], obj['lookAt']['y'], obj['lookAt']['z']])
        self.assertEqual(camera.up,
            [obj['up']['x'], obj['up']['y'], obj['up']['z']])
        self.assertEqual(camera.aperture, obj['fovAperture'])
        self.assertEqual(camera.focal_length, obj['fovFocalLength'])


    def test_camera_deserialization(self):
        camera = Camera()
        camera.deserialize(
            '{"up": {"x": 0, "z": 1, "y": 0}, '
            '"fovFocalLength": 100.0, '
            '"origin": {"x": 0.0, "z": -3.0, "y": 0.0}, '
            '"lookAt": {"x": 0.0, "z": 0.0, "y": 0.0}, '
            '"fovAperture": 0.5}')
        self.assertEqual(camera.origin, [0,0,-3])
        self.assertEqual(camera.look_at, [0,0,0])
        self.assertEqual(camera.up, [0,0,1])
        self.assertEqual(camera.focal_length, 100.0)
        self.assertEqual(camera.aperture, 0.5)


    def test_material_serialization(self):
        material = Material()
        material.index = 0
        material.diffuse_color = [1.0, 0.0, 0.0]
        material.specular_color = [0.0, 1.0, 0.0]
        material.specular_exponent = 10.0
        material.refraction_index = 0.5
        material.opacity = 0.1
        material.reflection_index = 0.1
        material.light_emission = 0.8

        obj = json.loads(material.serialize())
        self.assertEqual(material.diffuse_color,
            [obj['diffuseColor']['r'], obj['diffuseColor']['g'], obj['diffuseColor']['b']])
        self.assertEqual(material.specular_color,
            [obj['specularColor']['r'], obj['specularColor']['g'], obj['specularColor']['b']])
        self.assertEqual(material.specular_exponent, obj['specularExponent'])
        self.assertEqual(material.opacity, obj['opacity'])
        self.assertEqual(material.refraction_index, obj['refractionIndex'])
        self.assertEqual(material.reflection_index, obj['reflectionIndex'])
        self.assertEqual(material.light_emission, obj['lightEmission'])

    def test_material_deserialization(self):
        material = Material()
        material.deserialize(
            '{"opacity": 0.1, '
            '"diffuseColor": {"r": 1.0, "g": 0.0, "b": 0.0}, '
            '"specularColor": {"r": 0.0, "g": 1.0, "b": 0.0}, '
            '"reflectionIndex": 0.1, '
            '"index": 0, '
            '"specularExponent": 10.0, '
            '"refractionIndex": 0.5, '
            '"lightEmission": 0.8}')
        self.assertEqual(material.index, 0)
        self.assertEqual(material.diffuse_color, [1.0, 0.0, 0.0])
        self.assertEqual(material.specular_color, [0.0, 1.0, 0.0])
        self.assertEqual(material.specular_exponent, 10.0)
        self.assertEqual(material.opacity, 0.1)
        self.assertEqual(material.reflection_index, 0.1)
        self.assertEqual(material.refraction_index, 0.5)
        self.assertEqual(material.light_emission, 0.8)

    def test_transfer_function_serialization(self):
        transfer_function = TransferFunction()
        control_points = [
            [ -92.0915, 0.1 ], [-61.0, 0.1 ],
            [-50.0, 0.8 ], [0.0, 0.0], [49.5497, 1]]
        transfer_function.set_control_points(
            BRAYNS_ATTRIBUTE_RED, control_points)

        obj = json.loads(transfer_function.serialize(BRAYNS_ATTRIBUTE_RED))
        for p in range(0, len(control_points)):
            self.assertEqual(control_points[p], [obj['points'][p]['x'],obj['points'][p]['y']])

    def test_transfer_function_deserialization(self):
        transfer_function = TransferFunction()
        transfer_function.deserialize(
            '{"attribute": "red",'
            '"points": [{"x": -92.0915, "y": 0.1}, '
            '{"x": -61.0, "y": 0.1}, {"x": -50.0, "y": 0.8}, '
            '{"x": 0.0, "y": 0.0}, {"x": 49.5497, "y": 1}]}')
        control_points = [
            [ -92.0915, 0.1 ], [-61.0, 0.1 ],
            [-50.0, 0.8 ], [0.0, 0.0], [49.5497, 1]]
        for p in range(0, len(control_points)):
            self.assertEqual(control_points[p], transfer_function.get_control_points("red")[p])
