/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <future>
#include <memory>
#include <mutex>

#include "../input/KeyboardHandler.h"

#include "../manipulators/AbstractManipulator.h"

struct GLFWwindow;

enum class EventType
{
    Keyboard,
    Cursor,
    MouseButton,
    Scroll
};

/**
 * The different modes for moving the camera.
 */
enum class CameraMode
{
    flying,
    inspect
};

struct InputEvent
{
    EventType type;

    // Keyboard and mouse buttons
    int button{0};
    int action{0};

    // Cursor movement
    double xpos{0};
    double ypos{0};

    // Scrolling
    double xoffset{0};
    double yoffset{0};
};

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

    void registerKeyboardShortcuts();
    void setupCameraManipulator(CameraMode mode, bool adjust = true);

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

    void handleKey(InputEvent action);
    void handleCursor(InputEvent action);
    void handleMouseButton(InputEvent action);
    void handleScroll(InputEvent action);

    brayns::Brayns& m_brayns;
    brayns::KeyboardHandler m_keyboardHandler;
    std::unique_ptr<brayns::AbstractManipulator> m_cameraManipulator;
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

    // OpenGL textures:
    std::vector<uint32_t> m_fbTextures;

    FrameBufferMode m_frameBufferMode{FrameBufferMode::COLOR};

    brayns::Timer m_timer;

    brayns::Vector2i m_windowPos{0, 0};
    brayns::Vector2i m_windowSizePrev{0, 0};

    std::future<void> m_renderFuture;
    std::mutex m_actionLock;

    std::vector<InputEvent> m_actions;
};
