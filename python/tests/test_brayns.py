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

from nose.tools import assert_true, assert_false, assert_equal, raises
from mock import Mock, patch
import brayns

TEST_VERSION = {
    'major': 0,
    'minor': 7,
    'patch': 0,
    'revision': 12345
}

TEST_RPC_ONE_PARAMETER = {
    'title': 'test-rpc',
    'description': 'Pass on parameter to brayns',
    'type': 'method',
    'returns': {
        'anyOf': [{
                'type': 'null'
            }, {
                'type': 'object'
            }
        ]
    },
    'params': [{
        'type': 'object',
        'name': 'model_param',
        'description': 'bla',
        'properties': {
            'doit': {
                'type':'boolean'
            },
            'name': {
                'type: string'
            }
        },
        'required': ['name']
    }]
}

TEST_RPC_INVALID_TYPE = {
    'title': 'test-rpc-invalid-type',
    'description': 'Should be method, not object',
    'type': 'object'
}

TEST_RPC_INVALID_PARAM = {
    'title': 'test-rpc-invalid-param',
    'description': 'Only objects for params, no single values',
    'type': 'method',
    'params': [{'type': 'string'}]
}

TEST_RPC_TWO_PARAMETERS = {
    'title': 'test-rpc-two-params',
    'description': 'Pass on two parameters to brayns',
    'type': 'method',
    'params': [{
        'type': 'object',
        'name': 'param_one',
        'properties': {}
    }, {
        'type': 'object',
        'name': 'param_two',
        'properties': {}
    }]
}

TEST_RPC_ONLY_RETURN = {
    'title': 'test-rpc-return',
    'description': 'Only returns something, no parameter',
    'type': 'method',
    'returns': {
        'type': 'boolean'
    }
}

TEST_RPC_ONEOF_PARAMETER = {
    'title': 'set-camera',
    'description': 'Pass on oneOf parameter to brayns',
    'type': 'method',
    'params': [{
        'oneOf': [{
            'type': 'object',
            'title': 'perspective',
            'properties': { 'fov': { 'type': 'number' }}
        }, {
            'type': 'object',
            'title': 'panoramic',
            'properties': {}
        }]
    }]
}

TEST_RPC_ONEOF_PARAMETER_WEIRD_CASING = {
    'title': 'set-mode',
    'description': 'Check different casings work with created types',
    'type': 'method',
    'params': [{
        'oneOf': [{
            'type': 'object',
            'title': 'StereoFull',
            'properties': {}
        }, {
            'type': 'object',
            'title': 'mono Full',
            'properties': {}
        }, {
            'type': 'object',
            'title': 'truthFul',
            'properties': {}
        }, {
            'type': 'object',
            'title': 'meaning_ful',
            'properties': {}
        }]
    }]
}

TEST_RPC_ARRAY_PARAMETER = {
    'title': 'inspect',
    'description': 'Pass on array parameter to brayns',
    'type': 'method',
    'params': [{
        'type': 'array',
        'name': 'xy',
    }]
}

TEST_OBJECT_SCHEMA = {
    'title': 'TestObject',
    'type': 'object',
    'properties': {
        'number:': {'type': 'number'},
        'integer:': {'type': 'integer'},
        'string:': {'type': 'string'},
        'boolean:': {'type': 'boolean'},
        'enum': {
            'type': 'string',
            'enum': [u'value_a', u'value_b']
        },
        'enum_title': {
            'type': 'string',
            'title': 'my_enum',
            'enum': [u'mine', u'yours']
        },
        'enum_array': {
            'type': 'array',
            'items': {
                'type':'string',
                'enum': [u'one', u'two', u'three']
            }
        },
        'array': {
            'type': 'array',
            'items': {
                'type': 'number'
            }
        }
    }
}

TEST_OBJECT = {
    'number': 0.1,
    'integer': 5,
    'string': 'foobar',
    'boolean': False,
    'enum': 'value_b',
    'enum_title': 'yours',
    'enum_array': ['one', 'three'],
    'array': [0, 42, 1]
}

