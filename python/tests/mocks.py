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

from nose.tools import assert_equal


TEST_VERSION = {
    'major': 1,
    'minor': 1,
    'patch': 0,
    'revision': 12345
}

TEST_RPC_ONE_PARAMETER = {
    'title': 'test-request-single-arg',
    'description': 'Pass on parameter to brayns',
    'async': False,
    'type': 'method',
    'returns': {
        'anyOf': [{
                'type': 'null'
            }, {
                'type': 'string'
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

TEST_RPC_ONE_PARAMETER_NO_RETURN = {
    'title': 'test-notify-single-arg',
    'description': 'Pass on parameter to brayns',
    'async': False,
    'type': 'method',
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
    'async': False,
    'type': 'object'
}

TEST_RPC_NO_TYPE = {
    'title': 'test-rpc-no-type',
    'description': 'Must have a type',
    'async': False
}

TEST_RPC_INVALID_PARAM = {
    'title': 'test-rpc-invalid-param',
    'description': 'Only objects for params, no single values',
    'async': False,
    'type': 'method',
    'params': [{'type': 'string'}]
}

TEST_RPC_TWO_PARAMETERS = {
    'title': 'test-rpc-two-params',
    'description': 'Pass on two parameters to brayns',
    'async': False,
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
    'async': False,
    'type': 'method',
    'returns': {
        'type': 'integer'
    }
}

TEST_RPC_NOTIFICATION = {
    'title': 'test-notification',
    'description': 'Just does something, return nothing',
    'async': False,
    'type': 'method'
}

TEST_RPC_ONEOF_PARAMETER = {
    'title': 'set-camera',
    'description': 'Pass on oneOf parameter to brayns',
    'async': False,
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
    }],
    'returns': {
        'type': 'number'
    }
}

TEST_RPC_ONEOF_PARAMETER_NO_RETURN = {
    'title': 'set-camera-no-return',
    'description': 'Pass on oneOf parameter to brayns',
    'async': False,
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
    'async': False,
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
    'async': False,
    'type': 'method',
    'params': [{
        'type': 'array',
        'name': 'xy',
        'description': 'nice xy position'
    }],
    'returns': {
        'type': 'boolean'
    }
}

TEST_RPC_ARRAY_PARAMETER_NO_RETURN = {
    'title': 'inspect-notify',
    'description': 'Pass on array parameter to brayns',
    'async': False,
    'type': 'method',
    'params': [{
        'type': 'array',
        'name': 'xy',
        'description': 'nice xy position'
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

TEST_ADD_MODEL = 'OK'


VERSION_SCHEMA = {
    'title': 'Version',
    'type': 'object'
}

TEST_REGISTRY = {
    'test-request-single-arg/schema': ['GET'],
    'test-notify-single-arg/schema': ['GET'],
    'set-camera/schema': ['GET'],
    'set-camera-no-return/schema': ['GET'],
    'set-mode/schema': ['GET'],
    'inspect/schema': ['GET'],
    'inspect-notify/schema': ['GET'],
    'test-rpc-invalid-type/schema': ['GET'],
    'test-rpc-no-type/schema': ['GET'],
    'test-rpc-return/schema': ['GET'],
    'test-notification/schema': ['GET'],
    'test-rpc-two-params/schema': ['GET'],
    'test-array': ['GET', 'PUT'],
    'test-array/schema': ['GET'],
    'test-object': ['GET', 'PUT'],
    'test-object/schema': ['GET'],
    'add-model/schema': ['GET'],
    'version': ['GET']
}


def mock_batch(self, requests, response_timeout=None, make_async=False):
    mapping = {
        'test-request-single-arg': TEST_RPC_ONE_PARAMETER,
        'test-notify-single-arg': TEST_RPC_ONE_PARAMETER_NO_RETURN,
        'set-camera': TEST_RPC_ONEOF_PARAMETER,
        'set-camera-no-return': TEST_RPC_ONEOF_PARAMETER_NO_RETURN,
        'set-mode': TEST_RPC_ONEOF_PARAMETER_WEIRD_CASING,
        'inspect': TEST_RPC_ARRAY_PARAMETER,
        'inspect-notify': TEST_RPC_ARRAY_PARAMETER_NO_RETURN,
        'test-rpc-invalid-type': TEST_RPC_INVALID_TYPE,
        'test-rpc-no-type': TEST_RPC_NO_TYPE,
        'test-rpc-two-params': TEST_RPC_TWO_PARAMETERS,
        'test-rpc-return': TEST_RPC_ONLY_RETURN,
        'test-notification': TEST_RPC_NOTIFICATION,
        'test-object': TEST_OBJECT_SCHEMA,
        'test-array': TEST_ARRAY_SCHEMA,
        'add-model': TEST_ADD_MODEL,
        'version': VERSION_SCHEMA
    }
    if make_async:
        import copy
        mapping = copy.deepcopy(mapping)
        for i in mapping.values():
            if 'async' in i:
                i['async'] = True
    results = list()
    for request in requests:
        schema = request.params['endpoint']
        request_id = request.request_id()
        if schema in mapping:
            results.append(rockets.Response(result=mapping[schema], _id=request_id))
        else:
            results.append(rockets.Response(error={'code': -42, 'message': 'Invalid stuff'}, _id=request_id))
    return results


def mock_batch_request_invalid_rpc_param(self, requests, response_timeout=None, make_async=False):
    results = list()
    for request in requests:
        schema = request.params['endpoint']
        request_id = request.request_id()
        if schema == 'test-rpc-invalid-param':
            results.append(rockets.Response(result=TEST_RPC_INVALID_PARAM, _id=request_id))
    return results


async def mock_batch_async(self, requests, response_timeout=None):
    return mock_batch(self, requests, response_timeout, True)


async def mock_batch_async_with_sync_methods(self, requests, response_timeout=None):
    return mock_batch(self, requests, response_timeout, False)


def mock_http_request(method, url, command, body=None, query_params=None):
    if command == 'version':
        return brayns.utils.Status(200, TEST_VERSION)
    if command == 'registry':
        return brayns.utils.Status(200, TEST_REGISTRY)
    return brayns.utils.Status(404, None)


def mock_http_request_wrong_version(method, url, command, body=None, query_params=None):
    if command == 'version':
        import copy
        version = copy.deepcopy(TEST_VERSION)
        version['major'] = 0
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
    if command == 'version':
        return brayns.utils.Status(200, TEST_VERSION)
    if command == 'registry':
        return brayns.utils.Status(200, {'test-rpc-invalid-param/schema': ['GET']})
    return brayns.utils.Status(404, None)


def mock_rpc_request(self, method, params=None, response_timeout=None):
    if method == 'get-test-object':
        return TEST_OBJECT
    if method == 'get-test-array':
        return TEST_ARRAY
    if method == 'get-version':
        return TEST_VERSION
    if method == 'set-test-object':
        import copy
        obj = copy.deepcopy(TEST_OBJECT)
        obj['integer'] = 42
        assert_equal(obj, params)
        return True
    if method == 'test-request-single-arg':
        if params['doit']:
            return params['name']
        return None
    if method == 'inspect':
        return params == [1,2]
    if method == 'test-rpc-return':
        return 42
    if method == 'set-camera':
        if 'fov' in params:
            return params['fov']
        return 0
    if method == 'add-model':
        return True
    return None


async def mock_rpc_async_request(self, method, params=None):
    return mock_rpc_request(self, method, params)


def mock_rpc_notify(self, method, params=None, response_timeout=None):
    pass


async def mock_rpc_async_notify(self, method, params=None):
    pass


def mock_connected(self):
    return True


def mock_snapshot(format, size, animation_parameters=None, camera=None, name=None, quality=None,
                  renderer=None, samples_per_pixel=None):
    if format == 'png':
        return {'data': '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'}
    if format == 'jpg':
        return {'data': 'not base 64'}
    return {
        'code': 'wrong format',
        'message': 'bad'
    }


def mock_async_snapshot(*args, **kwargs):
    async def bla(*args, **kwargs):
        return mock_snapshot(*args, **kwargs)
    return asyncio.ensure_future(bla(*args, **kwargs))


def mock_not_in_notebook():
    return False


def mock_add_model(name, path, loader_properties):
    return True


def mock_plugin_dti_add_streamlines(name, streamlines, radius=1.0, opacity=1.0):
    return {'ok'}
