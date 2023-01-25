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

from abc import ABC, abstractmethod
from dataclasses import dataclass
from enum import Enum
from typing import Any, ClassVar

from brayns.network import Future, Instance, JsonRpcReply
from brayns.utils import JsonSchema, Vector3, deserialize_schema

from .model import Model, deserialize_model


@dataclass
class LoaderInfo:
    """Loader description.

    :param name: Loader name.
    :type name: str
    :param extensions: Supported extensions without the dot.
    :type extensions: list[str]
    :param schema: Parameters JSON schema (low level).
    :type schema: JsonSchema
    """

    name: str
    extensions: list[str]
    schema: JsonSchema


def get_loaders(instance: Instance) -> list[LoaderInfo]:
    """Retreive all available loaders from an instance.

    :param instance: Instance.
    :type instance: Instance
    :return: List of loader descriptions.
    :rtype: list[LoaderInfo]
    """
    result = instance.request("get-loaders")
    return [_deserialize_loader(loader) for loader in result]


class Loader(ABC):
    """Base class for all loaders.

    Loader are used to load models from files into an instance.

    Available loaders for a given instance depends on the loaded plugins.

    The list of available loaders and supported extensions for a given instance
    can be queried using ``get_loaders(instance)``.
    """

    @classmethod
    @property
    @abstractmethod
    def name(cls) -> str:
        """Get the loader name.

        Can be compared with ``LoaderInfo.name``.

        :return: Loader name.
        :rtype: str
        """
        pass

    @abstractmethod
    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        pass

    def load_models(self, instance: Instance, path: str) -> list[Model]:
        """Load the given file into an instance and return the models.

        :param instance: Instance.
        :type instance: Instance
        :param path: Model(s) file path.
        :type path: str
        :return: List of created models.
        :rtype: list[Model]
        """
        task = self.load_models_task(instance, path)
        return task.wait_for_result()

    def load_models_task(self, instance: Instance, path: str) -> Future[list[Model]]:
        """Asynchronous version of ``load_models``.

        :param instance: Instance.
        :type instance: Instance
        :param path: Model(s) file path.
        :type path: str
        :return: Future to monitor the task.
        :rtype: Future[list[Model]]
        """
        params = _get_params(self, path=path)
        task = instance.task("add-model", params)
        return Future(task, _get_models)

    def upload_models(
        self, instance: Instance, format: str, data: bytes
    ) -> list[Model]:
        """Load models from binary data.

        As the model format cannot be deduced from a path, it must be specified.

        The format is the file extension without the dot, check the class
        variables to see supported formats (ex: MeshLoader.PLY).

        :param instance: Instance.
        :type instance: Instance
        :param format: Model format (see loader class variables).
        :type format: str
        :param data: Model binary data.
        :type data: bytes
        :return: List of created models.
        :rtype: list[Model]
        """
        task = self.upload_models_task(instance, format, data)
        return task.wait_for_result()

    def upload_models_task(
        self, instance: Instance, format: str, data: bytes
    ) -> Future[list[Model]]:
        """Asynchronous version of ``upload_models``.

        :param instance: Instance.
        :type instance: Instance
        :param format: Model format (see loader class variables).
        :type format: str
        :param data: Model binary data.
        :type data: bytes
        :return: Future to monitor the task.
        :rtype: Future[list[Model]]
        """
        params = _get_params(self, format=format)
        task = instance.task("upload-model", params, data)
        return Future(task, _get_models)


class MeshLoader(Loader):
    """Mesh loader.

    Main supported formats are OBJ, PLY, STL and OFF.

    Format support can be queried using ``get_loaders(instance)``.
    """

    OBJ: ClassVar[str] = "obj"
    PLY: ClassVar[str] = "ply"
    STL: ClassVar[str] = "stl"
    OFF: ClassVar[str] = "off"

    @classmethod
    @property
    def name(cls) -> str:
        """Get the loader name.

        :return: Loader name.
        :rtype: str
        """
        return "mesh"

    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {}


class MhdVolumeLoader(Loader):
    """MDH volume loader.

    Supports .mhd file format.
    """

    MHD: ClassVar[str] = "mhd"

    @classmethod
    @property
    def name(cls) -> str:
        """Get the loader name.

        :return: Loader name.
        :rtype: str
        """
        return "mhd-volume"

    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {}


class VolumeDataType(Enum):
    """Supported volume voxel data types."""

    UNSIGNED_CHAR = "unsigned_char"
    SHORT = "short"
    UNSIGNED_SHORT = "unsigned_short"
    HALF_FLOAT = "half_float"
    FLOAT = "float"
    DOUBLE = "double"


@dataclass
class RawVolumeLoader(Loader):
    """Raw volume loader.

    Supports .raw file format.

    :param dimensions: Volume 3D grid dimensions.
    :type dimensions: Vector3.
    :param spacing: World-space dimensions of a voxel.
    :type size: Vector3.
    :param data_type: Type of data to interpret the volume bytes.
    :type data_type: str.
    """

    RAW: ClassVar[str] = "raw"

    dimensions: Vector3
    spacing: Vector3
    data_type: VolumeDataType

    @classmethod
    @property
    def name(cls) -> str:
        """Get the loader name.

        :return: Loader name.
        :rtype: str
        """
        return "raw-volume"

    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            "dimensions": list(self.dimensions),
            "spacing": list(self.spacing),
            "data_type": self.data_type.value,
        }


def _deserialize_loader(message: dict[str, Any]) -> LoaderInfo:
    return LoaderInfo(
        name=message["name"],
        extensions=message["extensions"],
        schema=deserialize_schema(message["input_parameters_schema"]),
    )


def _get_params(
    loader: Loader, path: str | None = None, format: str | None = None
) -> dict[str, Any]:
    params = {
        "loader_name": loader.name,
        "loader_properties": loader.get_properties(),
    }
    if path is not None:
        params["path"] = path
    if format is not None:
        params["type"] = format
    return params


def _get_models(reply: JsonRpcReply) -> list[Model]:
    return [deserialize_model(model) for model in reply.result]
