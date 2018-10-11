#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2018, Blue Brain Project
#                     Daniel Nachbaur <daniel.nachbaur@epfl.ch>
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
import json
import websockets

import brayns
import rockets

from nose.tools import assert_true, assert_false, assert_equal, raises
from mock import patch
from .mocks import *

send_message = asyncio.Future()
close_server = asyncio.Future()

async def hello(websocket, path):
    await send_message
    notification = rockets.Notification('set-test-object', {'string': 'changed'})
    await websocket.send(str(notification.json))
    await close_server

server_url = None
def setup():
    start_server = websockets.serve(hello, 'localhost')
    server = asyncio.get_event_loop().run_until_complete(start_server)
    global server_url
    server_url = 'localhost:'+str(server.sockets[0].getsockname()[1])


def test_notifications():
    with patch('brayns.utils.http_request', new=mock_http_request), \
         patch('rockets.Client.request', new=mock_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client(server_url)

        def _on_message(message):
            assert_equal(app.test_object.string, 'changed')
            close_server.set_result(True)
            asyncio.get_event_loop().stop()
        app.rockets_client.ws_observable.subscribe(_on_message)

        assert_equal(app.test_object.string, 'foobar')

        send_message.set_result(True)
        asyncio.get_event_loop().run_forever()


if __name__ == '__main__':
    import nose
    nose.run(defaultTest=__name__)
