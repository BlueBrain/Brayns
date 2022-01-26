/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Application.h"

#include <brayns/Brayns.h>
#include <brayns/engine/Camera.h>
#include <brayns/engine/Engine.h>
#include <brayns/engine/FrameBuffer.h>
#include <brayns/engine/Renderer.h>
#include <brayns/engine/Scene.h>
#include <brayns/parameters/ParametersManager.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <imconfig.h>
#include <imgui.h>
// NOTE: include samples after imconfig.h and imgui.h
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl2.h>

#include <algorithm>
#include <cassert>
#include <iostream>

#include "../manipulators/FlyingModeManipulator.h"
#include "../manipulators/InspectCenterManipulator.h"

namespace
{
constexpr auto DEFAULT_MOTION_ACCELERATION = 1.5f;
constexpr auto MAX_FRAMEBUFFERS = 2;

Application *appInstance = nullptr;

void glfwKeyCallback(GLFWwindow * /*window*/, int key, int /*scancode*/, int action, int /*mods*/)
{
    appInstance->keyCallback(key, action);
}

void glfwCursorCallback(GLFWwindow * /*window*/, double xpos, double ypos)
{
    appInstance->cursorCallback(xpos, ypos);
}

void glfwMouseButtonCallback(GLFWwindow * /*window*/, int button, int action, int /*mods*/)
{
    appInstance->mouseButtonCallback(button, action);
}

void glfwScrollCallback(GLFWwindow * /*window*/, double xoffset, double yoffset)
{
    appInstance->scrollCallback(xoffset, yoffset);
}

void glfwErrorCallback(int error, const char *description)
{
    std::cerr << "GLFW Error: " << error << ": " << description << std::endl;
}
} // namespace

Application *Application::createInstance(brayns::Brayns &brayns)
{
    delete appInstance;
    appInstance = new Application(brayns);
    return appInstance;
}

void Application::destroyInstance()
{
    delete appInstance;
    appInstance = nullptr;
}

Application::Application(brayns::Brayns &brayns)
    : m_brayns(brayns)
{
    registerKeyboardShortcuts();
    setupCameraManipulator(CameraMode::inspect, false);
    m_cameraManipulator->adjust(m_brayns.getEngine().getScene().getBounds());
}

void Application::reshape()
{
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(m_window, &width, &height);

    // Zero sized interop buffers are not allowed in OptiX.
    if (width == 0 || height == 0 || (m_width == width && m_height == height))
        return;

    auto &applicationParameters = m_brayns.getParametersManager().getApplicationParameters();

    m_width = width;
    m_height = height;
    applicationParameters.setWindowSize(brayns::Vector2ui{static_cast<uint32_t>(width), static_cast<uint32_t>(height)});
}

bool Application::init()
{
    if (!initGLFW())
        return false;
    initOpenGL();
    initImGUI();

    m_keyboardHandler.registerKeyboardShortcut(
        'z',
        "Switch between depth and color buffers",
        [&] {
            m_frameBufferMode =
                m_frameBufferMode == FrameBufferMode::DEPTH ? FrameBufferMode::COLOR : FrameBufferMode::DEPTH;
        });
    m_keyboardHandler.registerKeyboardShortcut('h', "Toggle help window", [&] { m_displayHelp = !m_displayHelp; });

    return true;
}

