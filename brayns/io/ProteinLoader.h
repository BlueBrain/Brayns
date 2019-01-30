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
#include <brayns/parameters/GeometryParameters.h>

namespace brayns
{
/** Loads protein from PDB files
 * http://www.rcsb.org
 */
class ProteinLoader : public Loader
{
public:
    ProteinLoader(Scene& scene, const PropertyMap& properties);
    ProteinLoader(Scene& scene, const GeometryParameters& params);

    std::vector<std::string> getSupportedExtensions() const final;
    std::string getName() const final;
    PropertyMap getProperties() const final;

    bool isSupported(const std::string& filename,
                     const std::string& extension) const final;
    ModelDescriptorPtr importFromFile(
        const std::string& fileName, const LoaderProgress& callback,
        const PropertyMap& properties) const final;

    ModelDescriptorPtr importFromBlob(Blob&&, const LoaderProgress&,
                                      const PropertyMap&) const final
    {
        throw std::runtime_error("Loading from blob not supported");
    }

private:
    PropertyMap _defaults;
};
}

#endif // PROTEINLOADER_H