TEST_ARRAY_SCHEMA = {
    'title': 'TestArray',
    'type': 'array',
    'items': {
        'type': 'number'
    }
}

TEST_ARRAY = [1, 42, -5]

VERSION_SCHEMA = {
    'title': 'Version',
    'type': 'object'
}

TEST_REGISTRY = {
    'test-rpc/schema': ['GET'],
    'set-camera/schema': ['GET'],
    'set-mode/schema': ['GET'],
    'inspect/schema': ['GET'],
    'test-rpc-invalid-type/schema': ['GET'],
    'test-rpc-invalid-param/schema': ['GET'],
    'test-rpc-return/schema': ['GET'],
    'test-rpc-two-params/schema': ['GET'],
    'test-array': ['GET', 'PUT'],
    'test-array/schema': ['GET'],
    'test-object': ['GET', 'PUT'],
    'test-object/schema': ['GET'],
    'version': ['GET']
}

def mock_http_request(method, url, command, body=None, query_params=None):
    if command == 'test-rpc/schema':
        return brayns.utils.Status(200, TEST_RPC_ONE_PARAMETER)
    if command == 'set-camera/schema':
        return brayns.utils.Status(200, TEST_RPC_ONEOF_PARAMETER)
    if command == 'set-mode/schema':
        return brayns.utils.Status(200, TEST_RPC_ONEOF_PARAMETER_WEIRD_CASING)
    if command == 'inspect/schema':
        return brayns.utils.Status(200, TEST_RPC_ARRAY_PARAMETER)
    if command == 'test-rpc-invalid-type/schema':
        return brayns.utils.Status(200, TEST_RPC_INVALID_TYPE)
    if command == 'test-rpc-two-params/schema':
        return brayns.utils.Status(200, TEST_RPC_TWO_PARAMETERS)
    if command == 'test-rpc-return/schema':
        return brayns.utils.Status(200, TEST_RPC_ONLY_RETURN)
    if command == 'test-object/schema':
        return brayns.utils.Status(200, TEST_OBJECT_SCHEMA)
    if command == 'test-object':
        return brayns.utils.Status(200, TEST_OBJECT)
    if command == 'test-array/schema':
        return brayns.utils.Status(200, TEST_ARRAY_SCHEMA)
    if command == 'test-array':
        return brayns.utils.Status(200, TEST_ARRAY)
    if command == 'version':
        return brayns.utils.Status(200, TEST_VERSION)
    if command == 'version/schema':
        return brayns.utils.Status(200, VERSION_SCHEMA)
    if command == 'registry':
        return brayns.utils.Status(200, TEST_REGISTRY)
    return brayns.utils.Status(404, None)


def mock_http_request_wrong_version(method, url, command, body=None, query_params=None):
    if command == 'version':
        import copy
        version = copy.deepcopy(TEST_VERSION)
        version['minor'] = 3
        return brayns.utils.Status(200, version)


def mock_http_request_no_version(method, url, command, body=None, query_params=None):
    return brayns.utils.Status(404, None)


def mock_http_request_no_registry(method, url, command, body=None, query_params=None):
    if command == 'version':
        return brayns.utils.Status(200, TEST_VERSION)
    if command == 'registry':
        return brayns.utils.Status(404, None)


def mock_http_request_invalid_rpc_param(method, url, command, body=None, query_params=None):
    if command == 'test-rpc-invalid-param/schema':
        return brayns.utils.Status(200, TEST_RPC_INVALID_PARAM)
    if command == 'version':
        return brayns.utils.Status(200, TEST_VERSION)
    if command == 'registry':
        return brayns.utils.Status(200, TEST_REGISTRY)
    return brayns.utils.Status(404, None)


def mock_rpc_request(self, method, params=None, response_timeout=5):
    return True


def mock_rpc_request_object_commit(self, method, params=None, response_timeout=5):
    assert_equal(method, 'set-test-object')
    import copy
    obj = copy.deepcopy(TEST_OBJECT)
    obj['integer'] = 42
    assert_equal(obj, params)
    return True


