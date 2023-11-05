#pragma once

#include <filesystem>
#include <map>

#include "vulkan/vulkan.h"

#include "Siho/Renderer/Shader.h"

namespace Siho {

	class VulkanShader : public Shader
	{
	public:
		VulkanShader() = delete;
		VulkanShader(const std::string& filepath, bool forceCompile = false, bool disableOptimization = false);
		virtual ~VulkanShader();

	private:
		void LoadAndCreateShaders(const std::map<VkShaderStageFlagBits, std::vector<uint32_t>>& shaderData);

	private:
		std::vector<VkPipelineShaderStageCreateInfo> m_PipelineShaderStageCreateInfos;

		std::filesystem::path m_AssetPath;
		std::string m_Name;
		bool m_DisableOptimization = false;

		std::map<VkShaderStageFlagBits, std::vector<uint32_t>> m_ShaderData;

		friend class ShaderPack;
		friend class VulkanShaderCompiler;
	};
}