#ifndef APP_HPP_
#define APP_HPP_
#include <vector>
#include <memory>

class App {
public:
	App();

	void SetResources();
	void PerFrameUpdate();
	void PhysicsUpdate();

private:
	App& AddModels(std::vector<std::shared_ptr<class Model>>&& models) noexcept;

private:
	std::vector<std::shared_ptr<class Model>> m_models;
};
#endif
