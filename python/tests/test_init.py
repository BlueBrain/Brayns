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

from nose.tools import assert_equal, raises
from mock import patch
import asyncio
import brayns

from .mocks import *


def test_init():
    with patch('brayns.utils.http_request', new=mock_http_request), \
         patch('rockets.Client.request', new=mock_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        assert_equal(app.http_url, 'http://localhost:8200/')
        assert_equal(app.version.as_dict(), TEST_VERSION)
        assert_equal(str(app), 'Brayns version 1.1.0 running on http://localhost:8200/')


def test_async_init():
    with patch('brayns.utils.http_request', new=mock_http_request), \
         patch('rockets.AsyncClient.batch', new=mock_batch_async), \
         patch('rockets.AsyncClient.request', new=mock_rpc_async_request):
        app = asyncio.get_event_loop().run_until_complete(brayns.AsyncClient('localhost:8200'))
        assert_equal(app.http_url, 'http://localhost:8200/')
        assert_equal(app.version.as_dict(), TEST_VERSION)
        assert_equal(str(app), 'Brayns version 1.1.0 running on http://localhost:8200/')


@raises(Exception)
def test_init_no_version():
    with patch('brayns.utils.http_request', new=mock_http_request_no_version):
        brayns.Client('localhost:8200')


@raises(Exception)
def test_init_wrong_version():
    with patch('brayns.utils.http_request', new=mock_http_request_wrong_version):
        brayns.Client('localhost:8200')


@raises(Exception)
def test_init_no_registry():
    with patch('brayns.utils.http_request', new=mock_http_request_no_registry):
        brayns.Client('localhost:8200')


if __name__ == '__main__':
    import nose
    nose.run(defaultTest=__name__)
