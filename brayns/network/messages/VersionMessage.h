/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#pragma once

#include <sstream>

#include <brayns/network/message/Message.h>

#include <brayns/version.h>

namespace brayns
{
BRAYNS_MESSAGE_BEGIN(VersionMessage)
BRAYNS_MESSAGE_ENTRY(int, major, "Major version")
BRAYNS_MESSAGE_ENTRY(int, minor, "Minor version")
BRAYNS_MESSAGE_ENTRY(int, patch, "Patch level")
BRAYNS_MESSAGE_ENTRY(int, abi, "Binary interface version")
BRAYNS_MESSAGE_ENTRY(std::string, revision, "SCM revision")

static VersionMessage create()
{
    VersionMessage message;
    message.major = Version::getMajor();
    message.minor = Version::getMinor();
    message.patch = Version::getPatch();
    message.abi = Version::getABI();
    std::ostringstream stream;
    stream << std::hex << Version::getRevision();
    message.revision = stream.str();
    return message;
}

BRAYNS_MESSAGE_END()
} // namespace brayns