# Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

import brayns
from testapi.render import render_and_validate
from testapi.simple_test_case import SimpleTestCase


class TestProtein(SimpleTestCase):
    def test_load_models(self) -> None:
        path = self.asset_folder / "1bna.pdb"
        loader = brayns.ProteinLoader(
            color_scheme=brayns.ProteinColorScheme.BY_ID,
            radius_multiplier=1,
        )
        models = loader.load_models(self.instance, str(path))
        self.assertEqual(len(models), 1)
        render_and_validate(self, "protein")
