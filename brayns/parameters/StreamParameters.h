/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#ifndef STREAMPARAMETERS_H
#define STREAMPARAMETERS_H

#include "AbstractParameters.h"

SERIALIZATION_ACCESS(StreamParameters)

namespace brayns
{
class StreamParameters : public AbstractParameters
{
public:
    StreamParameters();

    /** @copydoc AbstractParameters::print */
    void print() final;

    /** Streaming enabled */
    bool getEnabled() const { return _enabled; }
    void setEnabled(const bool enabled) { _updateValue(_enabled, enabled); }
    /** Stream compression enabled */
    bool getCompression() const { return _compression; }
    void setCompression(const bool enabled)
    {
        _updateValue(_compression, enabled);
    }

    /** Stream compression quality, 1 (worst) to 100 (best) */
    unsigned getQuality() const { return _quality; }
    void setQuality(const unsigned quality) { _updateValue(_quality, quality); }
    /** Stream ID; defaults to DEFLECT_ID if empty */
    const std::string& getId() const { return _id; }
    void setId(const std::string& id) { _updateValue(_id, id); }
    /** Stream hostname; defaults to DEFLECT_HOST if empty */
    const std::string& getHostname() const { return _host; }
    void setHost(const std::string& host) { _updateValue(_host, host); }
    /** Stream port; defaults to 1701 if empty */
    unsigned getPort() const { return _port; }
    void setPort(const unsigned port) { _updateValue(_port, port); }
private:
    bool _parse(const po::variables_map& vm) final;

    std::string _host;
    bool _enabled{true};
    std::string _id;
    unsigned _port{1701};
    bool _compression{true};
    unsigned _quality{80};

    SERIALIZATION_FRIEND(StreamParameters)
};
}
#endif
