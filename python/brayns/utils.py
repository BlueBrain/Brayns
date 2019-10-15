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

"""Utils module for http request and notebook stuff."""

import base64
import io
import sys
from collections import OrderedDict
from functools import wraps
from PIL import Image
import requests
import rockets


HTTP_METHOD_PUT = 'PUT'
HTTP_METHOD_GET = 'GET'
HTTP_METHOD_DELETE = 'DELETE'
HTTP_METHOD_POST = 'POST'
HTTP_STATUS_OK = 200
HTTP_RESPONSE_TIMEOUT = 3600

HTTP_PREFIX = 'http://'
HTTPS_PREFIX = 'https://'

WS_PATH = '/ws'

SCHEMA_ENDPOINT = '/schema'


class Status:
    """Holds the execution status of an HTTP request."""

    def __init__(self, code, contents):
        """Initialize status object with given code and contents."""
        self.code = code
        self.contents = contents


# pylint: disable=R0912
def http_request(method, url, command, body=None, query_params=None):  # pragma: no cover
    """
    Perform http requests to the given URL and return the applications' response.

    :param str method: the type of HTTP request, PUT or GET are supported
    :param str url: the URL of the applications' http server
    :param str body: optional body for PUT requests
    :param str command: the type of HTTP command to be executed on the target app
    :param str query_params: the query params to append to the request url
    :return: JSON-encoded response of the request
    :rtype: Status
    :raises Exception: on connection error
    """
    full_url = url
    request = None
    full_url += command
    try:
        if method == HTTP_METHOD_POST:
            if body == '':
                request = requests.post(full_url, params=query_params,
                                        timeout=HTTP_RESPONSE_TIMEOUT)
            else:
                request = requests.post(full_url, data=body, params=query_params,
                                        timeout=HTTP_RESPONSE_TIMEOUT)
        elif method == HTTP_METHOD_PUT:
            if body == '':
                request = requests.put(full_url, params=query_params, timeout=HTTP_RESPONSE_TIMEOUT)
            else:
                request = requests.put(full_url, data=body, params=query_params,
                                       timeout=HTTP_RESPONSE_TIMEOUT)
        elif method == HTTP_METHOD_GET:
            request = requests.get(full_url, params=query_params, timeout=HTTP_RESPONSE_TIMEOUT)
            if request.status_code == 502:
                raise requests.exceptions.ConnectionError('Bad Gateway 502')
        elif method == HTTP_METHOD_DELETE:
            if body == '':
                request = requests.delete(full_url, params=query_params,
                                          timeout=HTTP_RESPONSE_TIMEOUT)
            else:
                request = requests.delete(full_url, data=body, params=query_params,
                                          timeout=HTTP_RESPONSE_TIMEOUT)
        js = ''
        if request.content:
            if request.status_code == 200:
                js = request.json(object_pairs_hook=OrderedDict)
            else:
                js = request.text
        response = Status(request.status_code, js)
        request.close()
    except requests.exceptions.ConnectionError:
        raise Exception('ERROR: Failed to connect to Brayns, did you start it with the '
                        '--http-server command line option?')
    return response


def in_notebook():
    """
    Check we are running in a notebook or not.

    :return: ``True`` if the module is running in IPython kernel, ``False`` if in IPython shell or
    other Python shell.
    :rtype: bool
    """
    return 'ipykernel' in sys.modules


def set_http_protocol(url):
    """
    Set the http protocol to the url if it is not present.

    :param str url: Url to be checked
    :return: Url with protocol
    :rtype: str
    """
    if url.find(HTTP_PREFIX) == -1 and url.find(HTTPS_PREFIX) == -1:
        return HTTP_PREFIX + url
    return url


def underscorize(word):
    """
    Opposite of inflection.dasherize; replace dashes with underscore in the word.

    :param str word: the word to underscorize
    :return: the underscorized word
    :rtype: str
    """
    return word.replace('-', '_')


def add_method(cls, name, description):
    """
    Decorator that adds the decorated function with the given name and docstring to cls.

    :param object cls: the python class to the decorated function to
    :param str name: the name of the function
    :param str description: the docstring of the function
    :return: the decorator
    :rtype: decorator
    """
    def _decorator(func):
        func.__doc__ = description

        @wraps(func)
        def _wrapper(self, *args, **kwargs):
            return func(self, *args, **kwargs)
        setattr(cls, name, _wrapper)
        return func
    return _decorator


def add_progress_cancel_widget(func):  # pragma: no cover
    """
    Decorator that adds progress widget and cancel button to func that returns a RequestTask.

    :param funcion func: the async function to decorate
    :return: the decorator
    :rtype: decorator
    """
    @wraps(func)
    def _wrapper(self, *args, **kwargs):  # pylint: disable=too-many-locals
        result = func(self, *args, **kwargs)

        if isinstance(result, rockets.RequestTask) and in_notebook():
            from ipywidgets import FloatProgress, Label, HBox, VBox, Button
            from IPython.display import display

            progress = FloatProgress(min=0, max=1, value=0)
            label = Label(value='')
            button = Button(description='Cancel')
            box = VBox([label, HBox([progress, button])])
            display(box)

            def _on_cancel(value):  # pylint: disable=unused-argument
                result.cancel()

            def _on_progress(value):
                progress.value = value.amount
                label.value = value.operation

            def _on_done(task):  # pylint: disable=unused-argument
                box.close()

            button.on_click(_on_cancel)
            result.add_progress_callback(_on_progress)
            result.add_done_callback(_on_done)

        return result
    return _wrapper


def obtain_registry(url):
    """Obtain the registry of exposed objects and RPCs from Brayns."""
    status = http_request(HTTP_METHOD_GET, url, 'registry')
    if status.code != HTTP_STATUS_OK:
        raise Exception('Failed to obtain registry from Brayns')
    return status.contents


def build_schema_requests_from_registry(url):
    """Obtain the registry and return it alongside with a list of schema requests."""
    registry = obtain_registry(url)
    endpoints = {x.replace(SCHEMA_ENDPOINT, '') for x in registry}

    request_list = list()
    for endpoint in endpoints:
        request_list.append(rockets.Request('schema', {'endpoint': endpoint}))
    return registry, request_list


def convert_snapshot_response_to_PIL(response):
    """Convert the snapshot response from Brayns to a PIL image"""
    if not response:  # pragma: no cover
        return None

    # error case: invalid request/parameters
    if 'code' in response:
        print(response['message'])
        return None
    return Image.open(io.BytesIO(base64decode(response['data'])))


def base64decode(data):
    """Properly decode the given base64 string"""
    # https://stackoverflow.com/a/9807138
    missing_padding = len(data) % 4
    if missing_padding != 0:
        data += b'=' * (4 - missing_padding)
    return base64.b64decode(data)
