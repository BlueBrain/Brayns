/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "VersionEntrypoint.h"

#include <brayns/core/Version.h>

namespace brayns
{
std::string VersionEntrypoint::getMethod() const
{
    return "get-version";
}

std::string VersionEntrypoint::getDescription() const
{
    return "Get Brayns version";
}

void VersionEntrypoint::onRequest(const Request &request)
{
    VersionMessage message;
    message.major = BRAYNS_VERSION_MAJOR;
    message.minor = BRAYNS_VERSION_MINOR;
    message.patch = BRAYNS_VERSION_PATCH;
    message.pre_release = BRAYNS_VERSION_PRE_RELEASE;
    message.tag = getVersionTag();
    message.copyright = getCopyright();
    request.reply(message);
}
} // namespace brayns
