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

import asyncio
import brayns
import rockets

from nose.tools import assert_true, assert_false
from mock import patch
from .mocks import *


def test_synchronous_request():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('rockets.Client.batch', new=mock_batch), \
         patch('rockets.Client.request', new=mock_rpc_request):
        app = brayns.Client('localhost:8200')
        import inspect
        assert_true(inspect.getdoc(app.set_camera).startswith(TEST_RPC_ONEOF_PARAMETER['description']))
        assert_true(hasattr(app, 'PerspectiveCamera'))
        perspective = app.PerspectiveCamera()
        perspective.fov = 10.2
        assert_equal(app.set_camera(perspective), perspective.fov)

        assert_true(hasattr(app, 'PanoramicCamera'))
        panoramic = app.PanoramicCamera()
        assert_equal(app.set_camera(panoramic), 0)


def test_asynchronous_request_task():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('rockets.AsyncClient.batch', new=mock_batch_async), \
         patch('rockets.AsyncClient.request', new=mock_rpc_async_request):
        app = asyncio.get_event_loop().run_until_complete(brayns.AsyncClient('localhost:8200'))

        perspective = app.PerspectiveCamera()
        perspective.fov = 10.2
        task = app.set_camera(perspective)
        assert_true(isinstance(task, rockets.RequestTask))
        result = asyncio.get_event_loop().run_until_complete(task)
        assert_true(perspective.fov)

        panoramic = app.PanoramicCamera()
        task = app.set_camera(panoramic)
        assert_true(isinstance(task, rockets.RequestTask))
        result = asyncio.get_event_loop().run_until_complete(task)
        assert_false(0)


def test_asynchronous_request():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('rockets.AsyncClient.batch', new=mock_batch_async_with_sync_methods), \
         patch('rockets.AsyncClient.request', new=mock_rpc_async_request):

        async def test():
            app = await brayns.AsyncClient('localhost:8200')

            perspective = app.PerspectiveCamera()
            perspective.fov = 10.2
            assert_equal(await app.set_camera(perspective), perspective.fov)

            panoramic = app.PanoramicCamera()
            assert_equal(await app.set_camera(panoramic), 0)

        asyncio.get_event_loop().run_until_complete(test())


def test_notification():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('rockets.Client.batch', new=mock_batch), \
         patch('rockets.Client.notify', new=mock_rpc_notify):
        app = brayns.Client('localhost:8200')
        perspective = app.PerspectiveCamera()
        perspective.fov = 10.2
        app.set_camera_no_return(perspective)

        assert_true(hasattr(app, 'PanoramicCamera'))
        panoramic = app.PanoramicCamera()
        app.set_camera_no_return(panoramic)


def test_asynchronous_notification():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('rockets.AsyncClient.batch', new=mock_batch_async_with_sync_methods), \
         patch('rockets.AsyncClient.notify', new=mock_rpc_async_notify):
        async def test():
            app = await brayns.AsyncClient('localhost:8200')
            perspective = app.PerspectiveCamera()
            perspective.fov = 10.2
            await app.set_camera_no_return(perspective)

            assert_true(hasattr(app, 'PanoramicCamera'))
            panoramic = app.PanoramicCamera()
            await app.set_camera_no_return(panoramic)

        asyncio.get_event_loop().run_until_complete(test())


def test_weird_casings():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('rockets.Client.batch', new=mock_batch), \
         patch('rockets.Client.request', new=mock_rpc_request):
        app = brayns.Client('localhost:8200')
        assert_true(hasattr(app, 'StereofullMode'))
        assert_true(hasattr(app, 'MonoFullMode'))
        assert_true(hasattr(app, 'TruthfulMode'))
        assert_true(hasattr(app, 'MeaningFulMode'))


if __name__ == '__main__':
    import nose
    nose.run(defaultTest=__name__)