void Application::registerKeyboardShortcuts()
{
    auto &manager = m_brayns.getParametersManager();
    auto &renderParams = manager.getRenderingParameters();
    auto &animationParams = manager.getAnimationParameters();
    auto &applicationParams = manager.getApplicationParameters();
    auto &volumeParams = manager.getVolumeParameters();
    auto &engine = m_brayns.getEngine();
    auto &scene = engine.getScene();
    auto &renderer = engine.getRenderer();
    auto &camera = engine.getCamera();

    m_keyboardHandler.registerKeyboardShortcut(
        '0',
        "Black background",
        [&] {
            renderParams.setBackgroundColor({0.f, 0.f, 0.f});
        });
    m_keyboardHandler.registerKeyboardShortcut(
        '1',
        "Gray background",
        [&] {
            renderParams.setBackgroundColor({0.5f, 0.5f, 0.5f});
        });
    m_keyboardHandler.registerKeyboardShortcut(
        '2',
        "White background",
        [&] {
            renderParams.setBackgroundColor({1.f, 1.f, 1.f});
        });
    m_keyboardHandler.registerKeyboardShortcut(
        '3',
        "Set gradient materials",
        [&] { scene.setMaterialsColorMap(brayns::MaterialsColorMap::gradient); });
    m_keyboardHandler.registerKeyboardShortcut(
        '4',
        "Set random materials",
        [&] { scene.setMaterialsColorMap(brayns::MaterialsColorMap::random); });
    m_keyboardHandler.registerKeyboardShortcut(
        '5',
        "Scientific visualization renderer",
        [&] { renderParams.setCurrentRenderer("scivis"); });
    m_keyboardHandler.registerKeyboardShortcut(
        '6',
        "Default renderer",
        [&] { renderParams.setCurrentRenderer("basic"); });
    m_keyboardHandler.registerKeyboardShortcut(
        '7',
        "Basic simulation renderer",
        [&] { renderParams.setCurrentRenderer("basic_simulation"); });
    m_keyboardHandler.registerKeyboardShortcut(
        '8',
        "Advanced Simulation renderer",
        [&] { renderParams.setCurrentRenderer("advanced_simulation"); });
    m_keyboardHandler.registerKeyboardShortcut(
        '9',
        "Proximity renderer",
        [&] { renderParams.setCurrentRenderer("proximity"); });
    m_keyboardHandler.registerKeyboardShortcut(
        'e',
        "Enable eletron shading",
        [&]
        {
            renderer.updateProperty("shadingEnabled", false);
            renderer.updateProperty("electronShadingEnabled", true);
        });
    m_keyboardHandler.registerKeyboardShortcut(
        'f',
        "Enable fly mode",
        [this]() { setupCameraManipulator(CameraMode::flying); });
    m_keyboardHandler.registerKeyboardShortcut(
        'i',
        "Enable inspect mode",
        [this]() { setupCameraManipulator(CameraMode::inspect); });
    m_keyboardHandler.registerKeyboardShortcut(
        'o',
        "Decrease ambient occlusion strength",
        [&]
        {
            if (!renderer.hasProperty("aoWeight"))
                return;

            auto aoStrength = renderer.getProperty<double>("aoWeight");
            aoStrength -= 0.1;
            if (aoStrength < 0.)
                aoStrength = 0.;
            renderer.updateProperty("aoWeight", aoStrength);
        });
    m_keyboardHandler.registerKeyboardShortcut(
        'O',
        "Increase ambient occlusion strength",
        [&]
        {
            if (!renderer.hasProperty("aoWeight"))
                return;

            auto aoStrength = renderer.getProperty<double>("aoWeight");
            aoStrength += 0.1;
            if (aoStrength > 1.)
                aoStrength = 1.;
            renderer.updateProperty("aoWeight", aoStrength);
        });
    m_keyboardHandler.registerKeyboardShortcut(
        'p',
        "Enable diffuse shading",
        [&]
        {
            renderer.updateProperty("shadingEnabled", true);
            renderer.updateProperty("electronShadingEnabled", false);
        });
    m_keyboardHandler.registerKeyboardShortcut(
        'P',
        "Disable shading",
        [&]
        {
            renderer.updateProperty("shadingEnabled", false);
            renderer.updateProperty("electronShadingEnabled", false);
        });
    m_keyboardHandler.registerKeyboardShortcut('r', "Set animation frame to 0", [&] { animationParams.setFrame(0); });
    m_keyboardHandler.registerKeyboardShortcut(
        'u',
        "Enable/Disable shadows",
        [&]
        {
            if (!renderer.hasProperty("shadows"))
                return;
            renderer.updateProperty("shadows", renderer.getProperty<double>("shadows") == 0. ? 1. : 0.);
        });
    m_keyboardHandler.registerKeyboardShortcut(
        'U',
        "Enable/Disable soft shadows",
        [&]
        {
            if (!renderer.hasProperty("softShadows"))
                return;

            renderer.updateProperty("softShadows", renderer.getProperty<double>("softShadows") == 0. ? 1. : 0.);
        });
    m_keyboardHandler.registerKeyboardShortcut(
        't',
        "Multiply samples per ray by 2",
        [&] { volumeParams.setSamplingRate(volumeParams.getSamplingRate() * 2); });
    m_keyboardHandler.registerKeyboardShortcut(
        'T',
        "Divide samples per ray by 2",
        [&] { volumeParams.setSamplingRate(volumeParams.getSamplingRate() / 2); });
    m_keyboardHandler.registerKeyboardShortcut(
        'l',
        "Toggle load dynamic/static load balancer",
        [&] { applicationParams.setDynamicLoadBalancer(!applicationParams.getDynamicLoadBalancer()); });
    m_keyboardHandler.registerKeyboardShortcut(
        '{',
        "Decrease eye separation",
        [&]
        {
            if (!camera.hasProperty("interpupillaryDistance"))
                return;
            auto eyeSeparation = camera.getProperty<double>("interpupillaryDistance");
            eyeSeparation -= 0.01;
            camera.updateProperty("interpupillaryDistance", eyeSeparation);
            brayns::Log::info("Eye separation: {}.", eyeSeparation);
        });
    m_keyboardHandler.registerKeyboardShortcut(
        '}',
        "Increase eye separation",
        [&]
        {
            if (!camera.hasProperty("interpupillaryDistance"))
                return;
            auto eyeSeparation = camera.getProperty<double>("interpupillaryDistance");
            eyeSeparation += 0.01;
            camera.updateProperty("interpupillaryDistance", eyeSeparation);
            brayns::Log::info("Eye separation: {}.", eyeSeparation);
        });
    m_keyboardHandler.registerKeyboardShortcut(
        '<',
        "Decrease field of view",
        [&]
        {
            if (!camera.hasProperty("fovy"))
                return;
            auto fovy = camera.getProperty<double>("fovy");
            fovy -= 1.0;
            camera.updateProperty("fovy", fovy);
            brayns::Log::info("Field of view: {}.", fovy);
        });
    m_keyboardHandler.registerKeyboardShortcut(
        '>',
        "Increase field of view",
        [&]
        {
            if (!camera.hasProperty("fovy"))
                return;
            auto fovy = camera.getProperty<double>("fovy");
            fovy += 1.0;
            camera.updateProperty("fovy", fovy);
            brayns::Log::info("Field of view: {}.", fovy);
        });
    m_keyboardHandler.registerKeyboardShortcut(' ', "Camera reset to initial state", [&] { camera.reset(); });
    m_keyboardHandler.registerKeyboardShortcut(
        '+',
        "Increase motion speed",
        [&] { m_cameraManipulator->updateMotionSpeed(DEFAULT_MOTION_ACCELERATION); });
    m_keyboardHandler.registerKeyboardShortcut(
        '-',
        "Decrease motion speed",
        [&] { m_cameraManipulator->updateMotionSpeed(1.f / DEFAULT_MOTION_ACCELERATION); });
    m_keyboardHandler.registerKeyboardShortcut(
        'c',
        "Display current camera information",
        [&] { brayns::Log::info("{}, {}", camera.getPosition(), camera.getOrientation()); });
    m_keyboardHandler.registerKeyboardShortcut(
        'b',
        "Toggle benchmarking",
        [&] { applicationParams.setBenchmarking(!applicationParams.isBenchmarking()); });
}

