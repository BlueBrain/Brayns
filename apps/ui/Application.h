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

struct GLFWwindow;

class Application
{
public:
    static Application* createInstance(brayns::Brayns& brayns);
    static void destroyInstance();

    bool init();
    void run();

    void keyCallback(int key, int action);
    void cursorCallback(double xpos, double ypos);
    void mouseButtonCallback(int button, int action);
    void scrollCallback(double xoffset, double yoffset);

private:
    Application(brayns::Brayns& brayns);
    ~Application() = default;

    /* The different types of frame buffer targets for rendering. */
    enum class FrameBufferMode
    {
        COLOR,
        DEPTH
    };

    bool initGLFW();
    void initOpenGL();
    void initImGUI();

    void reshape();
    void render();
    void guiNewFrame();
    void guiRender();
    void guiShutdown();

    void toggleFullscreen();
    brayns::Vector2ui getWindowSize() const;

    brayns::Brayns& m_brayns;
    GLFWwindow* m_window{nullptr};
    int m_width{0};
    int m_height{0};
    bool m_exit{false};
    bool m_displayHelp{false};
    bool m_fullScreen{false};
    bool m_toggleFullScreen{false};

    brayns::Vector2d m_lastMousePos{0.0, 0.0};

    bool m_leftMouseButtonDown{false};
    bool m_rightMouseButtonDown{false};
    bool m_middleMouseButtonDown{false};

    bool m_altKeyDown{false};
    bool m_ctrlKeyDown{false};
    bool m_leftShiftKeyDown{false};
    bool m_rightShiftKeyDown{false};

    // OpenGL variables:
    uint32_t m_fbTexture{0};

    FrameBufferMode m_frameBufferMode{FrameBufferMode::COLOR};

    brayns::Timer m_timer;

    std::array<int, 2> m_windowPos{{0, 0}};
    std::array<int, 2> m_windowSizePrev{{0, 0}};
};
