/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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
bool NetworkParameters::isClient() const noexcept
{
    return _client;
}

void NetworkParameters::setClient(bool client) noexcept
{
    _client = client;
}

bool NetworkParameters::isSecure() const noexcept
{
    return _secure;
}

void NetworkParameters::setSecure(bool secure) noexcept
{
    _secure = secure;
}

size_t NetworkParameters::getMaxClients() const noexcept
{
    return _maxClients;
}

void NetworkParameters::setMaxClients(size_t maxClients) noexcept
{
    _maxClients = maxClients;
}

const std::string &NetworkParameters::getUri() const noexcept
{
    return _uri;
}

void NetworkParameters::setUri(std::string uri) noexcept
{
    _uri = std::move(uri);
}

std::chrono::milliseconds NetworkParameters::getReconnectionPeriod() const noexcept
{
    return std::chrono::milliseconds(_reconnectionPeriod);
}

void NetworkParameters::setReconnectionPeriod(std::chrono::milliseconds reconnectionPeriod) noexcept
{
    _reconnectionPeriod = reconnectionPeriod.count();
}

const std::string &NetworkParameters::getPrivateKeyFile() const noexcept
{
    return _privateKeyFile;
}

void NetworkParameters::setPrivateKeyFile(std::string privateKeyFile) noexcept
{
    _privateKeyFile = std::move(privateKeyFile);
}

const std::string &NetworkParameters::getPrivateKeyPassphrase() const noexcept
{
    return _privateKeyPassphrase;
}

void NetworkParameters::setPrivateKeyPassphrase(std::string privateKeyPassphrase) noexcept
{
    _privateKeyPassphrase = std::move(privateKeyPassphrase);
}

const std::string &NetworkParameters::getCertificateFile() const noexcept
{
    return _certificateFile;
}

void NetworkParameters::setCertificateFile(std::string certificateFile) noexcept
{
    _certificateFile = std::move(certificateFile);
}

const std::string &NetworkParameters::getCALocation() const noexcept
{
    return _caLocation;
}

void NetworkParameters::setCALocation(std::string caLocation) noexcept
{
    _caLocation = std::move(caLocation);
}

void NetworkParameters::build(ArgvBuilder &builder)
{
    builder.add("client", _client, "Enable client mode");
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
