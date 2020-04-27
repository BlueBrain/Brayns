#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2020, Blue Brain Project
#                          Nadir Román Guerrero <nadir.romanguerrero@epfl.ch>
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

RESPONSE_OK = {'ok'}


def mock_pbrvolumes_rpc_request(self, method, params=None,
                                response_timeout=None):
    if method == 'add-homogeneus-volume':
        return RESPONSE_OK
    elif method == 'add-homogeneus-volume-model':
        return RESPONSE_OK
    elif method == 'add-heterogeneus-volume':
        return RESPONSE_OK
    elif method == 'add-grid-volume':
        return RESPONSE_OK

    return None
