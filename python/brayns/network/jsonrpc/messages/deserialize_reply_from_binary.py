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

from .deserialize_reply_from_text import deserialize_reply_from_text
from .json_rpc_reply import JsonRpcReply


def deserialize_reply_from_binary(data: bytes) -> JsonRpcReply:
    json_size = int.from_bytes(data[0:4], byteorder='little', signed=False)
    text = data[4:4+json_size].decode('utf-8')
    reply = deserialize_reply_from_text(text)
    reply.binary = data[4+json_size:]
    return reply