void Application::setupCameraManipulator(CameraMode mode, bool adjust)
{
    m_cameraManipulator.reset();

    auto &engine = m_brayns.getEngine();
    auto &camera = engine.getCamera();
    auto &scene = engine.getScene();

    switch (mode)
    {
    case CameraMode::flying:
        m_cameraManipulator.reset(new brayns::FlyingModeManipulator(camera, m_keyboardHandler));
        break;
    case CameraMode::inspect:
        m_cameraManipulator.reset(new brayns::InspectCenterManipulator(camera, m_keyboardHandler));
        break;
    };

    if (adjust)
        m_cameraManipulator->adjust(scene.getBounds());
}

bool Application::initGLFW()
{
    const auto ws = getWindowSize();

    const int windowWidth = ws.x;
    const int windowHeight = ws.y;

    glfwSetErrorCallback(glfwErrorCallback);

    if (!glfwInit())
    {
        glfwErrorCallback(1, "GLFW failed to initialize.");
        return false;
    }

    const std::string windowTitle = "Brayns Viewer";

    m_window = glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(), NULL, NULL);
    if (!m_window)
    {
        glfwErrorCallback(2, "glfwCreateWindow() failed.");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);

// https://stackoverflow.com/a/11213354
#ifndef __APPLE__
    if (glewInit() != GL_NO_ERROR)
    {
        glfwErrorCallback(3, "GLEW failed to initialize.");
        glfwTerminate();
        return false;
    }
