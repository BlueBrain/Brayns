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

    /** Folder containing volume files */
    const std::string& getFolder() const { return _folder; }
    /** File containing volume data */
    const std::string& getFilename() const { return _filename; }
    void setFilename(const std::string& filename)
    {
        _updateValue(_filename, filename);
    }
    /** Volume dimension  */
    const Vector3ui& getDimensions() const { return _dimensions; }
    /** Volume scale  */
    const Vector3f& getElementSpacing() const { return _elementSpacing; }
    /** Volume offset */
    const Vector3f& getOffset() const { return _offset; }
    /** Volume epsilon */
    void setSamplesPerRay(const size_t spr) { _updateValue(_spr, spr); }
    size_t getSamplesPerRay() const { return _spr; }
protected:
    bool _parse(const po::variables_map& vm) final;

    std::string _folder;
    std::string _filename;
    Vector3ui _dimensions;
    Vector3f _elementSpacing;
    Vector3f _offset;
    size_t _spr;

    SERIALIZATION_FRIEND(VolumeParameters)
};
}
#endif // VOLUMEPARAMETERS_H
