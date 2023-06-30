#include <assert.h>
#include "./core/MemoryProfiler.hpp"
#include "./core/MainWindow.hpp"
#include "./core/SceneRenderer.hpp"
#include "./core/macro.hpp"

int main() {
	MemoryProfiler::make_memory_snapshot();
	{
		std::unique_ptr<SceneRenderer> scene_renderer = std::make_unique<SceneRenderer>();
		scene_renderer->render();
	}
	MemoryProfiler::make_memory_snapshot();
	assert(MemoryProfiler::dump() == MemoryProfiler::DumpResult::eNoMemoryLeaks);
	return 0;
}
