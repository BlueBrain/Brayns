#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2020, Blue Brain Project
# Nadir Román Guerrero <nadir.romanguerrero@epfl.ch>
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
# All rights reserved. Do not distribute without further notice

import brayns
from brayns.plugins.pbr_volumes import PBRVolumes

from .mocks import mock_connected
from .mocks import mock_http_request
from .mocks import mock_not_in_notebook
from .mocks import mock_batch
from .mocks_pbr_volumes import mock_pbrvolumes_rpc_request

from nose.tools import assert_equal

from mock import patch


def test_add_homogeneus_volume():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_pbrvolumes_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        pbrv = PBRVolumes(app)
        response = pbrv.add_homogeneus_volume(sig_a=(0, 0, 0), sig_s=(0, 0, 0))
        assert_equal(response, {'ok'})


def test_add_homogeneus_volume_to_model():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_pbrvolumes_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        pbrv = PBRVolumes(app)
        response = pbrv.add_homogeneus_volume_to_model(model_id=0, sig_a=(0, 0, 0),
                                                       sig_s=(0, 0, 0))
        assert_equal(response, {'ok'})


def test_add_heterogeneus_volume():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_pbrvolumes_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        pbrv = PBRVolumes(app)
        response = pbrv.add_heterogeneus_volume(sig_a=(0, 0, 0), sig_s=(0, 0, 0),
                                                min_density=0, max_density=0)
        assert_equal(response, {'ok'})


def test_add_grid_volume():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_pbrvolumes_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        pbrv = PBRVolumes(app)
        response = pbrv.add_grid_volume(sig_a=(0, 0, 0), sig_s=(0, 0, 0),
                                        density=list(), nx=0, ny=0, nz=0)
        assert_equal(response, {'ok'})
