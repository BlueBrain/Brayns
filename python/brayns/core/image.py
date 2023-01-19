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

from dataclasses import dataclass
from typing import Any

from brayns.network import Instance, JsonRpcReply
from brayns.utils import ImageFormat, parse_image_format


@dataclass
class ImageInfo:
    """Result of an image rendering with status and encoded data.

    If nothing has been downloaded, data is empty.

    :param accumulation: Current accumulation after render.
    :type accumulation: int
    :param max_accumulation: Accumulation limit to stop rendering.
    :type max_accumulation: int
    :param data: Encoded image data, can be empty.
    :type data: bytes
    """

    accumulation: int
    max_accumulation: int
    data: bytes

    @property
    def full_quality(self) -> bool:
        """Check if max accumulation has been reached.

        :return: True if image is full quality.
        :rtype: bool
        """
        return self.accumulation == self.max_accumulation


@dataclass
class Image:
    """Helper class to take an image of an instance with current settings.

    Note that no render will occur if nothing has changed since last call
    (same context and max accumulation reached).

    If accumulate is True, the instance will render images and accumulate them
    until current renderer ``samples_per_pixel`` is reached. Otherwise only one
    image will be rendered.

    If force_download is True, the framebuffer image will always be downloaded.
    Otherwise it will be downloaded only when something new has been rendered.

    Check ImageInfo.data to see if something has been downloaded.

    :param accumulate: Render all samples at once, defaults to True.
    :type accumulate: bool, optional
    :param force_download: Force download, defaults to True.
    :type force_download: bool, optional
    :param jpeg_quality: JPEG quality.
    :type jpeg_quality: int
    """

    accumulate: bool = True
    force_download: bool = True
    jpeg_quality: int = 100

    def save(self, instance: Instance, path: str) -> ImageInfo:
        """Try render image and save it under given path (if downloaded).

        :param instance: Instance.
        :type instance: Instance
        :param path: Path to save image.
        :type path: str
        :return: Render status and image data.
        :rtype: ImageInfo
        """
        format = parse_image_format(path)
        image = self.download(instance, format)
        if not image.data:
            return image
        with open(path, "wb") as file:
            file.write(image.data)
        return image

    def download(
        self, instance: Instance, format: ImageFormat = ImageFormat.PNG
    ) -> ImageInfo:
        """Try render image and download it at given format.

        :param instance: Instance.
        :type instance: Instance
        :param format: Image encoding format, defaults to ImageFormat.PNG
        :type format: ImageFormat, optional
        :return: Render status and image data.
        :rtype: ImageInfo
        """
        params = _serialize_image(self, format=format)
        return _request(instance, params)

    def render(self, instance: Instance) -> ImageInfo:
        """Try render image without downloading it.

        :param instance: Instance.
        :type instance: Instance
        :return: Render status.
        :rtype: ImageInfo
        """
        params = _serialize_image(self, send=False)
        return _request(instance, params)


def _request(instance: Instance, params: dict[str, Any]) -> ImageInfo:
    reply = instance.execute("render-image", params)
    return _deserialize_image(reply)


def _serialize_image(
    image: Image, send: bool = True, format: ImageFormat = ImageFormat.PNG
) -> dict[str, Any]:
    params: dict[str, Any] = {
        "send": send,
        "force": send and image.force_download,
        "accumulate": image.accumulate,
    }
    if send:
        params["format"] = format.value
    if send and format is ImageFormat.JPEG:
        params["jpeg_quality"] = image.jpeg_quality
    return params


def _deserialize_image(reply: JsonRpcReply) -> ImageInfo:
    return ImageInfo(
        accumulation=reply.result["accumulation"],
        max_accumulation=reply.result["max_accumulation"],
        data=reply.binary,
    )
