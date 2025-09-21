#include "core/app.h"

#include "core/glfw_common.h"
#include "core/logger.h"

#include <chrono>

namespace zaphod {
    bool App::initialize(int argc, char** argv) {
        if (m_initialized) return true;

        // Engine-level initialization (GLFW, Vulkan, etc.)
        // Initialize GLFW
        if (!glfwInit()) {
            return false;
		}

        m_initialized = onInitialize();
        return m_initialized;
    }

    int App::run() {
        if (!m_initialized) return -1;

        m_running = true;

        auto lastTime = std::chrono::high_resolution_clock::now();

        m_windows.emplace_back(std::make_unique<Window>(1280, 720, "Zaphod Engine"));

        auto logger = SimpleLoggerFactory().create();

        while (m_running) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
            lastTime = currentTime;

            // Handle window events, input, etc.
            for (auto& window : m_windows) {
                window->pollEvents();
			}

            onUpdate(deltaTime);
            onRender();

            // Check for window close, etc.
			for (auto& window : m_windows)
			{
				if (window->shouldClose())
				{
					m_running = false;
				}
			}
        }

        shutdown();
        return 0;
    }

    void App::shutdown() {
        if (!m_initialized) return;

        onShutdown();

        // Engine-level cleanup

        m_initialized = false;
        m_running = false;
    }
}