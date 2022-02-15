/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/common/Bounds.h>
#include <brayns/common/Transformation.h>
#include <brayns/engine/EngineObject.h>

#include <ospray/ospray.h>

#include <map>
#include <string_view>

namespace brayns
{
class IModelAction;

class Model : public EngineObject
{
public:
    using Ptr = std::unique_ptr<Model>;
    using MetaData = std::map<std::string, std::string>;

    Model();

    virtual ~Model();

    virtual void onRemoved();

    void setMetaData(MetaData metadata) noexcept;

    const MetaData& getMetaData() const noexcept;

    void addModelAction(std::unique_ptr<IModelAction>&& modelAction) noexcept;

    IModelAction& getModelAction(const std::string& name);

    OSPGroup groupHandle() const noexcept;

private:
    OSPGroup _groupHandle {nullptr};
    MetaData _metadata;

    std::map<std::string, std::unique_ptr<IModelAction>> _modelActions;
};


class ModelInstance : public EngineObject
{
public:
    using Ptr = std::unique_ptr<ModelInstance>;

    ModelInstance(const size_t modelID, Model::Ptr&& model);
    ~ModelInstance();

    const Bounds& getBounds() const noexcept;

    void commit() final;

    Model& getModel() noexcept;
    const Model& getModel() const noexcept;

    void setVisible(const bool val) noexcept;
    bool isVisible() const noexcept;

    void setTranform(const Transformation &transform) noexcept;
    const Transformation& getTransform() const noexcept;

    OSPInstance handle() const noexcept;

private:
    friend class Scene;

    const size_t _modelID {};
    Model::Ptr _model {nullptr};

    bool _visible {true};
    Transformation _transformation;
    Bounds _bounds;

    OSPInstance _instanceHandle {nullptr};
};
} // namespace brayns
