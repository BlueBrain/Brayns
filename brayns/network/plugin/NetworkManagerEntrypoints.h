/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/network/entrypoints/AddClipPlaneEntrypoint.h>
#include <brayns/network/entrypoints/AddLightEntrypoint.h>
#include <brayns/network/entrypoints/AddModelEntrypoint.h>
#include <brayns/network/entrypoints/AnimationParametersEntrypoint.h>
#include <brayns/network/entrypoints/ApplicationParametersEntrypoint.h>
#include <brayns/network/entrypoints/CameraEntrypoint.h>
#include <brayns/network/entrypoints/CameraParamsEntrypoint.h>
#include <brayns/network/entrypoints/CancelEntrypoint.h>
#include <brayns/network/entrypoints/ChunkEntrypoint.h>
#include <brayns/network/entrypoints/ClearLightsEntrypoint.h>
#include <brayns/network/entrypoints/ExitLaterEntrypoint.h>
#include <brayns/network/entrypoints/ExportFramesToDiskEntryPoint.h>
#include <brayns/network/entrypoints/GetClipPlanesEntrypoint.h>
#include <brayns/network/entrypoints/GetExportFramesProgressEntrypoint.h>
#include <brayns/network/entrypoints/GetInstancesEntrypoint.h>
#include <brayns/network/entrypoints/GetLightsEntrypoint.h>
#include <brayns/network/entrypoints/GetLoadersEntrypoint.h>
#include <brayns/network/entrypoints/GetModelEntrypoint.h>
#include <brayns/network/entrypoints/ImageJpegEntrypoint.h>
#include <brayns/network/entrypoints/ImageStreamingModeEntrypoint.h>
#include <brayns/network/entrypoints/InspectEntrypoint.h>
#include <brayns/network/entrypoints/LoadersSchemaEntrypoint.h>
#include <brayns/network/entrypoints/ModelPropertiesEntrypoint.h>
#include <brayns/network/entrypoints/ModelTransferFunctionEntrypoint.h>
#include <brayns/network/entrypoints/QuitEntrypoint.h>
#include <brayns/network/entrypoints/RegistryEntrypoint.h>
#include <brayns/network/entrypoints/RemoveClipPlanesEntrypoint.h>
#include <brayns/network/entrypoints/RemoveLightsEntrypoint.h>
#include <brayns/network/entrypoints/RemoveModelEntrypoint.h>
#include <brayns/network/entrypoints/RendererEntrypoint.h>
#include <brayns/network/entrypoints/RendererParamsEntrypoint.h>
#include <brayns/network/entrypoints/RequestModelUploadEntrypoint.h>
#include <brayns/network/entrypoints/ResetCameraEntrypoint.h>
#include <brayns/network/entrypoints/SceneEntrypoint.h>
#include <brayns/network/entrypoints/SchemaEntrypoint.h>
#include <brayns/network/entrypoints/SnapshotEntrypoint.h>
#include <brayns/network/entrypoints/StatisticsEntrypoint.h>
#include <brayns/network/entrypoints/TriggerJpegStreamEntrypoint.h>
#include <brayns/network/entrypoints/UpdateClipPlaneEntrypoint.h>
#include <brayns/network/entrypoints/UpdateInstanceEntrypoint.h>
#include <brayns/network/entrypoints/UpdateModelEntrypoint.h>
#include <brayns/network/entrypoints/VersionEntrypoint.h>
#include <brayns/network/entrypoints/VolumeParametersEntrypoint.h>

#include "NetworkManager.h"

namespace brayns
{
/**
 * @brief Helper class to register all core entrypoints.
 *
 */
class NetworkManagerEntrypoints
{
public:
    /**
     * @brief Register an instance of all core entrypoints in interface.
     *
     * @param plugin Plugin registering the entrypoint.
     */
    static void load(NetworkManager& plugin)
    {
        plugin.add<GetAnimationParametersEntrypoint>();
        plugin.add<SetAnimationParametersEntrypoint>();
        plugin.add<GetCameraEntrypoint>();
        plugin.add<SetCameraEntrypoint>();
        plugin.add<ImageJpegEntrypoint>();
        plugin.add<TriggerJpegStreamEntrypoint>();
        plugin.add<ImageStreamingModeEntrypoint>();
        plugin.add<GetRendererEntrypoint>();
        plugin.add<SetRendererEntrypoint>();
        plugin.add<VersionEntrypoint>();
        plugin.add<GetApplicationParametersEntrypoint>();
        plugin.add<SetApplicationParametersEntrypoint>();
        plugin.add<GetVolumeParametersEntrypoint>();
        plugin.add<SetVolumeParametersEntrypoint>();
        plugin.add<GetSceneEntrypoint>();
        plugin.add<SetSceneEntrypoint>();
        plugin.add<GetStatisticsEntrypoint>();
        plugin.add<SchemaEntrypoint>();
        plugin.add<InspectEntrypoint>();
        plugin.add<QuitEntrypoint>();
        plugin.add<ExitLaterEntrypoint>();
        plugin.add<ResetCameraEntrypoint>();
        plugin.add<SnapshotEntrypoint>();
        plugin.add<RequestModelUploadEntrypoint>();
        plugin.add<ChunkEntrypoint>();
        plugin.add<AddModelEntrypoint>();
        plugin.add<RemoveModelEntrypoint>();
        plugin.add<UpdateModelEntrypoint>();
        plugin.add<GetModelEntrypoint>();
        plugin.add<GetModelPropertiesEntrypoint>();
        plugin.add<SetModelPropertiesEntrypoint>();
        plugin.add<ModelPropertiesSchemaEntrypoint>();
        plugin.add<GetModelTransferFunctionEntrypoint>();
        plugin.add<SetModelTransferFunctionEntrypoint>();
        plugin.add<AddClipPlaneEntrypoint>();
        plugin.add<GetClipPlanesEntrypoint>();
        plugin.add<UpdateClipPlaneEntrypoint>();
        plugin.add<RemoveClipPlanesEntrypoint>();
        plugin.add<GetInstancesEntrypoint>();
        plugin.add<UpdateInstanceEntrypoint>();
        plugin.add<GetLoadersEntrypoint>();
        plugin.add<LoadersSchemaEntrypoint>();
        plugin.add<GetCameraParamsEntrypoint>();
        plugin.add<SetCameraParamsEntrypoint>();
        plugin.add<GetRendererParamsEntrypoint>();
        plugin.add<SetRendererParamsEntrypoint>();
        plugin.add<GetLightsEntrypoint>();
        plugin.add<AddLightDirectionalEntrypoint>();
        plugin.add<AddLightSphereEntrypoint>();
        plugin.add<AddLightQuadEntrypoint>();
        plugin.add<AddLightSpotEntrypoint>();
        plugin.add<AddLightAmbientEntrypoint>();
        plugin.add<RemoveLightsEntrypoint>();
        plugin.add<ClearLightsEntrypoint>();
        plugin.add<CancelEntrypoint>();
        plugin.add<RegistryEntrypoint>();
        plugin.add<ExportFramesToDiskEntrypoint>();
        plugin.add<GetExportFramesProgressEntrypoint>();
    }
};
} // namespace brayns
