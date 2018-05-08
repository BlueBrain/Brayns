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

#ifndef PROTEINLOADER_H
#define PROTEINLOADER_H

#include <brayns/common/loader/Loader.h>
#include <brayns/common/types.h>
#include <brayns/parameters/GeometryParameters.h>
#include <string>

namespace brayns
{
/** Loads protein from PDB files
 * http://www.rcsb.org
 */
class ProteinLoader : public Loader
{
public:
    ProteinLoader(const GeometryParameters& geometryParameters);

    static std::set<std::string> getSupportedDataTypes();

    void importFromFile(const std::string& fileName, Scene& scene,
                        const size_t index = 0,
                        const Matrix4f& transformation = Matrix4f(),
                        const size_t defaultMaterialId = NO_MATERIAL) final;

    void importFromBlob(Blob&&, Scene&, const size_t = 0,
                        const Matrix4f& = Matrix4f(),
                        const size_t = NO_MATERIAL) final
    {
        throw std::runtime_error("Unsupported");
    }

private:
    const GeometryParameters& _geometryParameters;
};
}

#endif // PROTEINLOADER_H
