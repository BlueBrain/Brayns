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

from brayns.network import Instance
from brayns.utils import ImageFormat

from .image import Image


def render_image(
    instance: Instance,
    send: bool = True,
    force: bool = False,
    format: ImageFormat = ImageFormat.JPEG,
    jpeg_quality: int = 100,
) -> Image:
    """Render an image using current instance parameters.

    If the current framebuffer content is already up-to-date with its max
    accumulation reached, nothing new is rendered and the current image is sent
    only if ``force`` is True.

    If ``send`` is False, then the image is not sent by the renderer but the
    render is still performed if the current framebuffer is not up-to-date.

    :param instance: Instance to use for render.
    :type instance: Instance
    :param send: Return image once rendered, defaults to True.
    :type send: bool, optional
    :param force: Force returning image, defaults to False.
    :type force: bool, optional
    :param format: Returned image format, defaults to JPEG.
    :type format: ImageFormat, optional
    :param format: JPEG quality if format is JPEG, defaults to 100%.
    :type jpeg_quality: int, optional
    :return: Image received from the instance.
    :rtype: Image
    """
    params = {
        'send': send,
        'force': force,
        'format': format.value,
    }
    if format is ImageFormat.JPEG:
        params['jpeg_quality'] = jpeg_quality
    reply = instance.execute('render-image', params)
    result, binary = reply.result, reply.binary
    return Image(
        accumulation=result['accumulation'],
        max_accumulation=result['max_accumulation'],
        data=binary,
    )
