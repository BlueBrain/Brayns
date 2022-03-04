/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#pragma once

#include <brayns/common/Bounds.h>
#include <brayns/engine/EngineObject.h>

#include <memory>
#include <string_view>
#include <typeindex>
#include <unordered_map>

namespace brayns
{
class Model;

/**
 * @brief The Component class is an unit which is added to the model to add extra data and functionality
 */
class Component
{
public:
    using Ptr = std::unique_ptr<Component>;

    virtual ~Component() = default;

    /**
     * @brief Returns the size in bytes of the component.
     */
    virtual uint64_t getSizeInBytes() const noexcept = 0;

protected:
    /**
     * @brief Returns a reference to the model that owns this component. This reference is only valid after the
     * component has been added to the model.
     * @throws runtime_error if the method is accessed before the component is initialized
     */
    Model& getModel();

    /**
     * @brief called when the component is added to the model. Does nothing by default.
     */
    virtual void onStart();

    /**
     * @brief onPreRender called before the commit+rendering process happens. Does nothing by default.
     */
    virtual void onPreRender();

    /**
     * @brief onPostRender called after the commit+rendering process happens. Does nothing by default.
     */
    virtual void onPostRender();

    /**
     * @brief onCommit called during the commit process. Does nothing by defualt.
     */
    virtual void onCommit();

    /**
     * @brief onDestroyed called when the component is removed from the model. Does nothing by defualt.
     */
    virtual void onDestroyed();

    /**
     * @brief Compute the bounds of the content of the component, if it applies. Does nothing by default.
     */
    virtual Bounds computeBounds(const Matrix4f& transform) const noexcept;

private:
    friend class ModelComponentContainer;
    friend class Model;

    Model* _model;
};

class ModelComponentContainer
{
public:
    template<typename T, typename ...Args>
    T& addComponent(Args&& ...args)
    {
        static_assert(std::is_base_of_v<Component, T>, "Model components must inherit from Component");

        const std::type_index ti = typeid(T);
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        auto componentPtr = component.get();
        _components[ti] = std::move(component);

        componentPtr->onStart();

        return *componentPtr;
    }

    template<typename T>
    T& getComponent()
    {
        const std::type_index ti = typeid(T);
        auto it = _components.find(ti);
        if(it == _components.end())
        {
            throw std::invalid_argument("No component of the given type registered");
        }

        return dynamic_cast<T&>(*(it->second));
    }

    template<typename T>
    void removeComponent()
    {
        const std::type_index ti = typeid(T);
        auto it = _components.find(ti);
        if(it == _components.end())
        {
            throw std::invalid_argument("No component of the given type registered");
        }

        auto& component = *(it->second);
        component.onDestroyed();

        _components.erase(it);
    }

    uint64_t getByteSize() const noexcept;

    /**
     * @brief Calls all the components 'onPreRender' in this container
     */
    void onPreRender();

    /**
     * @brief Calls all the components 'onPreRender' in this container
     */
    void onPostRender();

    /**
     * @brief Calls all the components 'onCommit' in this container
     */
    void onCommit();

    /**
     * @brief Calls all the components 'onDestroyed' in this container
     */
    void onDestroyed();
private:
    std::unordered_map<std::type_index, Component::Ptr> _components;
};
}
