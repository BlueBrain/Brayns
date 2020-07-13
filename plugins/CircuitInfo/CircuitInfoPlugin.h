/* Copyright (c) 2019 EPFL/Blue Brain Project
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

#ifndef CIRCUITINFOPLUGIN_H
#define CIRCUITINFOPLUGIN_H

#include "CircuitInfoParams.h"

#include <brayns/common/ActionInterface.h>
#include <brayns/common/types.h>
#include <brayns/pluginapi/ExtensionPlugin.h>

/**
   The CircuitInfo plugin gives access to circuit information stored
   in sonata-generated circuit files, those which Brion is unable
   to read.
 */
class CircuitInfoPlugin : public brayns::ExtensionPlugin
{
public:
    CircuitInfoPlugin();
    ~CircuitInfoPlugin();
    void init() final;

private:
    CellGIDList _getCellGIDs(const CellGIDListRequest& request);
    CellGIDList _getCellGIDsFromModel(const ModelCellGIDListRequest& request);

    ReportList _getReportList(const ReportListRequest& request);
    ReportInfo _getReportInfo(const ReportInfoRequest& request);

    TargetList _getTargetList(const TargetListRequest& request);

    AfferentGIDList _getAfferentGIDList(const AfferentGIDListRequest& request);
    EfferentGIDList _getEfferentGIDList(const EfferentGIDListRequest& request);

    ProjectionList _getProjectionList(const ProjectionListRequest& request);
    ProjectionAfferentGIDList _getProjectionAfferentGIDList(const ProjectionAfferentGIDListRequest& request);
    ProjectionEfferentGIDList _getProjectionEfferentGIDList(const ProjectionEfferentGIDListRequest& request);
};

#endif
