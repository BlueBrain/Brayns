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

from brayns.core.material.car_paint_material import CarPaintMaterial
from brayns.core.material.default_material import DefaultMaterial
from brayns.core.material.emissive_material import EmissiveMaterial
from brayns.core.material.get_material import get_material
from brayns.core.material.get_material_name import get_material_name
from brayns.core.material.glass_material import GlassMaterial
from brayns.core.material.material import Material
from brayns.core.material.matte_material import MatteMaterial
from brayns.core.material.metal_material import MetalMaterial
from brayns.core.material.plastic_material import PlasticMaterial
from brayns.core.material.set_material import set_material

__all__ = [
    'CarPaintMaterial',
    'DefaultMaterial',
    'EmissiveMaterial',
    'get_material',
    'get_material_name',
    'GlassMaterial',
    'Material',
    'MatteMaterial',
    'MetalMaterial',
    'PlasticMaterial',
    'set_material',
]
