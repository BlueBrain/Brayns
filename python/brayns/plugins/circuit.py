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

from typing import ClassVar

from brayns.network import Instance


class CircuitColorMethod:
    """Available coloring methods for circuits."""

    ID: ClassVar[str] = "id"
    ETYPE: ClassVar[str] = "etype"
    MTYPE: ClassVar[str] = "mtype"
    LAYER: ClassVar[str] = "layer"
    REGION: ClassVar[str] = "region"
    HEMISPHERE: ClassVar[str] = "hemisphere"
    MORPHOLOGY: ClassVar[str] = "morphology"
    MORPHOLOGY_CLASS: ClassVar[str] = "morphology class"
    MORPHOLOGY_SECTION: ClassVar[str] = "morphology section"
    SUBREGION: ClassVar[str] = "subregion"
    SYNAPSE_CLASS: ClassVar[str] = "synapse class"


def get_circuit_ids(instance: Instance, model_id: int) -> list[int]:
    """Retreive the list of loaded neurons / astrocytes / vasculatures by ID.

    :param instance: Instance.
    :type instance: Instance
    :param model_id: ID of the circuit model.
    :type model_id: int
    :return: ID list of the element loaded in given circuit.
    :rtype: list[int]
    """
    params = {"model_id": model_id}
    return instance.request("get-circuit-ids", params)


def set_circuit_thickness(
    instance: Instance, model_id: int, radius_multiplier: float
) -> None:
    """Multiply the radius of all primitives of a circuit by given factor.

    :param instance: Instance.
    :type instance: Instance
    :param model_id: Circuit model ID.
    :type model_id: int
    :param radius_multiplier: Scaling factor for capsules and spheres.
    :type radius_multiplier: float
    """
    params = {
        "model_id": model_id,
        "radius_multiplier": radius_multiplier,
    }
    instance.request("set-circuit-thickness", params)
