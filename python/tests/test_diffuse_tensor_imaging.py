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

from brayns.plugins.diffuse_tensor_imaging import DiffuseTensorImaging

from .mocks import *


def test_add_streamline():
    app = brayns.Client('localhost:8200')
    dti = DiffuseTensorImaging(app)
    streamlines = [
        [
            [0, 0, 0], [1, 1, 1], [2, 2, 2]
        ],
        [
            [0, 0, 0], [1, 1, 1], [2, 2, 2]
        ]
    ]
    response = dti.add_streamlines(name='dti', streamlines=streamlines)
    assert_equal(response, 'OK')


if __name__ == '__main__':
    import nose
    nose.run(defaultTest=__name__)
