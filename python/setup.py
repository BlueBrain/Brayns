#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright 2016-2024 Blue Brain Project/EPFL
#                          Raphael Dumusc <raphael.dumusc@epfl.ch>
#                          Daniel Nachbaur <daniel.nachbaur@epfl.ch>
#                          Cyrille Favreau <cyrille.favreau@epfl.ch>
#                          Nadir Roman <nadir.romanguerrero@epfl.ch>
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

import pathlib
import pkg_resources
import setuptools

DIRECTORY = pathlib.Path(__file__).parent


def get_requirements():
    with (DIRECTORY / 'requirements.txt').open() as requirements:
        return [
            str(requirement)
            for requirement in pkg_resources.parse_requirements(requirements)
        ]


def get_readme():
    with (DIRECTORY / 'README.md').open() as readme:
        return readme.read()


setuptools.setup(
    packages=setuptools.find_packages(),
    install_requires=get_requirements(),
    description="The Brayns renderer python API",
    long_description=get_readme(),
    long_description_content_type='text/markdown',
    url='https://github.com/BlueBrain/Brayns',
    download_url='https://github.com/BlueBrain/Brayns',
    project_urls={
        'Tracker': 'https://bbpteam.epfl.ch/project/issues/projects/BRAYNS/issues',
        'Source': 'https://github.com/BlueBrain/Brayns',
    }
)
