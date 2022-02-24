/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include <brayns/network/client/ClientRequest.h>
#include <brayns/network/entrypoint/EntrypointRegistry.h>

#include "ITask.h"

namespace brayns
{
class BinaryTask : public ITask
{
public:
    /**
     * @brief Construct a task to process the given binary request.
     *
     * @param request Binary request to process.
     * @param entrypoints Entrypoints notified by request.
     */
    BinaryTask(ClientRequest request, const EntrypointRegistry &entrypoints);

    /**
     * @brief Get the client who sent the request.
     *
     * @return const ClientRef& Client ref.
     */
    virtual const ClientRef &getClient() const override;

    /**
     * @brief Return an empty ID.
     *
     * @return const RequestId& Empty ID.
     */
    virtual const RequestId &getId() const override;

    /**
     * @brief Return an empty string.
     *
     * @return const std::string& No methods.
     */
    virtual const std::string &getMethod() const override;

    /**
     * @brief Notify entrypoints with onBinary(request).
     *
     */
    virtual void run() override;

    /**
     * @brief Always throw.
     *
     * @throw TaskNotCancellableException Task not cancellable.
     */
    virtual void cancel() override;

private:
    ClientRequest _request;
    const EntrypointRegistry &_entrypoints;
};
} // namespace brayns
