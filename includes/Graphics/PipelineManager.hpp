#ifndef __PIPELINE_MANAGER_HPP__
#define __PIPELINE_MANAGER_HPP__
#include <memory>

class PipelineManager {
public:
	virtual ~PipelineManager() = default;

	inline static void Init(std::unique_ptr<PipelineManager> instance) {
		s_instance = std::move(instance);
	}

	inline static PipelineManager* GetRef() noexcept {
		return s_instance.get();
	}

	virtual void SubmitCommands() = 0;
	virtual void Render() = 0;

private:
	static std::unique_ptr<PipelineManager> s_instance;
};
#endif
