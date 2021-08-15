#ifndef __PIPELINE_MANAGER_HPP__
#define __PIPELINE_MANAGER_HPP__

class PipelineManager {
public:
	virtual ~PipelineManager() = default;

	virtual void SubmitCommands() = 0;
	virtual void Render() = 0;
};
#endif
