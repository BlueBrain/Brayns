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

import sys
from collections import OrderedDict
import requests


HTTP_METHOD_PUT = 'PUT'
HTTP_METHOD_GET = 'GET'
HTTP_METHOD_DELETE = 'DELETE'
HTTP_METHOD_POST = 'POST'
HTTP_STATUS_OK = 200

HTTP_PREFIX = 'http://'
HTTPS_PREFIX = 'https://'
WS_PREFIX = 'ws://'
WSS_PREFIX = 'wss://'

WS_PATH = '/ws'

SCHEMA_ENDPOINT = '/schema'


class Status(object):
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
                request = requests.post(full_url, params=query_params)
            else:
                request = requests.post(full_url, data=body, params=query_params)
        elif method == HTTP_METHOD_PUT:
            if body == '':
                request = requests.put(full_url, params=query_params)
            else:
                request = requests.put(full_url, data=body, params=query_params)
        elif method == HTTP_METHOD_GET:
            request = requests.get(full_url, params=query_params)
            if request.status_code == 502:
                raise requests.exceptions.ConnectionError('Bad Gateway 502')
        elif method == HTTP_METHOD_DELETE:
            if body == '':
                request = requests.delete(full_url, params=query_params)
            else:
                request = requests.delete(full_url, data=body, params=query_params)
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


def set_ws_protocol(url):
    """
    Set the WebSocket protocol according to the resource url.

    :param str url: Url to be checked
    :return: Url preprend with ws for http, wss for https
    :rtype: str
    """
    if url.find(HTTPS_PREFIX) != -1:
        return WSS_PREFIX + url[len(HTTPS_PREFIX):]
    if url.find(HTTP_PREFIX) != -1:
        return WS_PREFIX + url[len(HTTP_PREFIX):]
    return WS_PREFIX + url


def underscorize(word):
    """
    Opposite of inflection.dasherize; replace dashes with underscore in the word.

    :param str word: the word to underscorize
    :return: the underscorized word
    :rtype: str
    """
    return word.replace('-', '_')
