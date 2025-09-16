#include "gui/window.h"

namespace zaphod
{
Window::Window(int width, int height, const char* title) {
	// Initialize GLFW window
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);	   // Disable window resizing for now

	m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
}

Window::~Window() {}

bool Window::shouldClose() const
{
	return glfwWindowShouldClose(m_window);
}

void Window::pollEvents() const {
	glfwPollEvents();
}
}	 // namespace zaphod