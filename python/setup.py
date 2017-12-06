#!/usr/bin/env python
# -*- coding: utf-8 -*-
# pylint: disable=R0801

# Copyright (c) 2016-2017, Blue Brain Project
#                          Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns
# <https://github.com/BlueBrain/Brayns>
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

"""setup.py"""
import os

from setuptools import setup  # pylint:disable=E0611,F0401
from brayns.version import VERSION
from pip.req import parse_requirements

BASEDIR = os.path.dirname(os.path.abspath(__file__))

install_reqs = parse_requirements(os.path.join(BASEDIR, "requirements.txt"), session=False)
REQS = [str(ir.req) for ir in install_reqs]

EXTRA_REQS_PREFIX = 'requirements_'
EXTRA_REQS = {}
for file_name in os.listdir(BASEDIR):
    if not file_name.startswith(EXTRA_REQS_PREFIX):
        continue
    base_name = os.path.basename(file_name)
    (extra, _) = os.path.splitext(base_name)
    extra = extra[len(EXTRA_REQS_PREFIX):]
    EXTRA_REQS[extra] = parse_requirements(file_name, session=False)

setup(name="brayns",
      version=VERSION,
      description="A python client to interact with Brayns through its http REST interface",
      packages=['brayns'],
      url='https://github.com/bluebrain/brayns.git',
      author='Cyrille Favreau',
      author_email='cyrille.favreau@epfl.ch',
      license='GNU LGPL',
      install_requires=REQS,
      extras_require=EXTRA_REQS,)
