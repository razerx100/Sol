#include <App.hpp>
#include <GraphicsEngine.hpp>
#include <InstanceManager.hpp>
#include <CRSColour.hpp>
#include <string>

App::App() {
	IModelContainer* modelCont = ModelContInst::GetRef();
	ITextureAtlas* texAtlas = TexAtlasInst::GetRef();

	texAtlas->AddColour("Red", Ceres::Colour::Red);
	texAtlas->AddColour("Cyan", Ceres::Colour::Cyan);

	m_triangleRefs.emplace_back(
		modelCont->AddModel(std::make_unique<Triangle>(Ceres::Colour::Cyan))
	);
	m_triangleRefs.emplace_back(
		modelCont->AddModel(std::make_unique<Triangle1>(Ceres::Colour::Fuchsia))
	);

	for(auto& triangle : m_triangleRefs)
		RendererInst::GetRef()->SubmitModel(triangle);
}

void App::Update() {
	IKeyboard* pKeyboardRef = IOInst::GetRef()->GetKeyboardByIndex();

	if (pKeyboardRef->AreKeysPressed(2, SKeyCodes::Alt, SKeyCodes::D))
		WindowInst::GetRef()->SetTitle("Alt + D");

	IMouse* pMouseRef = IOInst::GetRef()->GetMouseByIndex();

	if (pMouseRef->AreButtonsPressed(2, MouseButtons::X1, MouseButtons::Middle))
		WindowInst::GetRef()->SetTitle("Left + Right");

	IGamepad* pGamepadRef = IOInst::GetRef()->GetGamepadByIndex();

	if (auto e = pGamepadRef->Read(); e.GetType() == IGamepad::Event::Type::LeftThumbStick)
		WindowInst::GetRef()->SetTitle((
			"M: " + std::to_string(e.GetMagnitude()) +
			" X: " + std::to_string(e.GetASData().xDirection) +
			" Y: " + std::to_string(e.GetASData().yDirection)
			).c_str());

	if (pGamepadRef->AreButtonsPressed(2, XBoxButton::START, XBoxButton::X))
		WindowInst::GetRef()->SetTitle("Start + A");
}
