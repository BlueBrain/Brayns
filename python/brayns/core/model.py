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

from __future__ import annotations

from dataclasses import dataclass
from typing import Any

from brayns.network import Instance
from brayns.utils import (
    Bounds,
    Transform,
    deserialize_bounds,
    deserialize_transform,
    serialize_transform,
)


@dataclass
class Model:
    """Loaded model.

    All models are loaded without transform (identity) but it doesn't mean that
    their center of mass is at the origin (depends on the source file).

    Model metadata are a str -> str map and depends on the model type.

    :param id: Model ID.
    :type id: int
    :param type: Model type.
    :type type: str
    :param bounds: Model bounding box.
    :type bounds: Bounds
    :param info: Intrinsic nodel information.
    :type info: dict[str, Any]
    :param visible: Check wether the model is rendered or not.
    :type visible: bool
    :param transform: Model transform relative to the origin.
    :type transform: Transform
    """

    id: int
    type: str
    bounds: Bounds
    info: dict[str, Any]
    visible: bool
    transform: Transform


@dataclass
class Scene:
    """Contains all existing models and scene boundaries.

    Scene bounds take models and lights into account.

    :param bounds: Global bounds.
    :type bounds: Bounds
    :param models: List of all existing models.
    :type models: list[Model]
    """

    bounds: Bounds
    models: list[Model]


def get_model(instance: Instance, id: int) -> Model:
    """Retreive the model with given ID from an instance.

    :param instance: Instance.
    :type instance: Instance
    :param id: Model ID.
    :type id: int
    :return: Model object.
    :rtype: Model
    """
    result = instance.request("get-model", {"id": id})
    return deserialize_model(result)


def update_model(
    instance: Instance,
    model_id: int,
    transform: Transform | None = None,
    visible: bool | None = None,
) -> Model:
    """Modify the properties of a given model and return its updated version.

    Parameters left as None will remain in their current state.

    :param instance: Instance.
    :type instance: Instance
    :param model_id: ID of the model to update.
    :type model_id: int
    :param transform: Model transformation, defaults to None
    :type transform: Transform | None, optional
    :param visible: Model visibility, defaults to None
    :type visible: bool | None, optional
    :return: Updated model.
    :rtype: Model
    """
    properties = dict[str, Any]()
    if transform is not None:
        properties["transform"] = serialize_transform(transform)
    if visible is not None:
        properties["is_visible"] = visible
    params = {
        "model_id": model_id,
        "model": properties,
    }
    result = instance.request("update-model", params)
    return deserialize_model(result)


def remove_models(instance: Instance, ids: list[int]) -> None:
    """Remove the given models from an instance.

    :param instance: Instance.
    :type instance: Instance
    :param ids: ID(s) of the models to remove.
    :type ids: list[int]
    """
    params = {"ids": ids}
    instance.request("remove-model", params)


def clear_models(instance: Instance) -> None:
    """Clear all models from the given instance.

    :param instance: Instance.
    :type instance: Instance
    """
    instance.request("clear-models")


def clear_renderables(instance: Instance) -> None:
    """Clear all models that can be rendered from the given instance.

    Renderables exclude lights and clipping geometries.

    :param instance: Instance.
    :type instance: Instance
    """
    instance.request("clear-renderables")


def get_scene(instance: Instance) -> Scene:
    """Retreive all models and the world boundaries from an instance.

    :param instance: Instance.
    :type instance: Instance
    :return: Models and global bounds.
    :rtype: Scene
    """
    result = instance.request("get-scene")
    return _deserialize_scene(result)


def get_bounds(instance: Instance) -> Bounds:
    """Retreive the scene boundary of an instance.

    The scene boundaries are computed from all existing lights and models
    in the given instance.

    :param instance: Instance.
    :type instance: Instance
    :return: Bounds of the current scene.
    :rtype: Bounds
    """
    return get_scene(instance).bounds


def get_models(instance: Instance) -> list[Model]:
    """Retreive all models from an instance.

    :param instance: Instance.
    :type instance: Instance
    :return: List of models.
    :rtype: list[Model]
    """
    return get_scene(instance).models


def instantiate_model(
    instance: Instance, model_id: int, transforms: list[Transform]
) -> list[Model]:
    """Create instances of the given model and return them.

    The amount of new instances is equal to the number of transforms passed.

    :param instance: Instance.
    :type instance: Instance
    :param model_id: ID of the model to instantiate.
    :type model_id: int
    :param transforms: List of new instances transformations.
    :type transforms: list[Transform]
    :return: New instance model.
    :rtype: Model
    """

    if len(transforms) == 0:
        raise ValueError("transforms list cannot be empty")

    params: dict[str, Any] = {
        "model_id": model_id,
        "transforms": [serialize_transform(transform) for transform in transforms],
    }

    models = instance.request("instantiate-model", params)
    return [deserialize_model(model) for model in models]


def deserialize_model(message: dict[str, Any]) -> Model:
    return Model(
        id=message["model_id"],
        type=message["model_type"],
        bounds=deserialize_bounds(message["bounds"]),
        info=message.get("info", {}),
        visible=message["is_visible"],
        transform=deserialize_transform(message["transform"]),
    )


def _deserialize_scene(message: dict[str, Any]) -> Scene:
    return Scene(
        bounds=deserialize_bounds(message["bounds"]),
        models=[deserialize_model(model) for model in message["models"]],
    )
