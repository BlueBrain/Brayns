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
#include <brayns/json/JsonType.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/utils/TypeNameDemangler.h>

#include <memory>
#include <string_view>
#include <typeindex>
#include <unordered_map>

#include <ospray/ospray.h>

namespace brayns
{
class Model;

/**
 * @brief The Component class is an unit which is added to the model to add extra data and functionality
 */
class Component
{
public:
    Component() = default;

    Component(const Component &) = delete;
    Component &operator=(const Component &) = delete;

    Component(Component &&) = delete;
    Component &operator=(Component &&) = delete;

    virtual ~Component() = default;

protected:
    /**
     * @brief Returns a reference to the model that owns this component. This reference is only valid after the
     * component has been added to the model.
     * @throws runtime_error if the method is accessed before the component is initialized
     */
    Model &getModel();

    /**
     * @brief called when the component is added to the model. Does nothing by default.
     */
    virtual void onCreate();

    /**
     * @brief onPreRender called before the commit+rendering process happens. Does nothing by default.
     */
    virtual void onPreRender(const ParametersManager &params);

    /**
     * @brief onPostRender called after the commit+rendering process happens. Does nothing by default.
     */
    virtual void onPostRender(const ParametersManager &params);

    /**
     * @brief Called during the commit process. Does nothing by default. Should return true when the result of
     * the commit operation requires the model to be committed as well.
     */
    virtual bool commit();

    /**
     * @brief called when the component is removed from the model. Does nothing by defualt.
     */
    virtual void onDestroy();

    /**
     * @brief Compute the bounds of the content of the component, if it applies. Does nothing by default.
     */
    virtual Bounds computeBounds(const Matrix4f &transform) const noexcept;

    /**
     * @brief Called when a scene inspect event has hitted any geometry of the model that owns this component
     */
    virtual void onInspect(
        const Vector3f &hit,
        OSPGeometricModel modelHandle,
        uint32_t primitiveID,
        JsonObject &writeResult) const noexcept;

private:
    friend class Model;
    friend class ModelComponentContainer;

    Model *_owner{nullptr};
};

class ModelComponentContainer
{
private:
    /**
     * @brief Adds a new component of the given type to the component list.
     */
    template<typename T, typename... Args>
    T &addComponent(Args &&...args)
    {
        auto &entry = _components.emplace_back();

        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        std::type_index type = typeid(T);

        entry.type = type;
        entry.component = std::move(component);

        return static_cast<T &>(*entry.component);
    }

    /**
     * @brief Returns a reference to the component of the given type. If multiple components of the
     * same type are present, the first one added is returned.
     * @throws std::invalid_argument if there is no component of the given type
     */
    template<typename T>
    T &getComponent()
    {
        auto it = _findComponentIterator<T>();
        auto &entry = *it;
        auto &component = *entry.component;
        return static_cast<T &>(component);
    }

    /**
     * @brief Returns all components of a given type
     */
    template<typename T>
    std::vector<Component *> getAllComponents()
    {
        std::vector<T *> result;
        auto it = _components.begin();
        auto end = _components.end();

        const std::type_index ti = typeid(T);
        do
        {
            it = std::find_if(it, end, [type = ti](ComponentEntry &entry) { return entry.type = type; });

            if (it != end)
            {
                auto &entry = *it;
                auto &component = *entry.component;
                result.push_back(static_cast<T *>(&component));
                ++it;
            }
        } while (it != end);

        return result;
    }

    /**
     * @brief Removes a component of the given type. If there are multiple components of the same type,
     * the first one that was added is removed
     */
    template<typename T>
    void removeComponent()
    {
        auto it = _findComponentIterator<T>();
        auto &entry = *it;

        auto &component = *entry.component;
        ;
        component.onDestroyed();

        _components.erase(it);
    }

    /**
     * @brief Removes all components of a given type
     */
    template<typename T>
    void removeAllComponents()
    {
        auto it = _components.begin();
        auto end = _components.end();

        const std::type_index ti = typeid(T);
        do
        {
            it = std::find_if(it, end, [type = ti](ComponentEntry &entry) { return entry.type = type; });

            if (it != end)
            {
                _components.erase(it);
            }
        } while (it != end);
    }

    /**
     * @brief Calls all the components 'onPreRender' in this container
     */
    void onPreRender(const ParametersManager &params);

    /**
     * @brief Calls all the components 'onPreRender' in this container
     */
    void onPostRender(const ParametersManager &params);

    /**
     * @brief Calls all the components 'commit' in this container. Returns true if any component returned true
     * from its commit implementation
     */
    bool commit();

    /**
     * @brief Calls all the components 'onDestroy' in this container
     */
    void onDestroy();

    /**
     * @brief compute the merged bounds of all contained components
     * @param transform The transformation to apply when computing the bounds
     * @return Bounds
     */
    Bounds computeBounds(const Matrix4f &transform) const noexcept;

    /**
     * @brief Calls all the components for an inspect event. The components can write information
     * into the output Json object.
     *
     * @param hit 3D position where the hit occoured
     * @param modelHandle OSPGeometricModel handle that was hit
     * @param primitiveID Index of the primitive that was hit
     * @param writeResult Output parameter where the component might write information to send to the client
     */
    void onInspect(const Vector3f &hit, OSPGeometricModel modelHandle, uint32_t primitiveID, JsonObject &writeResult)
        const noexcept;

private:
    template<typename T>
    auto _findComponentIterator()
    {
        const std::type_index ti = typeid(T);
        auto begin = _components.begin();
        auto end = _components.end();

        auto it = std::find_if(begin, end, [type = ti](ComponentEntry &entry) { return entry.type == type; });
        if (it == _components.end())
        {
            throw std::invalid_argument("No " + TypeNameDemangler::demangle<T>() + " component registered");
        }

        return it;
    }

    friend class Model;

    struct ComponentEntry
    {
        std::type_index type = typeid(void);
        std::unique_ptr<Component> component;
    };

    std::vector<ComponentEntry> _components;
};
}
