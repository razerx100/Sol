#include <PipelineManager.hpp>

void PipelineManager::Init(std::unique_ptr<PipelineManager> instance) {
	s_instance = std::move(instance);
}

PipelineManager* PipelineManager::GetRef() noexcept {
	return s_instance.get();
}
