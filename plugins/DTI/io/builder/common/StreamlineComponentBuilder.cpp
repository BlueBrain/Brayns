/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "StreamlineComponentBuilder.h"

#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/components/GeometryViews.h>
#include <brayns/engine/geometry/types/Capsule.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>
#include <brayns/engine/systems/GeometryDataSystem.h>

#include <api/StreamlineColorGenerator.h>
#include <components/StreamlineColors.h>

#include <ospray/ospray_cpp/ext/rkcommon.h>

namespace
{
using StreamlineMap = std::map<uint64_t, dti::StreamlineData>;
using StreamlineGeometry = std::vector<brayns::Capsule>;
using StreamlineGeometries = std::vector<StreamlineGeometry>;
using StreamlineColor = std::vector<brayns::Vector4f>;
using StreamlineColors = std::vector<StreamlineColor>;

class GeometryGenerator
{
public:
    static StreamlineGeometries generate(const StreamlineMap &data, float radius)
    {
        StreamlineGeometries geometryList;
        geometryList.reserve(data.size());

        for (auto &[row, streamline] : data)
        {
            auto &points = streamline.points;

            auto &geometries = geometryList.emplace_back();
            geometries.reserve(points.size() - 1);

            for (size_t i = 1; i < points.size(); ++i)
            {
                auto &start = points[i - 1];
                auto &end = points[i];
                geometries.push_back(brayns::CapsuleFactory::cylinder(start, end, radius));
            }
        }

        return geometryList;
    }
};

class ModelBuilder
{
public:
    explicit ModelBuilder(brayns::Model &model):
        _model(model)
    {
    }

    void addComponents(StreamlineGeometries inputGeometry)
    {
        auto count = inputGeometry.size();
        auto &geometries = _addGeometryComponent(count);
        auto &views = _addViewComponent(count);
        auto &colors = _addColorComponent(count);

        for (size_t i = 0; i < inputGeometry.size(); ++i)
        {
            auto &capsules = inputGeometry[i];
            auto &color = colors.emplace_back(dti::StreamlineColorGenerator::generate(capsules));
            auto &geometry = geometries.emplace_back(std::move(capsules));
            auto &view = views.emplace_back(geometry);
            view.setColorPerPrimitive(ospray::cpp::SharedData(color));
        }
    }

    void addSystems()
    {
        auto &systems = _model.getSystems();
        systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Geometries>>();
        systems.setDataSystem<brayns::GeometryDataSystem>();
    }

private:
    std::vector<brayns::Geometry> &_addGeometryComponent(size_t count)
    {
        auto &geometryComponent = _model.getComponents().add<brayns::Geometries>();
        auto &geometries = geometryComponent.elements;
        geometries.reserve(count);
        return geometries;
    }

    std::vector<brayns::GeometryView> &_addViewComponent(size_t count)
    {
        auto &viewComponent = _model.getComponents().add<brayns::GeometryViews>();
        auto &views = viewComponent.elements;
        views.reserve(count);
        return views;
    }

    std::vector<std::vector<brayns::Vector4f>> &_addColorComponent(size_t count)
    {
        auto &colorComponent = _model.getComponents().add<dti::StreamlineColors>();
        auto &colors = colorComponent.elements;
        colors.reserve(count);
        return colors;
    }

private:
    brayns::Model &_model;
};
}

namespace dti
{
void StreamlineComponentBuilder::build(const StreamlineMap &data, float radius, brayns::Model &model)
{
    auto builder = ModelBuilder(model);
    builder.addComponents(GeometryGenerator::generate(data, radius));
    builder.addSystems();
}
}
