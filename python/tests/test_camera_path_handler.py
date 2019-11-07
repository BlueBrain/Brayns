#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2016-2019, Blue Brain Project
#                          Cyrille Favreau <cyrille.favreau@epfl.ch>
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
# All rights reserved. Do not distribute without further notice.

from brayns.helpers.camera_path_handler import CameraPathHandler

from .mocks import *


def test_camera_path():
    control_points = [
        {'direction': [-0.6387967956339202, -0.24980175210227645, -0.7276934371934849],
         'origin': [4959.093644345365, 2540.593343261373, 5511.791057724291],
         'up': [-0.121758469867742, 0.9667275047906156, -0.22497290169435175],
         'apertureRadius': 1.0, 'focusDistance': 100.0},
        {'direction': [-0.8969663405298818, -0.2622288334588651, -0.3559317671397636],
         'origin': [2524.1099420710984, 1436.729156528393, 1597.2534181302708],
         'up': [-0.238409405953329, 0.9649058871763962, -0.11007989846158356],
         'apertureRadius': 2.0, 'focusDistance': 200.0},
        {'direction': [0.43421043105752677, -0.3849091261967752, -0.8144361645526749],
         'origin': [-356.52809337362544, 1909.5442028079417, 3250.6527794333015],
         'up': [0.10027899339992785, 0.9191482330827097, -0.38093391697724943],
         'apertureRadius': 3.0, 'focusDistance': 300.0}]
    camera_path = CameraPathHandler(control_points, 20, 20)
    assert_equal(camera_path.get_key_frame(5), [
        (4319.910422498369, 2250.828994243965, 4484.224927330861),
        (-0.7065663011691101, -0.25306386095838096, -0.6301059988043831),
        (-0.15237934059020858, 0.9662493301668829, -0.19481348834575013),
        1.2625, 126.25
    ])
    assert_equal(camera_path.get_nb_frames(), 41)
    try:
        camera_path.get_key_frame(100)
    except Exception as e:
        assert_equal(e.__class__, KeyError)


def test_get_orbit_around_target():

    class TestScene:

        def __init__(self):
            self.models = list()

    class TestBrayns:

        def __init__(self):
            self.scene = TestScene()

    brayns = TestBrayns()
    brayns.scene.models = [
        {
            'bounding_box': False,
            'bounds': {
                'max': [1.0, 1.0, 1.0],
                'min': [0.0, 0.0, 0.0]
            },
            'id': 0, 'metadata': {}, 'name': 'DefaultScene', 'path': 'DefaultScene',
            'transformation': {
                'rotation': [0.0, 0.0, 0.0, 1.0], 'rotation_center': [0.0, 0.0, 0.0],
                'scale': [1.0, 1.0, 1.0], 'translation': [0.0, 0.0, 0.0]
            },
            'visible': True}
    ]

    camera_key_frames = CameraPathHandler.get_orbit_around_target(brayns, 0, 2)
    assert_equal(camera_key_frames, [
        [[2.0, 0.5, 0.5], [-1.0, 0.0, 0.0], [0.0, 1.0, 0.0]],
        [[-1.0, 0.5, 0.5000000000000002], [1.0, 0.0, -1.4802973661668753e-16], [0.0, 1.0, 0.0]]
    ])


if __name__ == '__main__':
    import nose
    nose.run(defaultTest=__name__)
