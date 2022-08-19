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

#include "ModelGroup.h"

#include <ospray/ospray_cpp/Data.h>

#include <algorithm>

namespace
{
struct GroupParameters
{
    inline static const std::string geometry = "geometry";
    inline static const std::string volume = "volume";
    inline static const std::string clipping = "clippingGeometry";
};

class GroupParameterUpdater
{
public:
    GroupParameterUpdater(const ospray::cpp::Group &group)
        : _group(group)
    {
    }

    template<typename T>
    void update(const std::string &name, const std::vector<T> &handleList)
    {
        if (handleList.empty())
        {
            _group.removeParam(name);
            return;
        }
        _group.setParam(name, ospray::cpp::CopiedData(handleList));
    }

private:
    const ospray::cpp::Group &_group;
};
}

namespace brayns
{
ospray::cpp::Group &ModelGroup::getOsprayGroup() noexcept
{
    return _osprayGroup;
}

bool ModelGroup::commit()
{
    if (!_modified)
    {
        return false;
    }

    GroupParameterUpdater updater(_osprayGroup);
    updater.update(GroupParameters::geometry, _geometries);
    updater.update(GroupParameters::volume, _volumes);
    updater.update(GroupParameters::clipping, _clippers);
    _osprayGroup.commit();
    _modified = false;
    return true;
}
}
