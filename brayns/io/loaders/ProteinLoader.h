/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <brayns/io/Loader.h>
#include <brayns/io/loaders/ProteinLoaderParameters.h>

namespace brayns
{
/**
 * @brief The ProteinLoader class loads protein from PDB files: http://www.rcsb.org
 */
class ProteinLoader : public Loader<ProteinLoaderParameters>
{
public:
    std::vector<std::string> getSupportedExtensions() const final;

    std::string getName() const final;

    std::vector<Model::Ptr> importFromFile(
        const std::string &fileName, const LoaderProgress &cb, const ProteinLoaderParameters &properties) const final;

    std::vector<Model::Ptr> importFromBlob(
        Blob &&blob, const LoaderProgress &callback, const ProteinLoaderParameters &properties) const final;
};
} // namespace brayns
