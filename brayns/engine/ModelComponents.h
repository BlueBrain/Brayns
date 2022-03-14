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
#include <brayns/parameters/ParametersManager.h>

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
    virtual size_t getSizeInBytes() const noexcept = 0;

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
    virtual void onPreRender(const ParametersManager& params);

    /**
     * @brief onPostRender called after the commit+rendering process happens. Does nothing by default.
     */
    virtual void onPostRender(const ParametersManager& params);

    /**
     * @brief Called during the commit process. Does nothing by default. Should return true when the result of
     * the commit operation requires the model to be committed as well.
     */
    virtual bool commit();

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

    Model* _owner {nullptr};
    std::type_index _type;
};

class ModelComponentContainer
{
private:
    /**
     * @brief Adds a new component of the given type to the component list.
     */
    template<typename T, typename ...Args>
    T& addComponent(Args&& ...args)
    {
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        auto componentPtr = component.get();
        _components.push_back(std::move(component));

        componentPtr._type = typeid(T);

        return *componentPtr;
    }

    /**
     * @brief Returns a reference to the component of the given type. If multiple components of the
     * same type are present, the first one added is returned.
     * @throws std::invalid_argument if there is no component of the given type
     */
    template<typename T>
    T& getComponent()
    {
        const std::type_index ti = typeid(T);
        auto begin = _components.begin();
        auto end = _components.end();

        auto it = std::find_if(begin, end, [type = ti](std::unique_ptr<Component> &component)
        {
            return component->_type == type;
        });

        if(it == end)
        {
            throw std::invalid_argument("No component of the given type registered");
        }

        return dynamic_cast<T&>(*(*it).get());
    }

    /**
     * @brief Returns all components of a given type
     */
    template<typename T>
    std::vector<Component*> getAllComponents()
    {
        std::vector<T *> result;
        auto it = _components.begin();
        auto end = _components.end();

        const std::type_index ti = typeid(T);
        do
        {
            it = std::find_if(it, end, [type = ti](std::unique_ptr<Component> &component)
            {
                return component->_type = type;
            });

            if(it != end)
            {
                result.push_back(static_cast<T*>((*it).get()));
                ++it;
            }
        }
        while(it != end);

        return result;
    }

    /**
     * @brief Removes a component of the given type. If there are multiple components of the same type,
     * the first one that was added is removed
     */
    template<typename T>
    void removeComponent()
    {
        const std::type_index ti = typeid(T);
        auto begin = _components.begin();
        auto end = _components.end();

        auto it = std::find_if(begin, end, [type = ti](std::unique_ptr<Component> &component)
        {
            return component->_type == type;
        });
        if(it == _components.end())
        {
            throw std::invalid_argument("No component of the given type registered");
        }

        auto& component = (*it);
        component->onDestroyed();

        _components.erase(it);
    }

    /**
     * @brief Returns the size in bytes of this component list. It will call getSizeInBytes for each
     * component
     */
    size_t getSizeInBytes() const noexcept;

    /**
     * @brief Calls all the components 'onPreRender' in this container
     */
    void onPreRender(const ParametersManager& params);

    /**
     * @brief Calls all the components 'onPreRender' in this container
     */
    void onPostRender(const ParametersManager& params);

    /**
     * @brief Calls all the components 'commit' in this container. Returns true if any component returned true
     * from its commit implementation
     */
    bool commit();

    /**
     * @brief Calls all the components 'onDestroyed' in this container
     */
    void onDestroyed();
private:
    friend class Model;

    std::vector<Component::Ptr> _components;
};
}
