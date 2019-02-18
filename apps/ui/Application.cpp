/* Copyright (c) 2019, EPFL/Blue Brain Project
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
#include <brayns/common/input/KeyboardHandler.h>
#include <brayns/engine/Camera.h>
#include <brayns/engine/Engine.h>
#include <brayns/engine/FrameBuffer.h>
#include <brayns/engine/Renderer.h>
#include <brayns/engine/Scene.h>
#include <brayns/manipulators/AbstractManipulator.h>
#include <brayns/parameters/ParametersManager.h>

#ifndef __APPLE__
#include <GL/glew.h>
#if defined(_WIN32)
#include <GL/wglew.h>
#endif
#endif

#include <GLFW/glfw3.h>

#include <imgui/imconfig.h>
#include <imgui/imgui.h>
// NOTE: include samples after imconfig.h and imgui.h
#include <imgui/examples/imgui_impl_glfw.h>
#include <imgui/examples/imgui_impl_opengl3.h>

#include <algorithm>
#include <cassert>

Application* appInstance = nullptr;

static void glfwKeyCallback(GLFWwindow* /*window*/, int key, int /*scancode*/,
                            int action, int /*mods*/)
{
    appInstance->keyCallback(key, action);
}

static void glfwCursorCallback(GLFWwindow* /*window*/, double xpos, double ypos)
{
    appInstance->cursorCallback(xpos, ypos);
}

static void glfwMouseButtonCallback(GLFWwindow* /*window*/, int button,
                                    int action, int /*mods*/)
{
    appInstance->mouseButtonCallback(button, action);
}

static void glfwScrollCallback(GLFWwindow* /*window*/, double xoffset,
                               double yoffset)
{
    appInstance->scrollCallback(xoffset, yoffset);
}

static void glfwErrorCallback(int error, const char* description)
{
    std::cerr << "GLFW Error: " << error << ": " << description << std::endl;
}

Application* Application::createInstance(brayns::Brayns& brayns)
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

Application::Application(brayns::Brayns& brayns)
    : m_brayns(brayns)
{
}

void Application::reshape()
{
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(m_window, &width, &height);

    // Zero sized interop buffers are not allowed in OptiX.
    if (width == 0 || height == 0 || (m_width == width && m_height == height))
        return;

    auto& applicationParameters =
        m_brayns.getParametersManager().getApplicationParameters();

    // In case of 3D stereo vision, make sure the width is even
    if (applicationParameters.isStereo())
    {
        if (width % 2 != 0)
            width = (width - 1) / 2;
        else
            width /= 2;
    }

    m_width = width;
    m_height = height;
    applicationParameters.setWindowSize(
        brayns::Vector2ui{static_cast<uint32_t>(width),
                          static_cast<uint32_t>(height)});
}

bool Application::init()
{
    if (!initGLFW())
        return false;
    initOpenGL();
    initImGUI();

    auto& keyHandler = m_brayns.getKeyboardHandler();
    keyHandler.registerKeyboardShortcut(
        'z', "Switch between depth and color buffers", [&]() {
            m_frameBufferMode = m_frameBufferMode == FrameBufferMode::DEPTH
                                    ? FrameBufferMode::COLOR
                                    : FrameBufferMode::DEPTH;
        });
    keyHandler.registerKeyboardShortcut('h', "Toggle help window", [&]() {
        m_displayHelp = !m_displayHelp;
    });

    return true;
}

bool Application::initGLFW()
{
    const auto ws = m_brayns.getParametersManager()
                        .getApplicationParameters()
                        .getWindowSize();
    const int windowWidth = ws.x;
    const int windowHeight = ws.y;

    glfwSetErrorCallback(glfwErrorCallback);

    if (!glfwInit())
    {
        glfwErrorCallback(1, "GLFW failed to initialize.");
        return false;
    }

    const auto engineName =
        m_brayns.getParametersManager().getApplicationParameters().getEngine();
    const std::string windowTitle = "Brayns Viewer [" + engineName + "] ";

    m_window = glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(),
                                NULL, NULL);
    if (!m_window)
    {
        glfwErrorCallback(2, "glfwCreateWindow() failed.");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);

    if (glewInit() != GL_NO_ERROR)
    {
        glfwErrorCallback(3, "GLEW failed to initialize.");
        glfwTerminate();
        return false;
    }

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

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(m_window, false);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void Application::run()
{
    // Main loop
    while (!glfwWindowShouldClose(m_window))
    {
        m_timer.stop();
        m_timer.start();

        if (m_exit)
            break;

        toggleFullscreen();
        reshape();

        render();
        guiNewFrame();
        guiRender();

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }

    glDeleteTextures(1, &m_fbTexture);
    m_fbTexture = 0;

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
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        // switch to full screen
        glfwSetWindowMonitor(m_window, monitor, 0, 0, mode->width, mode->height,
                             0);
    }
    else
    {
        // restore last window size and position
        glfwSetWindowMonitor(m_window, nullptr, m_windowPos[0], m_windowPos[1],
                             m_windowSizePrev[0], m_windowSizePrev[1], 0);
    }
}

