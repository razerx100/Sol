#include <SolHelperFunctions.hpp>
#include <Sol.hpp>

RenderEngineType GetRenderEngineTypeFromConfig() noexcept {
	RenderEngineType renderEngineType = RenderEngineType::IndirectDraw;
	if (auto renderEngine = Sol::configManager->GetRenderEngine();
		renderEngine == "IndirectDraw")
		renderEngineType = RenderEngineType::IndirectDraw;
	else if (renderEngine == "IndividualDraw")
		renderEngineType = RenderEngineType::IndividualDraw;
	else if (renderEngine == "MeshDraw")
		renderEngineType = RenderEngineType::MeshDraw;

	return renderEngineType;
}
