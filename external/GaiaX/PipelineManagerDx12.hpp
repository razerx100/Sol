#ifndef __PIPELINE_MANAGER_DX12_HPP__
#define __PIPELINE_MANAGER_DX12_HPP__
#include <PipelineManager.hpp>

class PipelineManagerDx12 : public PipelineManager {
public:
	PipelineManagerDx12(void* windowHandle);

	void SubmitCommands() override;
	void Render() override;
	void Resize(std::uint32_t width, std::uint32_t height) override;
	SRect GetMonitorCoordinates() override;
};
#endif
