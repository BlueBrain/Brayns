#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2016-2018, Blue Brain Project
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

from nose.tools import assert_true, assert_equal, raises
from mock import patch
import brayns

from .mocks import *


class MockTransferFunction(object):
    def __init__(self):
        self.contribution = []
        self.diffuse = []
        self.emission = []
        self.range = (0,1)
        self.commit_called = False

    def commit(self):
        self.commit_called = True



def test_set_colormap():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        setattr(app, 'transfer_function', MockTransferFunction())
        palette = [[0, 1, 0], [0, 0.5, 0.1]]
        app.set_colormap(palette, data_range=(0, 42))
        assert_equal(len(app.transfer_function.contribution), 2)
        assert_equal(len(app.transfer_function.diffuse), 2)
        assert_equal(len(app.transfer_function.emission), 0)
        assert_equal(app.transfer_function.range, (0,42))
        assert_true(app.transfer_function.commit_called)


if __name__ == '__main__':
    import nose
    nose.run(defaultTest=__name__)
