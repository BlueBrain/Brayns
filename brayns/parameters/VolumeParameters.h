/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
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

#ifndef VOLUMEPARAMETERS_H
#define VOLUMEPARAMETERS_H

#include "AbstractParameters.h"

SERIALIZATION_ACCESS(VolumeParameters)

namespace brayns
{
class VolumeParameters final : public AbstractParameters
{
public:
    VolumeParameters();

    /** @copydoc AbstractParameters::print */
    void print() final;

    /** Volume dimensions  */
    const Vector3ui& getDimensions() const { return _dimensions; }
    void setDimensions(const Vector3ui& dim) { _updateValue(_dimensions, dim); }
    /** Volume scale  */
    const Vector3f& getElementSpacing() const { return _elementSpacing; }
    void setElementSpacing(const Vector3f& spacing)
    {
        _updateValue(_elementSpacing, spacing);
    }
    /** Volume offset */
    const Vector3f& getOffset() const { return _offset; }
    void setGradientShading(const bool enabled)
    {
        _updateValue(_gradientShading, enabled);
    }
    bool getGradientShading() const { return _gradientShading; }
    void setSingleShade(const bool enabled)
    {
        _updateValue(_singleShade, enabled);
    }
    bool getSingleShade() const { return _singleShade; }
    void setPreIntegration(const bool enabled)
    {
        _updateValue(_preIntegration, enabled);
    }
    bool getPreIntegration() const { return _preIntegration; }
    void setAdaptiveSampling(const bool enabled)
    {
        _updateValue(_adaptiveSampling, enabled);
    }
    bool getAdaptiveSampling() const { return _adaptiveSampling; }
    void setAdaptiveMaxSamplingRate(const float value)
    {
        _updateValue(_adaptiveMaxSamplingRate, value);
    }
    float getAdaptiveMaxSamplingRate() const
    {
        return _adaptiveMaxSamplingRate;
    }

    void setSamplingRate(const float value)
    {
        _updateValue(_samplingRate, value);
    }
    float getSamplingRate() const { return _samplingRate; }
    void setSpecular(const Vector3f& value) { _updateValue(_specular, value); }
    const Vector3f& getSpecular() const { return _specular; }
    void setClipBox(const Boxf& value) { _updateValue(_clipBox, value); }
    const Boxf& getClipBox() const { return _clipBox; }
protected:
    void parse(const po::variables_map& vm) final;

    Vector3ui _dimensions;
    Vector3f _elementSpacing;
    Vector3f _offset;

    bool _gradientShading{false};
    bool _singleShade{true};
    bool _preIntegration{false};
    float _adaptiveMaxSamplingRate{2.f};
    bool _adaptiveSampling{true};
    float _samplingRate{0.125f};
    Vector3f _specular{0.3f, 0.3f, 0.3f};
    Boxf _clipBox;

    SERIALIZATION_FRIEND(VolumeParameters)
};
}
#endif // VOLUMEPARAMETERS_H
