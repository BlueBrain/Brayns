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

#include "NetworkParameters.h"

namespace brayns
{
bool NetworkParameters::isSecure() const noexcept
{
    return _secure;
}

size_t NetworkParameters::getMaxClients() const noexcept
{
    return _maxClients;
}

const std::string &NetworkParameters::getUri() const noexcept
{
    return _uri;
}

std::chrono::milliseconds NetworkParameters::getReconnectionPeriod() const noexcept
{
    return std::chrono::milliseconds(_reconnectionPeriod);
}

const std::string &NetworkParameters::getPrivateKeyFile() const noexcept
{
    return _privateKeyFile;
}

const std::string &NetworkParameters::getPrivateKeyPassphrase() const noexcept
{
    return _privateKeyPassphrase;
}

const std::string &NetworkParameters::getCertificateFile() const noexcept
{
    return _certificateFile;
}

const std::string &NetworkParameters::getCALocation() const noexcept
{
    return _caLocation;
}

void NetworkParameters::build(ArgvBuilder &builder)
{
    builder.add("secure", _secure, "Enable SSL");
    builder.add("max-clients", _maxClients, "Max simultaneous connections");
    builder.add("uri", _uri, "Server URI (host:port)");
    builder.add("reconnection-period", _reconnectionPeriod, "Client mode reconnection period in ms").minimum(0.0);
    builder.add("private-key-file", _privateKeyFile, "Private key file path if secure");
    builder.add("private-key-passphrase", _privateKeyPassphrase, "Private key password if any");
    builder.add("certificate-file", _certificateFile, "Server or client certificate");
    builder.add("ca-location", _caLocation, "File or folder of additional CA");
}
} // namespace brayns