#endif

    glfwSetKeyCallback(m_window, glfwKeyCallback);
    glfwSetCursorPosCallback(m_window, glfwCursorCallback);
    glfwSetMouseButtonCallback(m_window, glfwMouseButtonCallback);
    glfwSetScrollCallback(m_window, glfwScrollCallback);

    return true;
}

void Application::initImGUI()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // Do not create the imgui.ini file
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = nullptr;

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(m_window, false);
    ImGui_ImplOpenGL2_Init();
}

void Application::run()
{
    // Main loop
    while (!glfwWindowShouldClose(m_window))
    {
        m_timer.stop();
        m_timer.start();

        if (m_exit)
        {
            m_brayns.getEngine().setKeepRunning(false);
            break;
        }

        toggleFullscreen();
        reshape();

        m_brayns.getActionInterface()->processRequests();
        render();
        guiNewFrame();
        guiRender();

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }

    for (size_t i = 0; i < MAX_FRAMEBUFFERS; i++)
        glDeleteTextures(1, &m_fbTextures[i]);

    guiShutdown();

    glfwDestroyWindow(m_window);

    glfwTerminate();
}

void Application::toggleFullscreen()
{
    if (!m_toggleFullScreen)
        return;

    m_toggleFullScreen = false;
    m_fullScreen = !m_fullScreen;

    if (m_fullScreen)
    {
        // backup window position and window size
        glfwGetWindowPos(m_window, &m_windowPos[0], &m_windowPos[1]);
        glfwGetWindowSize(m_window, &m_windowSizePrev[0], &m_windowSizePrev[1]);

        auto monitor = glfwGetPrimaryMonitor();

        // get reolution of monitor
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);

        // switch to full screen
        glfwSetWindowMonitor(m_window, monitor, 0, 0, mode->width, mode->height, 0);
    }
    else
    {
        // restore last window size and position
        glfwSetWindowMonitor(
            m_window,
            nullptr,
            m_windowPos[0],
            m_windowPos[1],
            m_windowSizePrev[0],
            m_windowSizePrev[1],
            0);
    }
}

brayns::Vector2ui Application::getWindowSize() const
{
    brayns::Vector2ui newWindowSize;
    for (auto frameBuffer : m_brayns.getEngine().getFrameBuffers())
    {
        newWindowSize.x += frameBuffer->getFrameSize().x;
        newWindowSize.y = std::max(newWindowSize.y, frameBuffer->getFrameSize().y);
    }
    return newWindowSize;
}

void Application::guiNewFrame()
{
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    const bool displayFPS = m_brayns.getParametersManager().getApplicationParameters().isBenchmarking();
    if (displayFPS)
    {
        ImGui::Begin("Benchmark");
        ImGui::Text("Display fps: %d", static_cast<int>(m_timer.perSecond()));
        ImGui::Text("Render fps: %d", static_cast<int>(m_brayns.getEngine().getStatistics().getFPS()));
        ImGui::End();
    }

    if (m_displayHelp)
    {
        ImGui::Begin("Keyboard shortcuts");
        for (const auto &line : m_keyboardHandler.help())
            ImGui::Text("%s", line.c_str());
        ImGui::End();
    }
}

