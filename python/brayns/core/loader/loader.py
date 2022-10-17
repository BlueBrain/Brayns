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

from __future__ import annotations

from abc import ABC, abstractmethod
from typing import Any

from brayns.network import Future, Instance, JsonRpcReply

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
        task = instance.task('add-model', params)
        return Future(task, _get_models)

    def upload_models(self, instance: Instance, format: str, data: bytes) -> list[Model]:
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

    def upload_models_task(self, instance: Instance, format: str, data: bytes) -> Future[list[Model]]:
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
        task = instance.task('upload-model', params, data)
        return Future(task, _get_models)


def _get_params(loader: Loader, path: str | None = None, format: str | None = None) -> dict[str, Any]:
    params = {
        'loader_name': loader.name,
        'loader_properties': loader.get_properties(),
    }
    if path is not None:
        params['path'] = path
    if format is not None:
        params['type'] = format
    return params


def _get_models(reply: JsonRpcReply) -> list[Model]:
    return [
        deserialize_model(model)
        for model in reply.result
    ]
