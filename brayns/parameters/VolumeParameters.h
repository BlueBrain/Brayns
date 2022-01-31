/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
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

#pragma once

#include "AbstractParameters.h"

#include <brayns/common/MathTypes.h>

namespace brayns
{
class VolumeParameters final : public AbstractParameters
{
public:
    VolumeParameters();

    /**
     * @brief print prints these parametes to the Log
     */
    void print() final;

    const Vector3ui &getDimensions() const;
    void setDimensions(const Vector3ui &dim);
    const Vector3d &getElementSpacing() const;
    void setElementSpacing(const Vector3d &spacing);
    const Vector3d &getOffset() const;
    void setOffset(const Vector3d &offset);
    void setGradientShading(const bool enabled);
    bool getGradientShading() const;
    void setSingleShade(const bool enabled);
    bool getSingleShade() const;
    void setPreIntegration(const bool enabled);
    bool getPreIntegration() const;
    void setAdaptiveSampling(const bool enabled);
    bool getAdaptiveSampling() const;
    void setAdaptiveMaxSamplingRate(const double value);
    double getAdaptiveMaxSamplingRate() const;
    void setSamplingRate(const double value);
    double getSamplingRate() const;
    void setSpecular(const Vector3d &value);
    const Vector3d &getSpecular() const;
    void setClipBox(const Boxd &value);
    const Boxd &getClipBox() const;

protected:
    void parse(const po::variables_map &vm) final;

    Vector3ui _dimensions;
    Vector3d _elementSpacing;
    Vector3d _offset;

    bool _gradientShading{false};
    bool _singleShade{true};
    bool _preIntegration{false};
    double _adaptiveMaxSamplingRate{2.};
    bool _adaptiveSampling{true};
    double _samplingRate{0.125};
    Vector3d _specular{0.3, 0.3, 0.3};
    Boxd _clipBox;
};
} // namespace brayns