void Application::guiRender()
{
    ImGui::Render();
    glViewport(0, 0, m_width, m_height);
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

void Application::guiShutdown()
{
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Application::initOpenGL()
{
    m_fbTextures.resize(MAX_FRAMEBUFFERS, 0);

    for (size_t i = 0; i < MAX_FRAMEBUFFERS; i++)
    {
        glGenTextures(1, &m_fbTextures[i]);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_fbTextures[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Application::render()
{
    // Launch a render if not running
    if (!m_renderFuture.valid())
    {
        m_renderFuture = std::async(
            std::launch::async,
            [&]
            {
                // Process mouse and keyboard events
                {
                    std::unique_lock<std::mutex> lock(m_actionLock);

                    for (const auto &action : m_actions)
                    {
                        switch (action.type)
                        {
                        case EventType::Keyboard:
                            handleKey(action);
                            break;
                        case EventType::Cursor:
                            handleCursor(action);
                            break;
                        case EventType::MouseButton:
                            handleMouseButton(action);
                            break;
                        case EventType::Scroll:
                            handleScroll(action);
                            break;
                        }
                    }

                    m_actions.clear();
                }

                m_brayns.commit();
                m_brayns.render();
            });
    }

    const bool renderDone = m_renderFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready;

    // "Clear" future when done to trigger new render
    if (renderDone)
        m_renderFuture.get();

    glViewport(0, 0, m_width, m_height);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    size_t offset = 0;
    size_t bufferIdx = 0;
    for (auto frameBuffer : m_brayns.getEngine().getFrameBuffers())
    {
        GLenum format = GL_RGBA;
        switch (frameBuffer->getFrameBufferFormat())
        {
        case brayns::PixelFormat::RGB_I8:
            format = GL_RGB;
            break;
        default:
            format = GL_RGBA;
        }

        const auto &frameSize = frameBuffer->getFrameSize();

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, frameSize.x, 0.0f, frameSize.y, -1.0f, 1.0f);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glViewport(offset, 0, frameSize.x, frameSize.y);

        glBindTexture(GL_TEXTURE_2D, m_fbTextures[bufferIdx]);

        if (renderDone)
        {
            frameBuffer->map();

            GLenum type = GL_FLOAT;
            const GLvoid *buffer = 0;
            switch (m_frameBufferMode)
            {
            case FrameBufferMode::COLOR:
                type = GL_UNSIGNED_BYTE;
                buffer = frameBuffer->getColorBuffer();
                break;
            case FrameBufferMode::DEPTH:
                format = GL_LUMINANCE;
                buffer = frameBuffer->getDepthBuffer();
                break;
            default:
                glClearColor(0.f, 0.f, 0.f, 1.f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }

            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                format,
                frameBuffer->getSize().x,
                frameBuffer->getSize().y,
                0,
                format,
                type,
                buffer);
            frameBuffer->unmap();
        }

        glBegin(GL_QUADS);
        glTexCoord2f(0.f, 0.f);
        glVertex3f(0, 0, 0);
        glTexCoord2f(0.f, 1.f);
        glVertex3f(0, frameSize.y, 0);
        glTexCoord2f(1.f, 1.f);
        glVertex3f(frameSize.x, frameSize.y, 0);
        glTexCoord2f(1.f, 0.f);
        glVertex3f(frameSize.x, 0, 0);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, 0);

        offset += frameSize.x;
        bufferIdx += 1;
    }

    if (renderDone)
        m_brayns.postRender();
}

void Application::keyCallback(int key, int action)
{
    // Handle exit here so we don't have to wait for Brayns keyboard callback
    m_exit = m_exit || (action == GLFW_PRESS && (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE));
    if (m_exit)
        return;

    InputEvent event;
    event.type = EventType::Keyboard;
    event.action = action;
    event.button = key;

    std::unique_lock<std::mutex> lock(m_actionLock);
    m_actions.push_back(event);
}

void Application::cursorCallback(double xpos, double ypos)
{
    if (ImGui::GetIO().WantCaptureMouse)
        return;

    InputEvent event;
    event.type = EventType::Cursor;
    event.xpos = xpos;
    event.ypos = ypos;

    std::unique_lock<std::mutex> lock(m_actionLock);
    m_actions.push_back(event);
}

void Application::mouseButtonCallback(int button, int action)
{
    InputEvent event;
    event.type = EventType::MouseButton;
    event.button = button;
    event.action = action;

    std::unique_lock<std::mutex> lock(m_actionLock);
    m_actions.push_back(event);
}

void Application::scrollCallback(double xoffset, double yoffset)
{
    InputEvent event;
    event.type = EventType::Scroll;
    event.xoffset = xoffset;
    event.yoffset = yoffset;

    std::unique_lock<std::mutex> lock(m_actionLock);
    m_actions.push_back(event);
}

void Application::handleKey(InputEvent action)
{
    const auto toKeyChar = [&](int key, bool shiftDown)
    {
        constexpr auto KEY_A = 32; // 'a'
        constexpr auto KEY_A_CAPS = 65; // 'A'
        constexpr auto KEY_Z_CAPS = 90; // 'Z'

        // Convert uppercase to lowercase if shift is not pressed
        if (!shiftDown && key >= KEY_A_CAPS && key <= KEY_Z_CAPS)
            key = key + KEY_A;

        return static_cast<char>(key);
    };

    if (action.action == GLFW_PRESS)
    {
        switch (action.button)
        {
        case GLFW_KEY_LEFT:
            m_keyboardHandler.handle(brayns::SpecialKey::LEFT);
            break;
        case GLFW_KEY_RIGHT:
            m_keyboardHandler.handle(brayns::SpecialKey::RIGHT);
            break;
        case GLFW_KEY_UP:
            m_keyboardHandler.handle(brayns::SpecialKey::UP);
            break;
        case GLFW_KEY_DOWN:
            m_keyboardHandler.handle(brayns::SpecialKey::DOWN);
            break;
        case GLFW_KEY_F11:
            m_toggleFullScreen = true;
            break;
        case GLFW_MOD_ALT:
            m_altKeyDown = true;
            break;
        case GLFW_MOD_CONTROL:
            m_ctrlKeyDown = true;
            break;
        case GLFW_KEY_RIGHT_SHIFT:
            m_rightShiftKeyDown = true;
            break;
        case GLFW_KEY_LEFT_SHIFT:
            m_leftShiftKeyDown = true;
            break;
        default:
            const char keyChar = toKeyChar(action.button, m_leftShiftKeyDown || m_rightShiftKeyDown);
            m_keyboardHandler.handleKeyboardShortcut(keyChar);
        }
    }

    if (action.action == GLFW_RELEASE)
    {
        switch (action.button)
        {
        case GLFW_MOD_ALT:
            m_altKeyDown = false;
            break;
        case GLFW_MOD_CONTROL:
            m_ctrlKeyDown = false;
            break;
        case GLFW_KEY_RIGHT_SHIFT:
            m_rightShiftKeyDown = false;
            break;
        case GLFW_KEY_LEFT_SHIFT:
            m_leftShiftKeyDown = false;
            break;
        default:
            break;
        }
    }
}

void Application::handleCursor(InputEvent action)
{
    const brayns::Vector2d pos{action.xpos, action.ypos};

    if (m_rightMouseButtonDown || (m_leftMouseButtonDown && m_altKeyDown))
    {
        m_cameraManipulator->dragRight(pos, m_lastMousePos);
    }
    else if (m_middleMouseButtonDown || (m_leftMouseButtonDown && m_ctrlKeyDown))
    {
        m_cameraManipulator->dragMiddle(pos, m_lastMousePos);
    }
    else if (m_leftMouseButtonDown)
    {
        m_cameraManipulator->dragLeft(pos, m_lastMousePos);
    }

    m_lastMousePos = pos;
}

void Application::handleMouseButton(InputEvent action)
{
    if (action.button == GLFW_MOUSE_BUTTON_LEFT)
        m_leftMouseButtonDown = action.action == GLFW_PRESS;

    if (action.button == GLFW_MOUSE_BUTTON_RIGHT)
        m_rightMouseButtonDown = action.action == GLFW_PRESS;

    if (action.button == GLFW_MOUSE_BUTTON_MIDDLE)
        m_middleMouseButtonDown = action.action == GLFW_PRESS;

    if (action.button == GLFW_MOUSE_BUTTON_LEFT && action.action == GLFW_PRESS
        && (m_leftShiftKeyDown || m_rightShiftKeyDown))
    {
        const auto &windowSize = m_brayns.getParametersManager().getApplicationParameters().getWindowSize();
        const auto result = m_brayns.getEngine().getRenderer().pick(
            {m_lastMousePos.x / float(windowSize.x), 1.f - m_lastMousePos.y / float(windowSize.y)});
        if (result.hit)
        {
            // updates position based on new target and current rotation
            m_cameraManipulator->rotate(result.pos, 0, 0, brayns::AbstractManipulator::AxisMode::localY);
        }
    }
}

void Application::handleScroll(InputEvent action)
{
    const auto delta = action.yoffset > 0.0 ? 1 : -1;
    m_cameraManipulator->wheel(m_lastMousePos, delta);
}
