/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include <brayns/common/BaseObject.h>
#include <brayns/common/propertymap/PropertyMap.h>

#include <deflect/types.h>

namespace brayns
{
constexpr auto PARAM_CHROMA_SUBSAMPLING = "chromaSubsampling";
constexpr auto PARAM_COMPRESSION = "compression";
constexpr auto PARAM_ENABLED = "enabled";
constexpr auto PARAM_HOSTNAME = "hostname";
constexpr auto PARAM_ID = "id";
constexpr auto PARAM_PORT = "port";
constexpr auto PARAM_QUALITY = "quality";
constexpr auto PARAM_RESIZING = "resizing";
constexpr auto PARAM_TOP_DOWN = "topDown";
constexpr auto PARAM_USE_PIXEL_OP = "usePixelop";

class DeflectParameters : public BaseObject
{
public:
    DeflectParameters();

    bool getEnabled() const { return _props[PARAM_ENABLED].as<bool>(); }
    void setEnabled(const bool enabled)
    {
        _updateProperty(PARAM_ENABLED, enabled);
    }
    bool getCompression() const { return _props[PARAM_COMPRESSION].as<bool>(); }
    void setCompression(const bool enabled)
    {
        _updateProperty(PARAM_COMPRESSION, enabled);
    }

    unsigned getQuality() const
    {
        return (unsigned)_props[PARAM_QUALITY].as<int32_t>();
    }
    void setQuality(const unsigned quality)
    {
        _updateProperty(PARAM_QUALITY, (int32_t)quality);
    }
    std::string getId() const { return _props[PARAM_ID].as<std::string>(); }
    void setId(const std::string& id) { _updateProperty(PARAM_ID, id); }
    std::string getHostname() const
    {
        return _props[PARAM_HOSTNAME].as<std::string>();
    }
    void setHost(const std::string& host)
    {
        _updateProperty(PARAM_HOSTNAME, host);
    }

    unsigned getPort() const
    {
        return (unsigned)_props[PARAM_PORT].as<int32_t>();
    }
    void setPort(const unsigned port)
    {
        _updateProperty(PARAM_PORT, (int32_t)port);
    }

    bool isResizingEnabled() const { return _props[PARAM_RESIZING].as<bool>(); }

    bool isTopDown() const { return _props[PARAM_TOP_DOWN].as<bool>(); }
    void setIsTopDown(const bool topDown)
    {
        _updateProperty(PARAM_TOP_DOWN, topDown);
    }

    bool usePixelOp() const { return _props[PARAM_USE_PIXEL_OP].as<bool>(); }
    deflect::ChromaSubsampling getChromaSubsampling() const
    {
        return (deflect::ChromaSubsampling)_props[PARAM_CHROMA_SUBSAMPLING]
            .as<int32_t>();
    }
    void setChromaSubsampling(const deflect::ChromaSubsampling subsampling)
    {
        _updateProperty(PARAM_CHROMA_SUBSAMPLING, (int32_t)subsampling);
    }

    const PropertyMap& getPropertyMap() const { return _props; }
    PropertyMap& getPropertyMap() { return _props; }

private:
    PropertyMap _props;

    template <typename T>
    void _updateProperty(const char* property, const T& newValue)
    {
        if (!_isEqual(_props[property].as<T>(), newValue))
        {
            _props.update(property, newValue);
            markModified();
        }
    }
};
} // namespace brayns
