#include <App.hpp>
#include <IGraphicsEngine.hpp>
#include <InstanceManager.hpp>
#include <CRSColour.hpp>
#include <string>

App::App() {
	IModelContainer* modelCont = ModelContInst::GetRef();
	ITextureAtlas* texAtlas = TexAtlasInst::GetRef();

	texAtlas->SetTextureFormat(TextureFormat::UINT8);

	Ceres::Uint8_4 red = { 255u, 0u, 0u };

	texAtlas->AddColour("Fuchsia", Ceres::Colour::Fuchsia);
	texAtlas->AddColour("Cyan", Ceres::Colour::Cyan);
	texAtlas->AddColour("Red", red);
	texAtlas->AddColour("Green", Ceres::Colour::Green);
	texAtlas->AddColour("Blue", Ceres::Colour::Blue);

	std::unique_ptr<Triangle> triangle0 = std::make_unique<Triangle>();
	std::unique_ptr<Triangle1> triangle1 = std::make_unique<Triangle1>();

	triangle0->SetTextureIndex(0u);
	triangle1->SetTextureIndex(0u);

	m_triangleRefs.emplace_back(
		modelCont->AddModel(std::move(triangle0))
	);
	m_triangleRefs.emplace_back(
		modelCont->AddModel(std::move(triangle1))
	);

	for(auto& triangle : m_triangleRefs)
		RendererInst::GetRef()->SubmitModel(triangle);
}

void App::SetResources() {
	ITextureAtlas* texAtlas = TexAtlasInst::GetRef();
	auto fuchsia = texAtlas->GetPixelData("Fuchsia");
	auto cyan = texAtlas->GetPixelData("Cyan");
	std::uint32_t atlasWidth = texAtlas->GetWidth();
	std::uint32_t atlasHeight = texAtlas->GetHeight();

	m_triangleRefs[0]->SetTextureInfo(
		TextureData{
			fuchsia.uStart, fuchsia.uEnd, atlasWidth,
			fuchsia.vStart, fuchsia.vEnd, atlasHeight
		}
	);
	m_triangleRefs[1]->SetTextureInfo(
		TextureData{
			cyan.uStart, cyan.uEnd, atlasWidth,
			cyan.vStart, cyan.vEnd, atlasHeight
		}
	);
}

void App::Update() {
	IKeyboard* pKeyboardRef = IOInst::GetRef()->GetKeyboardByIndex();

	if (pKeyboardRef->AreKeysPressed(2, SKeyCodes::F, SKeyCodes::One)) {
		ITextureAtlas* texAtlas = TexAtlasInst::GetRef();
		auto fuchsia = texAtlas->GetPixelData("Fuchsia");
		std::uint32_t atlasWidth = texAtlas->GetWidth();
		std::uint32_t atlasHeight = texAtlas->GetHeight();

		m_triangleRefs[0]->SetTextureInfo(
			TextureData{
				fuchsia.uStart, fuchsia.uEnd, atlasWidth,
				fuchsia.vStart, fuchsia.vEnd, atlasHeight
			}
		);
	}
	else if(pKeyboardRef->AreKeysPressed(2, SKeyCodes::R, SKeyCodes::One)) {
		ITextureAtlas* texAtlas = TexAtlasInst::GetRef();
		auto red = texAtlas->GetPixelData("Red");
		std::uint32_t atlasWidth = texAtlas->GetWidth();
		std::uint32_t atlasHeight = texAtlas->GetHeight();

		m_triangleRefs[0]->SetTextureInfo(
			TextureData{
				red.uStart, red.uEnd, atlasWidth,
				red.vStart, red.vEnd, atlasHeight
			}
		);
	}

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
