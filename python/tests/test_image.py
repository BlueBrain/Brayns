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

from nose.tools import assert_true, assert_false, raises
from mock import patch
from .mocks import *


def test_image():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        setattr(app, 'snapshot', mock_snapshot)
        assert_equal(app.image(size=[50,50], format='png').size, (50,50))


def test_async_image():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.AsyncClient.batch', new=mock_batch_async_with_sync_methods):
        async def test():
            app = await brayns.AsyncClient('localhost:8200')
            setattr(app, 'snapshot', mock_async_snapshot)
            img = await app.image(size=[50,50], format='png')
            assert_equal(img.size, (50, 50))

        asyncio.get_event_loop().run_until_complete(test())


def test_image_wrong_format():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        setattr(app, 'snapshot', mock_snapshot)
        assert_false(app.image(size=[50,50], format='foo'))


@raises(TypeError)
def test_image_not_base64():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        setattr(app, 'snapshot', mock_snapshot)
        app.image(size=[50, 50], format='jpg')


if __name__ == '__main__':
    import nose
    nose.run(defaultTest=__name__)
