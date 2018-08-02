/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 *
 * Responsible Author: Daniel.Nachbaur@epfl.ch
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

#include <brayns/common/types.h>

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
 */
class ActionInterface
{
public:
    virtual ~ActionInterface() = default;

    /**
     * Register an action with no parameter and no return value.
     *
     * @param desc description of the action/RPC
     * @param action the action to perform on an incoming notification
     */
    virtual void registerNotification(const RpcDescription& desc,
                                      const std::function<void()>& action) = 0;

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
        const std::function<void(PropertyMap)>& action) = 0;

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
        const std::function<PropertyMap(PropertyMap)>& action) = 0;

    /**
     * Register an action with no parameter and a property map as the return
     * value.
     *
     * @param desc description of the action/RPC
     * @param output the property map layout that is returned on a successful
     *               request
     * @param action the action to perform on an incoming request
     */
    virtual void registerRequest(
        const RpcDescription& desc, const PropertyMap& output,
        const std::function<PropertyMap()>& action) = 0;

    /** Register an action with no parameter and no return value. */
    void registerNotification(const std::string& name,
                              const std::function<void()>& action)
    {
        _registerNotification(name, [action] { action(); });
    }

    /** Register an action with a parameter and no return value. */
    template <typename Params>
    void registerNotification(const std::string& name,
                              const std::function<void(Params)>& action)
    {
        _registerNotification(name, [action](const std::string& param) {
            Params params;
            if (!from_json(params, param))
                throw std::runtime_error("from_json failed");
            action(params);
        });
    }

    /** Register an action with a parameter and a return value. */
    template <typename Params, typename RetVal>
    void registerRequest(const std::string& name,
                         const std::function<RetVal(Params)>& action)
    {
        _registerRequest(name, [action](const std::string& param) {
            Params params;
            if (!from_json(params, param))
                throw std::runtime_error("from_json failed");
            return to_json(action(params));
        });
    }

    /** Register an action with no parameter and a return value. */
    template <typename RetVal>
    void registerRequest(const std::string& name,
                         const std::function<RetVal()>& action)
    {
        _registerRequest(name, [action] { return to_json(action()); });
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
}
