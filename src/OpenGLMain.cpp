#include <assert.h>
#include "MemoryProfiler.hpp"
#include "MainWindow.hpp"
#include "SceneRenderer.hpp"
#include "macro.hpp"

int main() {
	MemoryProfiler::make_memory_snapshot();
	{
		std::unique_ptr<MainWindow> main_window = std::make_unique<MainWindow>(800, 800, "OpenGLWindow");
		std::unique_ptr<SceneRenderer> scene_renderer = std::make_unique<SceneRenderer>(main_window.get());
		scene_renderer->render();
	}
	MemoryProfiler::make_memory_snapshot();
	assert(MemoryProfiler::dump() == MemoryProfiler::DumpResult::eNoMemoryLeaks);
	return 0;
}
