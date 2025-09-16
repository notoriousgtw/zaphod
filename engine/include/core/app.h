#pragma once

#include "gui/window.h"

#include <vector>
#include <memory>

namespace zaphod
{
class App
{
  public:
	App()		   = default;
	virtual ~App() = default;

	// Non-copyable, non-movable
	App(const App&)			   = delete;
	App& operator=(const App&) = delete;
	App(App&&)				   = delete;
	App& operator=(App&&)	   = delete;

	virtual bool initialize(int argc, char** argv);
	virtual int	 run();
	virtual void shutdown();

  protected:
	virtual bool onInitialize()			   = 0;
	virtual void onUpdate(float deltaTime) = 0;
	virtual void onRender()				   = 0;
	virtual void onShutdown()			   = 0;

  private:
	bool m_running	   = false;
	bool m_initialized = false;
	std::vector<std::unique_ptr<Window>> m_windows;
};
}	 // namespace zaphod
