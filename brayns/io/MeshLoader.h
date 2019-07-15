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

#pragma once

#include <brayns/common/loader/Loader.h>
#include <brayns/parameters/GeometryParameters.h>

struct aiScene;

namespace brayns
{
/** Loads meshes from files using the assimp library
 * http://assimp.sourceforge.net
 */
class MeshLoader : public Loader
{
public:
    MeshLoader(Scene& scene);
    MeshLoader(Scene& scene, const GeometryParameters& geom);

    std::vector<std::string> getSupportedExtensions() const final;
    std::string getName() const final;
    PropertyMap getProperties() const final;

    bool isSupported(const std::string& filename,
                     const std::string& extension) const final;

    ModelDescriptorPtr importFromFile(
        const std::string& fileName, const LoaderProgress& callback,
        const PropertyMap& properties) const final;

    ModelDescriptorPtr importFromBlob(
        Blob&& blob, const LoaderProgress& callback,
        const PropertyMap& properties) const final;

    ModelMetadata importMesh(const std::string& fileName,
                             const LoaderProgress& callback, Model& model,
                             const Matrix4f& transformation,
                             const size_t defaultMaterialId,
                             const GeometryQuality geometryQuality) const;

private:
    PropertyMap _defaults;

    void _createMaterials(Model& model, const aiScene* aiScene,
                          const std::string& folder) const;

    ModelMetadata _postLoad(const aiScene* aiScene, Model& model,
                            const Matrix4f& transformation,
                            const size_t defaultMaterial,
                            const std::string& folder,
                            const LoaderProgress& callback) const;
    size_t _getQuality(const GeometryQuality geometryQuality) const;
};
} // namespace brayns
