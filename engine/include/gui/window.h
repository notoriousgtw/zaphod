#pragma once

#include "core/glfw_common.h"

namespace zaphod
{

//!  Window class for creating and managing application windows.
/*!
  Handles window creation, event polling, and provides access to the underlying GLFW window.
*/
class Window
{
  public:
	//! Constructor to create a window with specified width, height, and title.
	/*!
	 * Constructs a window using GLFW.
	 * @param width  - Width of the window in pixels.
	 * @param height - Height of the window in pixels.
	 * @param title  - Title of the window.
	 */
	Window(int width, int height, const char* title);
	~Window();

	// Non-copyable, non-movable
	Window(const Window&)			 = delete;
	Window& operator=(const Window&) = delete;
	Window(Window&&)				 = delete;
	Window& operator=(Window&&)		 = delete;

	//! Check if the window should close.
	/*!
		Checks if the window has been signaled to close.
		@return true if the window should close, false otherwise.
	*/
	bool shouldClose() const;

	//! Poll for and process window events.
	/*
		This function processes all pending events for the window.
	*/
	void pollEvents() const;
	//! Get the underlying GLFW window.
	/*! 
		Retrieves the pointer to the GLFWwindow instance.
		@return Pointer to the GLFWwindow instance.
	*/
	GLFWwindow* getGLFWwindow() const { return m_window; }

  private:
	GLFWwindow* m_window = nullptr;
};
}	 // namespace zaphod
