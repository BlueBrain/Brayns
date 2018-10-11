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

from nose.tools import assert_false, raises
from mock import patch
import brayns

from .mocks import *


def test_rpc_two_parameters():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('rockets.Client.batch', new=mock_batch), \
         patch('rockets.Client.request', new=mock_rpc_request):
        app = brayns.Client('localhost:8200')
        assert_false(hasattr(app, 'test-rpc-two-params'))


def test_rpc_invalid_type():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('rockets.Client.batch', new=mock_batch), \
         patch('rockets.Client.request', new=mock_rpc_request):
        app = brayns.Client('localhost:8200')
        assert_false(hasattr(app, 'test-rpc-invalid-type'))


def test_rpc_no_type():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('rockets.Client.batch', new=mock_batch), \
         patch('rockets.Client.request', new=mock_rpc_request):
        app = brayns.Client('localhost:8200')
        assert_false(hasattr(app, 'test-rpc-no-type'))


@raises(Exception)
def test_rpc_invalid_param():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request_invalid_rpc_param), \
         patch('rockets.Client.batch', new=mock_batch_request_invalid_rpc_param):
        brayns.Client('localhost:8200')


if __name__ == '__main__':
    import nose
    nose.run(defaultTest=__name__)
