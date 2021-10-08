/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
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

#pragma once

#include <chrono>
#include <string>

#include "AbstractParameters.h"

namespace brayns
{
/**
 * @brief Network parameters pool.
 *
 */
class NetworkParameters : public AbstractParameters
{
public:
    /**
     * @brief Register parameters.
     *
     */
    NetworkParameters();

    /**
     * @brief Print the parameters.
     *
     */
    virtual void print() override;

    /**
     * @brief Check if the client mode is enabled.
     *
     * Default: false.
     *
     * @return true Client mode.
     * @return false Server mode.
     */
    bool isClient() const { return _client; }

    /**
     * @brief Set client mode.
     *
     * @param client True if client mode.
     */
    void setClient(bool client) { _updateValue(_client, client); }

    /**
     * @brief Check if SSL is enabled.
     *
     * Default: false.
     *
     * @return true SSL on, certificate might be required.
     * @return false SSL off.
     */
    bool isSecure() const { return _secure; }

    /**
     * @brief Enable or disable SSL.
     *
     * @param secure True if SSL enabled.
     */
    void setSecure(bool secure) { _updateValue(_secure, secure); }

    /**
     * @brief Get the maximum number of simultaneous connections allowed.
     *
     * Default: 2.
     *
     * @return size_t Max simultaneous client count.
     */
    size_t getMaxClients() const { return _maxClients; }

    /**
     * @brief Set the maximum number of simultaneous connections.
     *
     * @param maxClients Max connection count.
     */
    void setMaxClients(size_t maxClients)
    {
        _updateValue(_maxClients, maxClients);
    }

    /**
     * @brief Get the URI of the client or server in format host:port.
     *
     * @return const std::string& URI.
     */
    const std::string& getUri() const { return _uri; }

    /**
     * @brief Set the URI of the client or server.
     *
     * @param uri URI.
     */
    void setUri(const std::string& uri) { _updateValue(_uri, uri); }

    /**
     * @brief Get the delay between two connections attempts in client mode.
     *
     * Default: 300ms.
     *
     * @return std::chrono::milliseconds Connnection period.
     */
    std::chrono::milliseconds getReconnectionPeriod() const
    {
        return std::chrono::milliseconds(_reconnectionPeriod);
    }

    /**
     * @brief Set the reconnection period.
     *
     * @param reconnectionPeriod Connection attempt period.
     */
    void setReconnectionPeriod(std::chrono::milliseconds reconnectionPeriod)
    {
        _updateValue(_reconnectionPeriod, reconnectionPeriod.count());
    }

    /**
     * @brief Get the path of the server private key (server + SSL).
     *
     * @return const std::string& Private key server file.
     */
    const std::string& getPrivateKeyFile() const { return _privateKeyFile; }

    /**
     * @brief Set the server private key.
     *
     * @param privateKeyFile File path.
     */
    void setPrivateKeyFile(const std::string& privateKeyFile)
    {
        _updateValue(_privateKeyFile, privateKeyFile);
    }

    /**
     * @brief Get the private key passphrase if any.
     *
     * @return const std::string& Passphrase.
     */
    const std::string& getPrivateKeyPassphrase() const
    {
        return _privateKeyPassphrase;
    }

    /**
     * @brief Set the private key passphrase.
     *
     * @param privateKeyPassphrase Passphrase.
     */
    void setPrivateKeyPassphrase(const std::string& privateKeyPassphrase)
    {
        _updateValue(_privateKeyPassphrase, privateKeyPassphrase);
    }

    /**
     * @brief Get the certificate of the server if any.
     *
     * @return const std::string& Certificate file.
     */
    const std::string& getCertificateFile() const { return _certificateFile; }

    /**
     * @brief Set the server certificate.
     *
     * @param certificateFile Certificate file.
     */
    void setCertificateFile(const std::string& certificateFile)
    {
        _updateValue(_certificateFile, certificateFile);
    }

    /**
     * @brief Get CA location, if empty, default SLL ones will be used.
     *
     * CA is the trusted authorities that can certify hosts.
     *
     * @return const std::string& CA file or directory.
     */
    const std::string& getCALocation() const { return _caLocation; }

    /**
     * @brief Set CA location.
     *
     * @param caLocation CA file or directory.
     */
    void setCALocation(const std::string& caLocation)
    {
        _updateValue(_caLocation, caLocation);
    }

private:
    bool _client = false;
    bool _secure = false;
    size_t _maxClients = 2;
    std::string _uri;
    int64_t _reconnectionPeriod = 300;
    std::string _privateKeyFile;
    std::string _privateKeyPassphrase;
    std::string _certificateFile;
    std::string _caLocation;
};
} // namespace brayns