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

#pragma once

#include <brayns/Brayns.h>
#include <brayns/common/Timer.h>

#ifndef __APPLE__
#include <GL/glew.h>
#if defined(_WIN32)
#include <GL/wglew.h>
#endif
#endif

#include <GLFW/glfw3.h>

/* The different types of frame buffer targets for rendering. */
enum class FrameBufferMode
{
    COLOR,
    DEPTH
};

class Application
{
public:
    Application(brayns::Brayns& brayns, GLFWwindow* window, const int width,
                const int height);
    ~Application();

    void reshape(int width, int height);

    void render();
    void guiNewFrame();
    void guiRender();
    void guiShutdown();

    void keyCallback(int key, int action);
    void cursorCallback(double xpos, double ypos);
    void mouseButtonCallback(int button, int action);
    void scrollCallback(double xoffset, double yoffset);

    bool exitCalled() const { return m_exit; }
    bool isFullScreen() const { return m_fullScreen; }
    void timerBegin();
    void timerEnd();

private:
    void initOpenGL();
    void initImGUI();

    brayns::Brayns& m_brayns;
    GLFWwindow* m_window;
    int m_width;
    int m_height;
    bool m_exit{false};
    bool m_displayHelp{false};
    bool m_fullScreen{false};

    brayns::Vector2d m_lastMousePos{0.0, 0.0};

    bool m_leftMouseButtonDown{false};
    bool m_rightMouseButtonDown{false};
    bool m_middleMouseButtonDown{false};

    bool m_altKeyDown{false};
    bool m_ctrlKeyDown{false};
    bool m_leftShiftKeyDown{false};
    bool m_rightShiftKeyDown{false};

    // OpenGL variables:
    GLuint m_fbTexture;

    FrameBufferMode m_frameBufferMode{FrameBufferMode::COLOR};

    brayns::Timer m_timer;
};
