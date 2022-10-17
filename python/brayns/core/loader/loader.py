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

from abc import ABC, abstractmethod
from typing import Any

from brayns.network import Instance

from ..model import Model, deserialize_model


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
        params = {
            'path': path,
            'loader_name': self.name,
            'loader_properties': self.get_properties(),
        }
        result = instance.request('add-model', params)
        return [
            deserialize_model(model)
            for model in result
        ]

    def load_models_from_binary(self, instance: Instance, format: str, data: bytes) -> list[Model]:
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
        params = {
            'type': format,
            'loader_name': self.name,
            'loader_properties': self.get_properties(),
        }
        reply = instance.execute('upload-model', params, data)
        return [
            deserialize_model(model)
            for model in reply.result
        ]
