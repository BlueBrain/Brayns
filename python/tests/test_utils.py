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

from nose.tools import assert_equal
import brayns


def test_set_http_protocol():
    assert_equal(brayns.utils.set_http_protocol(''), 'http://')
    assert_equal(brayns.utils.set_http_protocol('foo'), 'http://foo')
    assert_equal(brayns.utils.set_http_protocol('foo:8080'), 'http://foo:8080')
    assert_equal(brayns.utils.set_http_protocol('http://foo:8080'), 'http://foo:8080')
    assert_equal(brayns.utils.set_http_protocol('https://foo:8080'), 'https://foo:8080')


if __name__ == '__main__':
    import nose
    nose.run(defaultTest=__name__)
