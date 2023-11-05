#pragma once
#include "Shader.h"

#include <filesystem>

namespace Siho {

	class ShaderPack : public RefCounted
	{
	public:
		ShaderPack() = default;
		ShaderPack(const std::filesystem::path& path);
	private:
		bool m_Loaded = false;
	};
}