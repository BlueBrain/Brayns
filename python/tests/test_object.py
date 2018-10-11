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


def test_object_properties():
    with patch('brayns.utils.http_request', new=mock_http_request), \
         patch('rockets.Client.request', new=mock_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        assert_equal(app.test_object.integer, TEST_OBJECT['integer'])
        assert_equal(app.test_object.number, TEST_OBJECT['number'])
        assert_equal(app.test_object.string, TEST_OBJECT['string'])
        assert_equal(app.test_object.boolean, TEST_OBJECT['boolean'])


def test_object_properties_enum():
    with patch('brayns.utils.http_request', new=mock_http_request), \
         patch('rockets.Client.request', new=mock_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        assert_equal(app.test_object.enum, TEST_OBJECT['enum'])
        assert_true(hasattr(app, 'ENUM_VALUE_A'))
        assert_true(hasattr(app, 'ENUM_VALUE_B'))


def test_object_properties_enum_with_title():
    with patch('brayns.utils.http_request', new=mock_http_request), \
         patch('rockets.Client.request', new=mock_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        assert_equal(app.test_object.enum_title, TEST_OBJECT['enum_title'])
        assert_true(hasattr(app, 'MY_ENUM_MINE'))
        assert_true(hasattr(app, 'MY_ENUM_YOURS'))


def test_object_properties_enum_array():
    with patch('brayns.utils.http_request', new=mock_http_request), \
         patch('rockets.Client.request', new=mock_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        assert_equal(app.test_object.enum_array, TEST_OBJECT['enum_array'])

        assert_true(hasattr(app, 'ENUM_ARRAY_ONE'))
        assert_true(hasattr(app, 'ENUM_ARRAY_TWO'))
        assert_true(hasattr(app, 'ENUM_ARRAY_THREE'))


def test_object_properties_array():
    with patch('rockets.Client.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('rockets.Client.request', new=mock_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        assert_equal(app.test_object.array, TEST_OBJECT['array'])


@raises(AttributeError)
def test_object_replace():
    with patch('rockets.Client.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        app.test_object = [1,2,3]


def test_object_commit():
    with patch('brayns.utils.http_request', new=mock_http_request), \
         patch('rockets.Client.batch', new=mock_batch), \
         patch('rockets.Client.request', new=mock_rpc_request), \
         patch('rockets.AsyncClient.connected', new=mock_connected):
        app = brayns.Client('localhost:8200')
        app.test_object.integer = 42
        app.test_object.commit()


if __name__ == '__main__':
    import nose
    nose.run(defaultTest=__name__)
