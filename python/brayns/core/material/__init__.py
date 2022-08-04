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
from brayns.core.material.get_car_paint_material import get_car_paint_material
from brayns.core.material.get_default_material import get_default_material
from brayns.core.material.get_emissive_material import get_emissive_material
from brayns.core.material.get_glass_material import get_glass_material
from brayns.core.material.get_matte_material import get_matte_material
from brayns.core.material.get_metal_material import get_metal_material
from brayns.core.material.get_plastic_material import get_plastic_material
from brayns.core.material.glass_material import GlassMaterial
from brayns.core.material.material import Material
from brayns.core.material.matte_material import MatteMaterial
from brayns.core.material.metal_material import MetalMaterial
from brayns.core.material.plastic_material import PlasticMaterial

__all__ = [
    'CarPaintMaterial',
    'DefaultMaterial',
    'EmissiveMaterial',
    'get_car_paint_material',
    'get_default_material',
    'get_emissive_material',
    'get_glass_material',
    'get_matte_material',
    'get_metal_material',
    'get_plastic_material',
    'GlassMaterial',
    'Material',
    'MatteMaterial',
    'MetalMaterial',
    'PlasticMaterial',
]
