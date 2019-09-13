#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2016-2019, Blue Brain Project
#                          Raphael Dumusc <raphael.dumusc@epfl.ch>
#                          Daniel Nachbaur <daniel.nachbaur@epfl.ch>
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

from mock import patch

from .mocks import *
from .mocks_diffuse_tensor_imaging import *


def test_add_streamline():
    streamlines = [
        [
            [0, 0, 0], [1, 1, 1], [2, 2, 2]
        ],
        [
            [0, 0, 0], [1, 1, 1], [2, 2, 2]
        ]
    ]
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_dti_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        dti = DiffuseTensorImaging(app)
        response = dti.add_streamlines(name='dti', streamlines=streamlines)
        assert_equal(response, {'ok'})


if __name__ == '__main__':
    import nose
    nose.run(defaultTest=__name__)
