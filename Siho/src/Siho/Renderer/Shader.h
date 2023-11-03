#pragma once

#include "Siho/Core/Ref.h"

#include <string>

namespace Siho
{
	class Shader
	{
	public:
		static Ref<Shader> Create(const std::string& filepath, bool forceCompile = false, bool disableOptimization = false);
	};

	class ShaderPack;

	class ShaderLibrary
	{
	public:
		ShaderLibrary() = default;
		~ShaderLibrary() = default;
		// Using std::string_view instead of const std::string& for greater flexibility in accepting various string representations(e.g., std::string, char arrays, etc.), 
		// avoiding potential temporary string constructions, and optimized substring operations.
		void Load(std::string_view path, bool forceCompile = false, bool disableOptimization = false);
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
		Ref<ShaderPack> m_ShaderPack;
	};
}