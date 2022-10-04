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

from .jpeg_image import JpegImage


def render_image(instance: Instance, send: bool = True, force: bool = True) -> JpegImage:
    """Render an image using current instance parameters.

    If the current framebuffer content is already up-to-date with its max
    accumulation reached, nothing new is rendered and the current image is sent
    only if ``force`` is True.

    If ``send`` is False, then the image is not sent by the renderer but the
    render is still performed if the current framebuffer is not up-to-date.

    :param instance: Instance to use for render.
    :type instance: Instance
    :param send: Send image in JPEG once render, defaults to True.
    :type send: bool, optional
    :param force: Force send JPEG, defaults to True.
    :type force: bool, optional
    :return: JPEG image if received.
    :rtype: JpegImage
    """
    params = {
        'send': send,
        'force': force,
    }
    result, binary = instance.execute('render-image', params)
    return JpegImage(
        accumulation=result['accumulation'],
        max_accumulation=result['max_accumulation'],
        data=binary,
    )