void Application::guiNewFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    const bool displayFPS = m_brayns.getParametersManager()
                                .getApplicationParameters()
                                .isBenchmarking();
    if (displayFPS)
    {
        ImGui::Begin("Benchmark");
        ImGui::Text("Display fps: %d", static_cast<int>(m_timer.perSecond()));
        ImGui::Text("Render fps: %d",
                    static_cast<int>(
                        m_brayns.getEngine().getStatistics().getFPS()));
        ImGui::End();
    }

    if (m_displayHelp)
    {
        ImGui::Begin("Keyboard shortcuts");
        for (const auto& line : m_brayns.getKeyboardHandler().help())
            ImGui::Text("%s", line.c_str());
        ImGui::End();
    }
}

void Application::guiRender()
{
    ImGui::Render();
    glViewport(0, 0, m_width, m_height);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::guiShutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Application::initOpenGL()
{
    glGenTextures(1, &m_fbTexture);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_fbTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Application::render()
{
    glViewport(0, 0, m_width, m_height);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_brayns.commitAndRender();

    size_t offset = 0;
    for (auto frameBuffer : m_brayns.getEngine().getFrameBuffers())
    {
        GLenum format = GL_RGBA;
        switch (frameBuffer->getFrameBufferFormat())
        {
        case brayns::FrameBufferFormat::bgra_i8:
            format = GL_BGRA;
            break;
        case brayns::FrameBufferFormat::rgb_i8:
            format = GL_RGB;
            break;
        default:
            format = GL_RGBA;
        }

        const auto& frameSize = frameBuffer->getFrameSize();

        frameBuffer->map();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, frameSize.x, 0.0f, frameSize.y, -1.0f, 1.0f);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glViewport(offset, 0, frameSize.x, frameSize.y);

        GLenum type = GL_FLOAT;
        const GLvoid* buffer = 0;
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

        if (buffer)
        {
            glBindTexture(GL_TEXTURE_2D, m_fbTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, format, frameBuffer->getSize().x,
                         frameBuffer->getSize().y, 0, format, type, buffer);

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
        }

        frameBuffer->unmap();
        offset += frameSize.x;
    }
}

void Application::keyCallback(int key, int action)
{
    const auto toKeyChar = [&](int k, bool shiftDown) {
        if (!shiftDown && (k >= 65) && (k <= 90))
            k = k + 32;
        return static_cast<char>(k);
    };

    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_Q:
        case GLFW_KEY_ESCAPE:
            m_exit = true;
            break;
        case GLFW_KEY_LEFT:
            m_brayns.getKeyboardHandler().handle(brayns::SpecialKey::LEFT);
            break;
        case GLFW_KEY_RIGHT:
            m_brayns.getKeyboardHandler().handle(brayns::SpecialKey::RIGHT);
            break;
        case GLFW_KEY_UP:
            m_brayns.getKeyboardHandler().handle(brayns::SpecialKey::UP);
            break;
        case GLFW_KEY_DOWN:
            m_brayns.getKeyboardHandler().handle(brayns::SpecialKey::DOWN);
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
            const char keyChar =
                toKeyChar(key, m_leftShiftKeyDown || m_rightShiftKeyDown);
            m_brayns.getKeyboardHandler().handleKeyboardShortcut(keyChar);
        }

        m_brayns.getEngine().commit();
    }

    if (action == GLFW_RELEASE)
    {
        switch (key)
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

void Application::cursorCallback(double xpos, double ypos)
{
    if (ImGui::GetIO().WantCaptureMouse)
        return;

    const brayns::Vector2d pos{xpos, ypos};

    auto& manipulator = m_brayns.getCameraManipulator();

    if (m_rightMouseButtonDown || (m_leftMouseButtonDown && m_altKeyDown))
    {
        manipulator.dragRight(pos, m_lastMousePos);
    }
    else if (m_middleMouseButtonDown ||
             (m_leftMouseButtonDown && m_ctrlKeyDown))
    {
        manipulator.dragMiddle(pos, m_lastMousePos);
    }
    else if (m_leftMouseButtonDown)
    {
        manipulator.dragLeft(pos, m_lastMousePos);
    }

    m_lastMousePos = pos;
}

void Application::mouseButtonCallback(int button, int action)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
        m_leftMouseButtonDown = action == GLFW_PRESS;

    if (button == GLFW_MOUSE_BUTTON_RIGHT)
        m_rightMouseButtonDown = action == GLFW_PRESS;

    if (button == GLFW_MOUSE_BUTTON_MIDDLE)
        m_middleMouseButtonDown = action == GLFW_PRESS;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS &&
        (m_leftShiftKeyDown || m_rightShiftKeyDown))
    {
        const auto& windowSize = m_brayns.getParametersManager()
                                     .getApplicationParameters()
                                     .getWindowSize();
        const auto result = m_brayns.getEngine().getRenderer().pick(
            {m_lastMousePos.x / float(windowSize.x),
             1.f - m_lastMousePos.y / float(windowSize.y)});
        if (result.hit)
        {
            // updates position based on new target and current rotation
            m_brayns.getCameraManipulator().rotate(
                result.pos, 0, 0,
                brayns::AbstractManipulator::AxisMode::localY);
        }
    }
}

void Application::scrollCallback(double /*xoffset*/, double yoffset)
{
    const auto delta = yoffset > 0.0 ? 1 : -1;
    m_brayns.getCameraManipulator().wheel(m_lastMousePos, delta);
}
