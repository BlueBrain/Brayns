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

#include <brayns/network/adapters/ApplicationParametersAdapter.h>
#include <brayns/network/entrypoint/ObjectEntrypoint.h>

namespace brayns
{
class GetApplicationParametersEntrypoint : public GetEntrypoint<ApplicationParameters>
{
public:
    GetApplicationParametersEntrypoint(const ApplicationParameters &parameters, INetworkInterface &interface);

    virtual std::string getName() const override;
    virtual std::string getDescription() const override;
};

class SetApplicationParametersEntrypoint : public SetEntrypoint<ApplicationParameters>
{
public:
    SetApplicationParametersEntrypoint(ApplicationParameters &parameters, Engine &engine);

    virtual std::string getName() const override;
    virtual std::string getDescription() const override;
};
} // namespace brayns
