/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include <brayns/network/common/ExtractModel.h>
#include <brayns/network/entrypoint/Entrypoint.h>

#include <plugin/network/messages/MirrorModelMessage.h>

class ModelMirrorer
{
public:
    ModelMirrorer(brayns::PluginAPI& api)
        : _api(&api)
    {
    }

    void mirrorModel(const MirrorModelMessage& params)
    {
        // Extract params
        auto modelId = params.model_id;
        auto& mirrorAxis = params.mirror_axis;

        // Extract API data
        auto& engine = _api->getEngine();
        auto& scene = engine.getScene();

        // Extract model data
        auto& descriptor = brayns::ExtractModel::fromId(scene, modelId);
        auto& model = descriptor.getModel();
        auto& bounds = model.getBounds();
        auto dimensions = (bounds.getMax() - bounds.getMin());
        auto min = bounds.getMin();
        auto max = bounds.getMax();

        // Check axis to skip
        std::vector<uint32_t> skipMirrorAxis;
        for (uint32_t axis = 0; axis < 3; ++axis)
        {
            if (mirrorAxis[axis] < 0.)
                min[axis] -= dimensions[axis] * std::fabs(mirrorAxis[axis]);
            else if (mirrorAxis[axis] > 0)
                max[axis] += dimensions[axis] * std::fabs(mirrorAxis[axis]);
            else
                skipMirrorAxis.push_back(axis);
        }

        // Center of model bounds
        brayns::Vector3f center = (max + min) * 0.5;

        // Spheres
        auto& sphereMap = model.getSpheres();
        for (auto& entry : sphereMap)
        {
            std::vector<brayns::Sphere> tempBuf;
            tempBuf.reserve(entry.second.size());
            for (const brayns::Sphere& sphere : entry.second)
            {
                auto toCenter = (center - sphere.center) * 2.f;
                for (const auto skipAxis : skipMirrorAxis)
                    toCenter[skipAxis] = 0.f;
                tempBuf.emplace_back(sphere.center + toCenter, sphere.radius,
                                     sphere.userData);
            }
            entry.second.insert(entry.second.end(), tempBuf.begin(),
                                tempBuf.end());
        }

        // Cones
        auto& conesMap = model.getCones();
        for (auto& entry : conesMap)
        {
            std::vector<brayns::Cone> tempBuf;
            tempBuf.reserve(entry.second.size());
            for (const brayns::Cone& cone : entry.second)
            {
                auto toCenter = (center - cone.center) * 2.f;
                auto toUp = (center - cone.up) * 2.f;
                for (const auto skipAxis : skipMirrorAxis)
                {
                    toCenter[skipAxis] = 0.f;
                    toUp[skipAxis] = 0.f;
                }
                tempBuf.emplace_back(cone.center + toCenter, cone.up + toUp,
                                     cone.centerRadius, cone.upRadius,
                                     cone.userData);
            }
            entry.second.insert(entry.second.end(), tempBuf.begin(),
                                tempBuf.end());
        }

        // Cylinders
        auto& cylindersMap = model.getCylinders();
        for (auto& entry : cylindersMap)
        {
            std::vector<brayns::Cylinder> tempBuf;
            tempBuf.reserve(entry.second.size());
            for (brayns::Cylinder& cylinder : entry.second)
            {
                auto toCenter = (center - cylinder.center) * 2.f;
                auto toUp = (center - cylinder.up) * 2.f;
                for (const auto skipAxis : skipMirrorAxis)
                {
                    toCenter[skipAxis] = 0.f;
                    toUp[skipAxis] = 0.f;
                }
                tempBuf.emplace_back(cylinder.center + toCenter,
                                     cylinder.up + toUp, cylinder.radius,
                                     cylinder.userData);
            }
            entry.second.insert(entry.second.end(), tempBuf.begin(),
                                tempBuf.end());
        }

        // SDF geometry
        brayns::SDFGeometryData& sdfGeometry = model.getSDFGeometryData();
        const size_t originalOffset = sdfGeometry.geometries.size();
        std::vector<brayns::SDFGeometry> tempBuf;
        tempBuf.reserve(originalOffset);
        for (auto& entry : sdfGeometry.geometryIndices)
        {
            std::vector<uint64_t> extraIndices;
            extraIndices.reserve(entry.second.size());
            for (const auto geomIndex : entry.second)
            {
                const brayns::SDFGeometry& geom =
                    sdfGeometry.geometries[geomIndex];
                auto toP0 = (center - geom.p0) * 2.f;
                auto toP1 = (center - geom.p1) * 2.f;
                for (const auto skipAxis : skipMirrorAxis)
                {
                    toP0[skipAxis] = 0.f;
                    toP1[skipAxis] = 0.f;
                }
                // Update geometry indices (materials)
                extraIndices.push_back(originalOffset + tempBuf.size());
                // Update neighbours
                auto newNeighbours = sdfGeometry.neighbours[geomIndex];
                for (size_t i = 0; i < newNeighbours.size(); ++i)
                    newNeighbours[i] += originalOffset;
                sdfGeometry.neighbours.push_back(newNeighbours);
                // Insert the new geometry
                tempBuf.emplace_back();
                brayns::SDFGeometry& last = tempBuf.back();
                last = geom;
                last.p0 += toP0;
                last.p1 += toP1;
            }
            entry.second.insert(entry.second.end(), extraIndices.begin(),
                                extraIndices.end());
        }
        sdfGeometry.geometries.insert(sdfGeometry.geometries.end(),
                                      tempBuf.begin(), tempBuf.end());
        tempBuf.clear();

        // Recompute bounds
        model.updateBounds();
        auto tr = descriptor.getTransformation();
        tr.setRotationCenter((max + min) * 0.5);
        descriptor.setTransformation(tr);
        descriptor.computeBounds();

        // Commit
        descriptor.markModified();
        scene.markModified();
        engine.triggerRender();
    }

private:
    brayns::PluginAPI* _api;
};

class MirrorModelEntrypoint
    : public brayns::Entrypoint<MirrorModelMessage, brayns::EmptyMessage>
{
public:
    virtual std::string getName() const override { return "mirror-model"; }

    virtual std::string getDescription() const override
    {
        return "Mirrors a model along a given axis";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        ModelMirrorer mirrorer(getApi());
        mirrorer.mirrorModel(params);
        request.reply(brayns::EmptyMessage());
    }
};
