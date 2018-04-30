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

#ifndef XYZBLOADER_H
#define XYZBLOADER_H

#include <brayns/common/loader/Loader.h>
#include <brayns/parameters/GeometryParameters.h>

namespace brayns
{
class XYZBLoader : public Loader
{
public:
    XYZBLoader(const GeometryParameters& geometryParameters);

    static std::set<std::string> getSupportedDataTypes();

    void importFromBlob(Blob&& blob, Scene& scene,
                        const Matrix4f& transformation,
                        const size_t materialID) final;

    void importFromFile(const std::string& filename, Scene& scene,
                        const Matrix4f& transformation,
                        const size_t materialID) final;

private:
    const GeometryParameters& _geometryParameters;
};
}

#endif // XYZBLOADER_H
