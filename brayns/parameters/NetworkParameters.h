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

#include <chrono>
#include <string>

#include "AbstractParameters.h"

namespace brayns
{
class NetworkParameters : public AbstractParameters
{
public:
    NetworkParameters();

    virtual void print() override;

    bool isClient() const { return _client; }

    void setClient(bool client) { _updateValue(_client, client); }

    bool isSecure() const { return _secure; }

    void setSecure(bool secure) { _updateValue(_secure, secure); }

    size_t getMaxClients() const { return _maxClients; }

    void setMaxClients(size_t maxClients)
    {
        _updateValue(_maxClients, maxClients);
    }

    const std::string& getUri() const { return _uri; }

    void setUri(const std::string& uri) { _updateValue(_uri, uri); }

    std::chrono::milliseconds getReconnectionPeriod() const
    {
        return std::chrono::milliseconds(_reconnectionPeriod);
    }

    void setReconnectionPeriod(std::chrono::milliseconds reconnectionPeriod)
    {
        _updateValue(_reconnectionPeriod, reconnectionPeriod.count());
    }

    const std::string& getPrivateKeyFile() const { return _privateKeyFile; }

    void setPrivateKeyFile(const std::string& privateKeyFile)
    {
        _updateValue(_privateKeyFile, privateKeyFile);
    }

    const std::string& getPrivateKeyPassphrase() const
    {
        return _privateKeyPassphrase;
    }

    void setPrivateKeyPassphrase(const std::string& privateKeyPassphrase)
    {
        _updateValue(_privateKeyPassphrase, privateKeyPassphrase);
    }

    const std::string& getCertificateFile() const { return _certificateFile; }

    void setCertificateFile(const std::string& certificateFile)
    {
        _updateValue(_certificateFile, certificateFile);
    }

    const std::string& getCALocation() const { return _caLocation; }

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