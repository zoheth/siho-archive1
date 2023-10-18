#pragma once

#include "Siho/Renderer/Shader.h"

#include <filesystem>

namespace Siho {

	class VulkanShader : public Shader
	{
	public:
		VulkanShader() = delete;
		VulkanShader(const std::string& filepath, bool forceCompile = false, bool disableOptimization = false);
		virtual ~VulkanShader();

	private:
		std::filesystem::path m_AssetPath;
		std::string m_Name;
		bool m_DisableOptimization = false;
	};
}