def mock_snapshot(format, size, animation_parameters=None, camera=None, name=None, quality=None,
                  renderer=None, samples_per_pixel=None, response_timeout=None):
    if format == 'png':
        return {'data': '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'}
    if format == 'jpg':
        return {'data': 'not base 64'}
    return {
        'code': 'wrong format',
        'message': 'bad'
    }


def test_init():
    with patch('brayns.utils.http_request', new=mock_http_request):
        app = brayns.Client('localhost:8200')
        assert_equal(app.url(), 'http://localhost:8200/')
        assert_equal(app.version.as_dict(), TEST_VERSION)
        assert_equal(str(app), 'Brayns version 0.7.0.12345 running on http://localhost:8200/')


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


def test_object_properties():
    with patch('brayns.utils.http_request', new=mock_http_request):
        app = brayns.Client('localhost:8200')
        assert_equal(app.test_object.integer, TEST_OBJECT['integer'])
        assert_equal(app.test_object.number, TEST_OBJECT['number'])
        assert_equal(app.test_object.string, TEST_OBJECT['string'])
        assert_equal(app.test_object.boolean, TEST_OBJECT['boolean'])


def test_object_properties_enum():
    with patch('brayns.utils.http_request', new=mock_http_request):
        app = brayns.Client('localhost:8200')
        assert_equal(app.test_object.enum, TEST_OBJECT['enum'])
        assert_true(hasattr(app, 'ENUM_VALUE_A'))
        assert_true(hasattr(app, 'ENUM_VALUE_B'))


def test_object_properties_enum_with_title():
    with patch('brayns.utils.http_request', new=mock_http_request):
        app = brayns.Client('localhost:8200')
        assert_equal(app.test_object.enum_title, TEST_OBJECT['enum_title'])
        assert_true(hasattr(app, 'MY_ENUM_MINE'))
        assert_true(hasattr(app, 'MY_ENUM_YOURS'))


def test_object_properties_enum_array():
    with patch('brayns.utils.http_request', new=mock_http_request):
        app = brayns.Client('localhost:8200')
        assert_equal(app.test_object.enum_array, TEST_OBJECT['enum_array'])

        assert_true(hasattr(app, 'ENUM_ARRAY_ONE'))
        assert_true(hasattr(app, 'ENUM_ARRAY_TWO'))
        assert_true(hasattr(app, 'ENUM_ARRAY_THREE'))


def test_object_properties_array():
    with patch('brayns.utils.http_request', new=mock_http_request):
        app = brayns.Client('localhost:8200')
        assert_equal(app.test_object.array, TEST_OBJECT['array'])


@raises(AttributeError)
def test_object_replace():
    with patch('brayns.utils.http_request', new=mock_http_request):
        app = brayns.Client('localhost:8200')
        app.test_object = [1,2,3]


def test_object_commit():
    with patch('brayns.utils.http_request', new=mock_http_request), \
            patch('brayns.RpcClient.request', new=mock_rpc_request_object_commit):
        app = brayns.Client('localhost:8200')
        app._ws_connected = True
        app.test_object.integer = 42
        app.test_object.commit()


def test_array():
    with patch('brayns.utils.http_request', new=mock_http_request):
        app = brayns.Client('localhost:8200')
        assert_equal(app.test_array, TEST_ARRAY)


def test_rpc_one_parameter():
    with patch('brayns.utils.http_request', new=mock_http_request), \
            patch('brayns.RpcClient.request', new=mock_rpc_request):
        app = brayns.Client('localhost:8200')
        import inspect
        assert_equal(inspect.getdoc(app.test_rpc), TEST_RPC_ONE_PARAMETER['description'])
        assert_true(app.test_rpc(doit=False, name='foo'))


def test_rpc_one_of_parameter():
    with patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.RpcClient.request', new=mock_rpc_request):
        app = brayns.Client('localhost:8200')
        import inspect
        assert_true(inspect.getdoc(app.set_camera).startswith(TEST_RPC_ONEOF_PARAMETER['description']))
        assert_true(hasattr(app, 'PerspectiveCamera'))
        param = app.PerspectiveCamera()
        param.fov = 10.2
        assert_true(app.set_camera(param))

