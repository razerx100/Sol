#ifndef GLTF_OBJECT_HPP_
#define GLTF_OBJECT_HPP_
#include <tiny_gltf.h>

namespace Sol
{
class GLTFObject
{
public:
	GLTFObject() : m_gltf{} {}

	void Set(tinygltf::Model&& model) noexcept { m_gltf = std::move(model); }

	void LoadFromFile(const char* fileName);

	[[nodiscard]]
	const tinygltf::Model& Get() const noexcept { return m_gltf; }

private:
	static void LoadFromFile(tinygltf::Model& model, const char* fileName);

private:
	tinygltf::Model m_gltf;
};
}
#endif
