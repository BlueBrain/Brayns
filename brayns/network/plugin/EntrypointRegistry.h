/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include <brayns/network/entrypoints/AnimationParametersEntrypoint.h>
#include <brayns/network/entrypoints/ApplicationParametersEntrypoint.h>
#include <brayns/network/entrypoints/CameraEntrypoint.h>
#include <brayns/network/entrypoints/CancelEntrypoint.h>
#include <brayns/network/entrypoints/ExitLaterEntrypoint.h>
#include <brayns/network/entrypoints/ImageJpegEntrypoint.h>
#include <brayns/network/entrypoints/ImageStreamingModeEntrypoint.h>
#include <brayns/network/entrypoints/InspectEntrypoint.h>
#include <brayns/network/entrypoints/QuitEntrypoint.h>
#include <brayns/network/entrypoints/RendererEntrypoint.h>
#include <brayns/network/entrypoints/RequestModelUploadEntrypoint.h>
#include <brayns/network/entrypoints/ResetCameraEntrypoint.h>
#include <brayns/network/entrypoints/SceneEntrypoint.h>
#include <brayns/network/entrypoints/SchemaEntrypoint.h>
#include <brayns/network/entrypoints/SnapshotEntrypoint.h>
#include <brayns/network/entrypoints/StatisticsEntrypoint.h>
#include <brayns/network/entrypoints/TestEntrypoint.h>
#include <brayns/network/entrypoints/TriggerJpegStreamEntrypoint.h>
#include <brayns/network/entrypoints/VersionEntrypoint.h>
#include <brayns/network/entrypoints/VolumeParametersEntrypoint.h>

namespace brayns
{
class EntrypointRegistry
{
public:
    static void registerEntrypoints(ActionInterface& interface)
    {
        interface.add<GetAnimationParametersEntrypoint>();
        interface.add<SetAnimationParametersEntrypoint>();
        interface.add<GetCameraEntrypoint>();
        interface.add<SetCameraEntrypoint>();
        interface.add<ImageJpegEntrypoint>();
        interface.add<TriggerJpegStreamEntrypoint>();
        interface.add<ImageStreamingModeEntrypoint>();
        interface.add<GetRendererEntrypoint>();
        interface.add<SetRendererEntrypoint>();
        interface.add<VersionEntrypoint>();
        interface.add<GetApplicationParametersEntrypoint>();
        interface.add<SetApplicationParametersEntrypoint>();
        interface.add<GetVolumeParametersEntrypoint>();
        interface.add<SetVolumeParametersEntrypoint>();
        interface.add<GetSceneEntrypoint>();
        interface.add<SetSceneEntrypoint>();
        interface.add<GetStatisticsEntrypoint>();
        interface.add<SchemaEntrypoint>();
        interface.add<InspectEntrypoint>();
        interface.add<QuitEntrypoint>();
        interface.add<ExitLaterEntrypoint>();
        interface.add<ResetCameraEntrypoint>();
        interface.add<SnapshotEntrypoint>();
        interface.add<RequestModelUploadEntrypoint>();
        interface.add<CancelEntrypoint>();
        interface.add<TestEntrypoint>();
    }
};
} // namespace brayns