def test_rpc_one_of_parameter_weird_casings():
    with patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.RpcClient.request', new=mock_rpc_request):
        app = brayns.Client('localhost:8200')
        assert_true(hasattr(app, 'StereofullMode'))
        assert_true(hasattr(app, 'MonoFullMode'))
        assert_true(hasattr(app, 'TruthfulMode'))
        assert_true(hasattr(app, 'MeaningFulMode'))


def test_rpc_array_parameter():
    with patch('brayns.utils.http_request', new=mock_http_request), \
            patch('brayns.RpcClient.request', new=mock_rpc_request):
        app = brayns.Client('localhost:8200')
        import inspect
        assert_true(inspect.getdoc(app.inspect).startswith(TEST_RPC_ARRAY_PARAMETER['description']))
        assert_true(app.inspect(xy=[1,2]))


def test_rpc_two_parameters():
    with patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.RpcClient.request', new=mock_rpc_request):
        app = brayns.Client('localhost:8200')
        assert_false(hasattr(app, 'test-rpc-two-params'))


def test_rpc_only_return():
    with patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.RpcClient.request', new=mock_rpc_request):
        app = brayns.Client('localhost:8200')
        import inspect
        assert_equal(inspect.getdoc(app.test_rpc_return), TEST_RPC_ONLY_RETURN['description'])
        assert_true(app.test_rpc_return())


def test_rpc_invalid_type():
    with patch('brayns.utils.http_request', new=mock_http_request), \
            patch('brayns.RpcClient.request', new=mock_rpc_request):
        app = brayns.Client('localhost:8200')
        assert_false(hasattr(app, 'test-rpc-invalid-type'))


@raises(Exception)
def test_rpc_invalid_param():
    with patch('brayns.utils.http_request', new=mock_http_request_invalid_rpc_param):
        brayns.Client('localhost:8200')


def test_image():
    with patch('brayns.utils.http_request', new=mock_http_request):
        app = brayns.Client('localhost:8200')
        setattr(app, 'snapshot', mock_snapshot)
        assert_true(app.image(size=[50,50], format='png'))


def test_image_wrong_format():
    with patch('brayns.utils.http_request', new=mock_http_request):
        app = brayns.Client('localhost:8200')
        setattr(app, 'snapshot', mock_snapshot)
        assert_false(app.image(size=[50,50], format='foo'))


@raises(TypeError)
def test_image_not_base64():
    with patch('brayns.utils.http_request', new=mock_http_request):
        app = brayns.Client('localhost:8200')
        setattr(app, 'snapshot', mock_snapshot)
        app.image(size=[50, 50], format='jpg')


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
    with patch('brayns.utils.http_request', new=mock_http_request):
        app = brayns.Client('localhost:8200')
        setattr(app, 'transfer_function', MockTransferFunction())
        app.set_colormap(colormap_size=123, data_range=(0, 42))
        assert_equal(len(app.transfer_function.contribution), 123)
        assert_equal(len(app.transfer_function.diffuse), 123)
        assert_equal(len(app.transfer_function.emission), 0)
        assert_equal(app.transfer_function.range, (0,42))
        assert_true(app.transfer_function.commit_called)


@raises(ValueError)
def test_set_colormap_unknown_colormap():
    with patch('brayns.utils.http_request', new=mock_http_request):
        app = brayns.Client('localhost:8200')
        setattr(app, 'transfer_function', MockTransferFunction())
        app.set_colormap(colormap='foo')


def mock_webbrowser_open(url):
    assert_equal(url, 'https://bbp-brayns.epfl.ch?host=http://localhost:8200/')


def test_open_ui():
    with patch('brayns.utils.http_request', new=mock_http_request), \
            patch('webbrowser.open', new=mock_webbrowser_open):
        app = brayns.Client('localhost:8200')
        app.open_ui()


if __name__ == '__main__':
    import nose
    nose.run(defaultTest=__name__)
