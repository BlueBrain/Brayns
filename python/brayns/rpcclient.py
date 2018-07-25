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

"""
The RpcClient manages a websocket connection to handle messaging with a remote Brayns instance.

It runs in a thread and provides methods to send notifications and requests in JSON-RPC format.
"""

import json
import threading
import time
import websocket

from .utils import set_http_protocol, set_ws_protocol, WS_PATH, underscorize

JSON_RPC_VERSION = '2.0'


class RpcClient(object):
    """
    The RpcClient manages a websocket connection to handle messaging with a remote Brayns instance.

    It runs in a thread and provides methods to send notifications and requests in JSON-RPC format.
    """

    def __init__(self, url):
        """
        Initialize the RpcClient, but don't setup the websocket connection yet.

        Convert the URL to a proper format and initialize the state of the client. Does not
        establish the websocket connection yet. This will be postponed to either the first notify
        or request.

        :param str url: The address of the remote running Brayns instance.
        """
        self._url = set_http_protocol(url) + '/'

        self._ws = None
        self._ws_connected = False
        self._request_id = 0
        self._update_callback = {}
        self._ws_requests = {}

    def url(self):
        """
        Returns the address of the remote running Brayns instance.

        :return: The address of the remote running Brayns instance.
        :rtype: str
        """
        return self._url

    def connected(self):
        """Returns true if the websocket is connected to the remote Brayns instance."""
        return self._ws_connected

    def request(self, method, params=None, response_timeout=5):  # pragma: no cover
        """
        Invoke an RPC on the remote running Brayns instance and wait for its reponse.

        :param str method: name of the method to invoke
        :param dict params: params for the method
        :param int response_timeout: number of seconds to wait for the response
        :return: result or error of RPC
        :rtype: dict
        :raises Exception: if request was not answered within given response_timeout
        """
        data = dict()
        data['jsonrpc'] = JSON_RPC_VERSION
        data['id'] = self._request_id
        data['method'] = method
        if params:
            data['params'] = params

        result = {'done': False, 'result': None}

        def callback(payload):
            """
            The callback for the response.

            :param dict payload: the actual response data
            """
            result['result'] = payload
            result['done'] = True

        self._ws_requests[self._request_id] = callback
        self._request_id += 1

        self._setup_websocket()
        self._ws.send(json.dumps(data))

        if response_timeout:
            timeout = response_timeout * 10

            while not result['done'] and timeout:
                time.sleep(0.1)
                timeout -= 1

            if 'done' not in result:
                raise Exception('Request was not answered within {0} seconds'
                                .format(response_timeout))
        else:
            while not result['done']:
                time.sleep(0.0001)

        return result['result']

    def notify(self, method, params=None):  # pragma: no cover
        """
        Invoke an RPC on the remote running Brayns instance without waiting for a response.

        :param str method: name of the method to invoke
        :param str params: params for the method
        """
        data = dict()
        data['jsonrpc'] = JSON_RPC_VERSION
        data['method'] = method
        if params:
            data['params'] = params

        self._setup_websocket()
        self._ws.send(json.dumps(data))

    def _setup_websocket(self):  # pragma: no cover
        """
        Setups websocket to handle binary (image) and text (all properties) messages.

        The websocket app runs in a separate thread to unblock all notebook cells.
        """
        if self._ws_connected:
            return

        def on_open(ws):
            # pylint: disable=unused-argument
            """Websocket is open, remember this state."""
            self._ws_connected = True

        def on_data(ws, data, data_type, cont):
            # pylint: disable=unused-argument
            """Websocket received data, handle it."""
            if data_type == websocket.ABNF.OPCODE_TEXT:
                data = json.loads(data)

                # check if the received data is a response from a previous request
                if self._handle_response(data):
                    return

                # if the received data is a change notification from any known object
                if not data['method'].startswith('set-'):
                    return

                # remove the 'set-' part of the data method to find the property to update its
                # content
                prop_name = underscorize(data['method'][4:])
                prop = getattr(self, '_' + prop_name, None)
                if prop is None:
                    return

                prop.__init__(**data['params'])

                # call an optionally registered callback on the newly updated property
                if data['method'] in self._update_callback:
                    self._update_callback[data['method']](data=prop)
            elif data_type == websocket.ABNF.OPCODE_BINARY:
                if 'image-jpeg' in self._update_callback:
                    self._update_callback['image-jpeg'](data=data)

        def on_close(ws):
            # pylint: disable=unused-argument
            """Websocket is closing, notify all registered callbacks to e.g. close widgets."""
            self._ws_connected = False
            for f in self._update_callback.values():
                f(close=True)
            self._update_callback.clear()

        websocket.enableTrace(False)

        self._ws = websocket.WebSocketApp(set_ws_protocol(self._url) + WS_PATH,
                                          subprotocols=['rockets'], on_open=on_open,
                                          on_data=on_data, on_close=on_close)
        ws_thread = threading.Thread(target=self._ws.run_forever)
        ws_thread.daemon = True
        ws_thread.start()

        conn_timeout = 5
        while not self._ws_connected and conn_timeout:
            time.sleep(0.2)
            conn_timeout -= 1

    def _handle_response(self, data):  # pragma: no cover
        """
        Handle a potential JSON-RPC response message.

        :param dict data: data of the reply
        :return: True if a request was handled, False otherwise
        :rtype: bool
        """
        if 'id' not in data:
            return False

        payload = None
        if 'result' in data:
            payload = None if data['result'] == '' or data['result'] == 'OK' else data['result']
        elif 'error' in data:
            payload = data['error']

        if data['id'] in self._ws_requests:
            self._ws_requests[data['id']](payload)
            self._ws_requests.pop(data['id'])
        else:
            print('Invalid reply for request ' + str(data['id']))
        return True
