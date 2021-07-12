/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#include <brayns/network/entrypoint/Entrypoint.h>
#include <brayns/network/messages/VolumeParametersMessage.h>

#include <brayns/parameters/VolumeParameters.h>

namespace brayns
{
class GetVolumeParametersEntrypoint
    : public Entrypoint<EmptyMessage, VolumeParametersMessage>
{
public:
    virtual std::string getName() const override
    {
        return "get-volume-parameters";
    }

    virtual std::string getDescription() const override
    {
        return "Get the current state of the volume parameters";
    }

    virtual void onUpdate() const override
    {
        auto& manager = getApi().getParametersManager();
        auto& volumeParameters = manager.getVolumeParameters();
        if (!volumeParameters.isModified())
        {
            return;
        }
        auto params = _extractVolumeParameters();
        notify(params);
    }

    virtual void onRequest(const Request& request) const override
    {
        auto result = _extractVolumeParameters();
        request.reply(result);
    }

private:
    VolumeParametersMessage _extractVolumeParameters() const
    {
        auto& manager = getApi().getParametersManager();
        auto& volumeParameters = manager.getVolumeParameters();
        VolumeParametersMessage result;
        result.volume_dimensions = volumeParameters.getDimensions();
        result.volume_element_spacing = volumeParameters.getElementSpacing();
        result.volume_offset = volumeParameters.getOffset();
        result.gradient_shading = volumeParameters.getGradientShading();
        result.single_shade = volumeParameters.getSingleShade();
        result.pre_integration = volumeParameters.getPreIntegration();
        result.adaptive_max_sampling_rate =
            volumeParameters.getAdaptiveMaxSamplingRate();
        result.adaptive_sampling = volumeParameters.getAdaptiveSampling();
        result.sampling_rate = volumeParameters.getSamplingRate();
        result.specular = volumeParameters.getSpecular();
        result.clip_box = volumeParameters.getClipBox();
        return result;
    }
};

class SetVolumeParametersEntrypoint
    : public Entrypoint<VolumeParametersMessage, EmptyMessage>
{
public:
    virtual std::string getName() const override
    {
        return "set-volume-parameters";
    }

    virtual std::string getDescription() const override
    {
        return "Set the current state of the volume parameters";
    }

    virtual void onRequest(const Request& request) const override
    {
        auto& params = request.getParams();
        auto& manager = getApi().getParametersManager();
        auto& volumeParameters = manager.getVolumeParameters();
        volumeParameters.setDimensions(params.volume_dimensions);
        volumeParameters.setElementSpacing(params.volume_element_spacing);
        volumeParameters.setOffset(params.volume_offset);
        volumeParameters.setGradientShading(params.gradient_shading);
        volumeParameters.setSingleShade(params.single_shade);
        volumeParameters.setPreIntegration(params.pre_integration);
        volumeParameters.setAdaptiveMaxSamplingRate(params.adaptive_max_sampling_rate);
        volumeParameters.setAdaptiveSampling(params.adaptive_sampling);
        volumeParameters.setSamplingRate(params.sampling_rate);
        volumeParameters.setSpecular(params.specular);
        volumeParameters.setClipBox(params.clip_box);
        triggerRender();
        request.reply(EmptyMessage());
    }
};
} // namespace brayns