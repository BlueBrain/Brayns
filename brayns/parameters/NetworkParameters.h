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
     * @brief Check if the client mode is enabled.
     *
     * Default: false.
     *
     * @return true Client mode.
     * @return false Server mode.
     */
    bool isClient() const noexcept;

    /**
     * @brief Check if SSL is enabled.
     *
     * Default: false.
     *
     * @return true SSL on, certificate might be required.
     * @return false SSL off.
     */
    bool isSecure() const noexcept;

    /**
     * @brief Get the maximum number of simultaneous connections allowed.
     *
     * Default: 2.
     *
     * @return size_t Max simultaneous client count.
     */
    size_t getMaxClients() const noexcept;

    /**
     * @brief Get the URI of the client or server in format host:port.
     *
     * @return const std::string& URI.
     */
    const std::string &getUri() const noexcept;

    /**
     * @brief Get the delay between two connections attempts in client mode.
     *
     * Default: 300ms.
     *
     * @return std::chrono::milliseconds Connnection period.
     */
    std::chrono::milliseconds getReconnectionPeriod() const noexcept;

    /**
     * @brief Get the path of the server private key (server + SSL).
     *
     * @return const std::string& Private key server file.
     */
    const std::string &getPrivateKeyFile() const noexcept;

    /**
     * @brief Get the private key passphrase if any.
     *
     * @return const std::string& Passphrase.
     */
    const std::string &getPrivateKeyPassphrase() const noexcept;

    /**
     * @brief Get the certificate of the server if any.
     *
     * @return const std::string& Certificate file.
     */
    const std::string &getCertificateFile() const noexcept;

    /**
     * @brief Get CA location, if empty, default SLL ones will be used.
     *
     * CA is the trusted authorities that can certify hosts.
     *
     * @return const std::string& CA file or directory.
     */
    const std::string &getCALocation() const noexcept;

    /**
     * @brief Register argv properties of the parameter set.
     *
     * @param builder Helper class to register argv properties.
     */
    virtual void build(ArgvBuilder &builder) override;

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
