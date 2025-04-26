#include <GLTFObject.hpp>

namespace Sol
{
void GLTFObject::LoadFromFile(const char* fileName)
{
	LoadFromFile(m_gltf, fileName);
}

void GLTFObject::LoadFromFile(tinygltf::Model& model, const char* fileName)
{
    tinygltf::TinyGLTF loader{};
    std::string        error{};
    std::string        warning{};

    loader.LoadASCIIFromFile(&model, &error, &warning, fileName);

    // Should add logging here.
}
}
