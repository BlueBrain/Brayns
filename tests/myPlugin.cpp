/* Copyright (c) 2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include "../plugins/Rockets/jsonSerialization.h"

#include <brayns/common/ActionInterface.h>
#include <brayns/pluginapi/ExtensionPlugin.h>
#include <brayns/pluginapi/PluginAPI.h>

#include "BasicRenderer_ispc.h"
#include <engines/ospray/ispc/render/DefaultMaterial.h>
#include <engines/ospray/ispc/render/utils/AbstractRenderer.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

class MyPlugin : public brayns::ExtensionPlugin
{
public:
    struct Vec2Param : public brayns::Message
    {
        MESSAGE_BEGIN(Vec2Param)
        MESSAGE_ENTRY(brayns::Vector2i, param, "A vec2 param")
    };

    struct StringParam : public brayns::Message
    {
        MESSAGE_BEGIN(StringParam)
        MESSAGE_ENTRY(std::string, param, "A string param");
    };

    MyPlugin(const int argc, const char** argv)
    {
        if (argc > 0)
        {
            std::cout << "Creating plugin with arguments:";
            for (int i = 0; i < argc; i++)
                std::cout << " " << std::string(argv[i]);
            std::cout << std::endl;
        }
    }

    void init() final
    {
        auto actions = _api->getActionInterface();
        REQUIRE(actions);

        // test property map for actions
        actions->registerNotification(
            brayns::RpcDescription{"notify", "A notification with no params"},
            [&] { ++numCalls; });

        brayns::PropertyMap input;
        input.add({"value", 0});
        actions->registerNotification(
            brayns::RpcParameterDescription{"notify-param",
                                            "A notification with property map",
                                            "param", "a beautiful input param"},
            input,
            [&](const brayns::PropertyMap& prop)
            {
                if (prop.find("value"))
                    CHECK_EQ(prop["value"].as<int>(), 42);
                ++numCalls;
            });

        brayns::PropertyMap output;
        output.add({"result", true});
        actions->registerRequest(
            brayns::RpcDescription{"request",
                                   "A request returning a property map"},
            output,
            [&, output = output]
            {
                ++numCalls;
                return output;
            });

        actions->registerRequest(
            brayns::RpcParameterDescription{
                "request-param",
                "A request with a param and returning a property map", "param",
                "another nice input param"},
            input, output,
            [&, output = output](const brayns::PropertyMap& prop)
            {
                ++numCalls;
                auto val = prop["value"].as<int>();
                CHECK_EQ(val, 42);
                return output;
            });

        // test "arbitrary" objects for actions
        actions->registerNotification("hello", [&] { ++numCalls; });
        actions->registerNotification<Vec2Param>(
            {"foo", "Notification Test", "Vec2", "A 2D vector"},
            [&](const Vec2Param& vec)
            {
                ++numCalls;
                CHECK(vec.param == brayns::Vector2i{1, 1});
            });

        actions->registerRequest<StringParam>({"who", "Request Test"},
                                              [&]
                                              {
                                                  ++numCalls;
                                                  StringParam result;
                                                  result.param =
                                                      std::string("me");
                                                  return result;
                                              });
        actions->registerRequest<Vec2Param, Vec2Param>(
            {"echo", "Echo test", "Vec2Param", "2D Integer vector to echo"},
            [&](const Vec2Param& vec)
            {
                ++numCalls;
                return vec;
            });

        // test properties from custom renderer
        brayns::PropertyMap props;
        props.add({"awesome",
                   int32_t(42),
                   {"Best property", "Because it's the best"}});
        _api->getRenderer().setProperties("myrenderer", props);
    }

    ~MyPlugin() { REQUIRE_EQ(numCalls, 10); }
    size_t numCalls{0};
};

class MyRenderer : public brayns::AbstractRenderer
{
public:
    MyRenderer() { ispcEquivalent = ispc::BasicRenderer_create(this); }
    void commit() final
    {
        AbstractRenderer::commit();
        ispc::BasicRenderer_set(getIE(),
                                (_bgMaterial ? _bgMaterial->getIE() : nullptr),
                                _timestamp, spp, _lightPtr, _lightArray.size());
    }
};

OSP_REGISTER_RENDERER(MyRenderer, myrenderer);
OSP_REGISTER_MATERIAL(myrenderer, brayns::DefaultMaterial, default);

extern "C" brayns::ExtensionPlugin* brayns_plugin_create(int argc,
                                                         const char** argv)
{
    return new MyPlugin(argc, argv);
}
