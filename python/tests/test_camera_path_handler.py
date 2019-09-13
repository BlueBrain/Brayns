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
         'up': [-0.121758469867742, 0.9667275047906156, -0.22497290169435175]},
        {'direction': [-0.8969663405298818, -0.2622288334588651, -0.3559317671397636],
         'origin': [2524.1099420710984, 1436.729156528393, 1597.2534181302708],
         'up': [-0.238409405953329, 0.9649058871763962, -0.11007989846158356]},
        {'direction': [0.43421043105752677, -0.3849091261967752, -0.8144361645526749],
         'origin': [-356.52809337362544, 1909.5442028079417, 3250.6527794333015],
         'up': [0.10027899339992785, 0.9191482330827097, -0.38093391697724943]}]
    camera_path = CameraPathHandler(control_points, 20, 20)
    assert_equal(camera_path.get_key_frame(5), [
        (3182.58910186726, 1779.708788705276, 2812.9496940410527),
        (-0.786236057771408, -0.26156771637033627, -0.4789228780912027),
        (-0.19494691514382148, 0.96430843110832, -0.15131914989430567)])
    assert_equal(camera_path.get_nb_frames(), 60)
    try:
        camera_path.get_key_frame(100)
    except Exception as e:
        assert_equal(e.__class__, KeyError)


if __name__ == '__main__':
    import nose
    nose.run(defaultTest=__name__)
