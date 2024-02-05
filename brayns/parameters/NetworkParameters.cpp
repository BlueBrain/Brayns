/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/common/Log.h>

namespace brayns
{
NetworkParameters::NetworkParameters()
    : AbstractParameters("Network")
{
    _parameters.add_options()(
        "client", po::value(&_client),
        "Enable client mode (default = false), if enabled no server will be "
        "run and a connection will be done at the given URI")(
        "secure", po::value(&_secure),
        "Enable HTTPS (default = false), if enabled private key and/or "
        "certificates must be provided")(
        "max-clients", po::value(&_maxClients),
        "Defines the max amount of parallel clients (default = 2) that can "
        "connect in server mode")("uri", po::value(&_uri),
                                  "URI of the internal HTTP server or of the "
                                  "one to connect to in client mode")(
        "reconnection-period", po::value(&_reconnectionPeriod),
        "Period to wait [ms] (default = 300) in client mode before doing a new "
        "reconnection attempt")("private-key-file", po::value(&_privateKeyFile),
                                "Private key file if a private key is used")(
        "private-key-passphrase", po::value(&_privateKeyPassphrase),
        "Private key file passphrase if any")(
        "certificate-file", po::value(&_certificateFile),
        "Certificate used for encryption in secure mode")(
        "ca-location", po::value(&_caLocation),
        "Certification Authority file or directory, if not set, default "
        "OpenSSL ones will be used");
}

bool NetworkParameters::isClient() const noexcept
{
    return _client;
}

void NetworkParameters::setClient(bool client) noexcept
{
    _updateValue(_client, client);
}

bool NetworkParameters::isSecure() const noexcept
{
    return _secure;
}

void NetworkParameters::setSecure(bool secure) noexcept
{
    _updateValue(_secure, secure);
}

size_t NetworkParameters::getMaxClients() const noexcept
{
    return _maxClients;
}

void NetworkParameters::setMaxClients(size_t maxClients) noexcept
{
    _updateValue(_maxClients, maxClients);
}

const std::string& NetworkParameters::getUri() const noexcept
{
    return _uri;
}

void NetworkParameters::setUri(const std::string& uri) noexcept
{
    _updateValue(_uri, uri);
}

std::chrono::milliseconds NetworkParameters::getReconnectionPeriod() const
    noexcept
{
    return std::chrono::milliseconds(_reconnectionPeriod);
}

void NetworkParameters::setReconnectionPeriod(
    std::chrono::milliseconds reconnectionPeriod) noexcept
{
    _updateValue(_reconnectionPeriod, reconnectionPeriod.count());
}

const std::string& NetworkParameters::getPrivateKeyFile() const noexcept
{
    return _privateKeyFile;
}

void NetworkParameters::setPrivateKeyFile(
    const std::string& privateKeyFile) noexcept
{
    _updateValue(_privateKeyFile, privateKeyFile);
}

const std::string& NetworkParameters::getPrivateKeyPassphrase() const noexcept
{
    return _privateKeyPassphrase;
}

void NetworkParameters::setPrivateKeyPassphrase(
    const std::string& privateKeyPassphrase) noexcept
{
    _updateValue(_privateKeyPassphrase, privateKeyPassphrase);
}

const std::string& NetworkParameters::getCertificateFile() const noexcept
{
    return _certificateFile;
}

void NetworkParameters::setCertificateFile(
    const std::string& certificateFile) noexcept
{
    _updateValue(_certificateFile, certificateFile);
}

const std::string& NetworkParameters::getCALocation() const noexcept
{
    return _caLocation;
}

void NetworkParameters::setCALocation(const std::string& caLocation) noexcept
{
    _updateValue(_caLocation, caLocation);
}

void NetworkParameters::print()
{
    AbstractParameters::print();
    Log::info("Client mode               : {}", _client);
    Log::info("\nSecure                    : {}", _secure);
    Log::info("\nMax simultaneous clients  : {}", _maxClients);
    Log::info("\nURI                       : {}", _uri);
    Log::info("\nPrivate key file          : {}", _privateKeyFile);
    Log::info("\nPrivate key passphrase    : {}", _privateKeyPassphrase);
    Log::info("\nCertificate file          : {}", _certificateFile);
    Log::info("\nCA location               : {}", _caLocation);
}
} // namespace brayns
