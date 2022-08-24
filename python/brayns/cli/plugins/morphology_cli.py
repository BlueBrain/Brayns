# Copyright (c) 2015-2022 EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# Responsible Author: adrien.fleury@epfl.ch
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

import argparse
from dataclasses import dataclass

from brayns.plugins import GeometryType, Morphology

from ..cli import Cli
from ..utils import TRUE_FALSE, boolean


@dataclass
class MorphologyCli(Cli):

    radius_multiplier: float = 10.0
    constant_radius: bool = False
    load_soma: bool = True
    load_dendrites: bool = False
    load_axon: bool = False

    def register(self, parser: argparse.ArgumentParser) -> None:
        parser.add_argument(
            '--radius_multiplier',
            type=float,
            default=self.radius_multiplier,
            metavar='VALUE',
            help='Multiply geometries radius by this value',
        )
        parser.add_argument(
            '--constant_radius',
            type=boolean,
            choices=TRUE_FALSE,
            default=self.constant_radius,
            help='Use constant radius of radius_multiplier for all geometries',
        )
        parser.add_argument(
            '--load_soma',
            type=boolean,
            choices=TRUE_FALSE,
            default=self.load_soma,
            help='Wether to load morphologies somas or not',
        )
        parser.add_argument(
            '--load_dendrites',
            type=boolean,
            choices=TRUE_FALSE,
            default=self.load_dendrites,
            help='Wether to load morphologies dendrites or not',
        )
        parser.add_argument(
            '--load_axon',
            type=boolean,
            choices=TRUE_FALSE,
            default=self.load_axon,
            help='Wether to load morphologies axons or not',
        )

    def load(self, args: argparse.Namespace) -> None:
        self.radius_multiplier = args.radius_multiplier
        self.constant_radius = args.constant_radius
        self.load_soma = args.load_soma
        self.load_dendrites = args.load_dendrites
        self.load_axon = args.load_axon

    def create_morphology(self) -> Morphology:
        geometry_type = GeometryType.SMOOTH
        if self.constant_radius:
            geometry_type = GeometryType.CONSTANT_RADII
        return Morphology(
            radius_multiplier=self.radius_multiplier,
            load_soma=self.load_soma,
            load_axon=self.load_axon,
            load_dendrites=self.load_dendrites,
            geometry_type=geometry_type,
        )
