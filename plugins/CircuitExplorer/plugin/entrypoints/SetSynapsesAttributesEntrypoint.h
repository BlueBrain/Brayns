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

#include <sstream>

#include <brayns/network/entrypoint/Entrypoint.h>

#include <plugin/CircuitExplorerContext.h>
#include <plugin/adapters/SynapseAttributesAdapter.h>
#include <plugin/io/SynapseJSONLoader.h>

class HtmlColorParser
{
public:
    static std::vector<brayns::Vector3f> parse(
        const std::vector<std::string>& htmlColors)
    {
        std::vector<brayns::Vector3f> colors;
        colors.reserve(htmlColors.size());
        for (const auto& htmlColor : htmlColors)
        {
            colors.push_back(_parseColor(htmlColor));
        }
        return colors;
    }

private:
    static brayns::Vector3f _parseColor(const std::string& htmlColor)
    {
        if (htmlColor.empty())
        {
            return {};
        }
        auto hexCode = htmlColor;
        if (hexCode[0] == '#')
        {
            hexCode.erase(0, 1);
        }
        return {
            _parseHex(hexCode.substr(0, 2)),
            _parseHex(hexCode.substr(2, 2)),
            _parseHex(hexCode.substr(4, 2)),
        };
    }

    static float _parseHex(const std::string& hexCode)
    {
        std::istringstream stream(hexCode);
        int result;
        stream >> std::hex >> result;
        return float(result) / 255.0f;
    }
};

class SetSynapsesAttributesEntrypoint
    : public brayns::Entrypoint<SynapseAttributes, brayns::EmptyMessage>
{
public:
    SetSynapsesAttributesEntrypoint(CircuitExplorerContext& context)
        : _attributes(&context.getSynapseAttributes())
    {
    }

    virtual std::string getName() const override
    {
        return "set-synapses-attributes";
    }

    virtual std::string getDescription() const override
    {
        return "Set synapses specific attributes for a given model";
    }

    virtual void onRequest(const Request& request) override
    {
        *_attributes = request.getParams();
        auto& engine = getApi().getEngine();
        auto& scene = engine.getScene();
        SynapseJSONLoader loader(scene, *_attributes);
        auto colors = _parseColors();
        auto model = loader.importSynapsesFromGIDs(*_attributes, colors);
        scene.addModel(model);
        scene.markModified();
        request.reply(brayns::EmptyMessage());
    }

private:
    std::vector<brayns::Vector3f> _parseColors()
    {
        auto& htmlColors = _attributes->htmlColors;
        return HtmlColorParser::parse(htmlColors);
    }

    SynapseAttributes* _attributes;
};