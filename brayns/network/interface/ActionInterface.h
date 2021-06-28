/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 *
 * Responsible Authors: Daniel.Nachbaur@epfl.ch
 *                      Nadir Román Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/common/log.h>
#include <brayns/common/propertymap/PropertyMap.h>
#include <brayns/common/types.h>

#include <brayns/network/entrypoint/EntrypointHolder.h>
#include <brayns/network/messages/ActionMessage.h>

#include <functional>
#include <string>

namespace brayns
{
/**
 * Interface for registering actions, namely notifications which have no return
 * values with an optional parameter, and requests which return a value after
 * processing.
 *
 * The encoding of the parameter and return value is restricted to JSON.
 *
 * The parameters object must be deserializable by a free function:
 * @code
 * bool from_json(Params& object, const std::string& json)
 * @endcode
 *
 * The return type must be serializable by a free function:
 * @code
 * std::string to_json(const RetVal&)
 * @endcode
 *
 * The new way of registering entrypoint is using the IEntrypoint interface.
 */
class ActionInterface
{
public:
    virtual ~ActionInterface() = default;

    /**
     * @brief Find an entrypoint with the given name.
     *
     * @param name Entrypoint name (method in request).
     * @return const EntrypointHolder* Pointer to the corresponding entrypoint
     * or null if not found.
     */
    virtual const EntrypointHolder* findEntrypoint(
        const std::string& name) const = 0;

    /**
     * @brief Register an entrypoint implementing IEntrypoint interface.
     *
     * @param entrypoint Pointer to an IEntrypoint implementation.
     */
    virtual void addEntrypoint(EntrypointPtr entrypoint) = 0;

    /**
     * @brief Shortcut to add an entrypoint from its type.
     *
     * @tparam T Concrete type of the entrypoint (subclass of IEntrypoint).
     * @tparam Args Types of the arguments to pass to the constructor of T.
     * @param args Arguments to pass to the constructor of T.
     */
    template <typename T, typename... Args>
    void add(Args&&... args)
    {
        static_assert(std::is_base_of<IEntrypoint, T>());
        addEntrypoint(std::make_unique<T>(std::forward<Args>(args)...));
    }

    /**
     * Register an action with no parameter and no return value.
     *
     * @param desc description of the action/RPC
     * @param action the action to perform on an incoming notification
     */
    virtual void registerNotification(const RpcDescription& desc,
                                      const std::function<void()>& action){};

    /**
     * Register an action with a property map as the parameter and no return
     * value.
     *
     * @param desc description of the action/RPC
     * @param input the acceptable property map as the parameter for the RPC
     * @param action the action to perform on an incoming notification
     */
    virtual void registerNotification(
        const RpcParameterDescription& desc, const PropertyMap& input,
        const std::function<void(PropertyMap)>& action){};

    /**
     * Register an action with a property map as the parameter and a property
     * map as the return value.
     *
     * @param desc description of the action/RPC
     * @param input the acceptable property map as the parameter for the RPC
     * @param output the property map layout that is returned on a successful
     *               request
     * @param action the action to perform on an incoming request
     */
    virtual void registerRequest(
        const RpcParameterDescription& desc, const PropertyMap& input,
        const PropertyMap& output,
        const std::function<PropertyMap(PropertyMap)>& action){};

    /**
     * Register an action with no parameter and a property map as the return
     * value.
     *
     * @param desc description of the action/RPC
     * @param output the property map layout that is returned on a successful
     *               request
     * @param action the action to perform on an incoming request
     */
    virtual void registerRequest(const RpcDescription& desc,
                                 const PropertyMap& output,
                                 const std::function<PropertyMap()>& action){};

    /** Register an action with no parameter and no return value. */
    void registerNotification(const std::string& name,
                              const std::function<void()>& action)
    {
        _registerNotification(name, [action] { action(); });
    }

    template <typename Params, typename = std::enable_if_t<
                                   std::is_base_of<Message, Params>::value>>
    void registerNotification(const RpcParameterDescription& desc,
                              const std::function<void(Params)>& action)
    {
        Params p;
        BRAYNS_INFO << "Registering notification " << desc.methodName
                    << std::endl;
        registerNotification(desc, p.getPropertyMap(),
                             [action](PropertyMap map)
                             {
                                 Params par(map);
                                 par.fromPropertyMap();
                                 action(par);
                             });
    }

    template <typename RetVal, typename = std::enable_if_t<
                                   std::is_base_of<Message, RetVal>::value>>
    void registerRequest(const RpcDescription& desc,
                         const std::function<RetVal()>& action)
    {
        RetVal rv;
        BRAYNS_INFO << "Registering request " << desc.methodName << std::endl;
        registerRequest(desc, rv.getPropertyMap(),
                        [action]()
                        {
                            RetVal retVal = action();
                            retVal.toPropertyMap();
                            return retVal.getPropertyMap();
                        });
    }

    template <
        typename Params, typename RetVal,
        typename = std::enable_if_t<std::is_base_of<Message, Params>::value &&
                                    std::is_base_of<Message, RetVal>::value>>
    void registerRequest(const RpcParameterDescription& desc,
                         const std::function<RetVal(Params)>& action)
    {
        Params p;
        RetVal rv;
        BRAYNS_INFO << "Registering request " << desc.methodName << std::endl;
        registerRequest(desc, p.getPropertyMap(), rv.getPropertyMap(),
                        [action](PropertyMap map)
                        {
                            Params par(map);
                            par.fromPropertyMap();

                            RetVal retVal = action(par);
                            retVal.toPropertyMap();

                            return retVal.getPropertyMap();
                        });
    }

protected:
    using RetParamFunc = std::function<std::string(std::string)>;
    using RetFunc = std::function<std::string()>;
    using ParamFunc = std::function<void(std::string)>;
    using VoidFunc = std::function<void()>;

private:
    virtual void _registerRequest(const std::string&, const RetParamFunc&) {}
    virtual void _registerRequest(const std::string&, const RetFunc&) {}
    virtual void _registerNotification(const std::string&, const ParamFunc&) {}
    virtual void _registerNotification(const std::string&, const VoidFunc&) {}
};
} // namespace brayns
