#include "shpch.h"
#include "VulkanShader.h"

#include <vulkan/vulkan_core.h>

#include "Siho/Platform/Vulkan/VulkanContext.h"

namespace Siho {
	VulkanShader::VulkanShader(const std::string& filepath, bool forceCompile, bool disableOptimization)
		:m_AssetPath(filepath), m_DisableOptimization(disableOptimization)
	{
		size_t found = filepath.find_last_of("/\\");
		m_Name = found != std::string::npos ? filepath.substr(found + 1) : filepath;
		found = m_Name.find_last_of('.');
		m_Name = found != std::string::npos ? m_Name.substr(0, found) : m_Name;
	}

	void VulkanShader::LoadAndCreateShaders(const std::map<VkShaderStageFlagBits, std::vector<uint32_t>>& shaderData)
	{
		m_ShaderData = shaderData;

		VkDevice device = VulkanContext::GetCurrentDevice()->GetHandle();
		m_PipelineShaderStageCreateInfos.clear();
		std::string moduleName;
		for (auto [stage, data] : shaderData)
		{
			SH_CORE_ASSERT(!data.empty(), "Shader data is empty!");
			VkShaderModuleCreateInfo moduleCreateInfo{};

			moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			moduleCreateInfo.codeSize = data.size() * sizeof(uint32_t);
			moduleCreateInfo.pCode = data.data();

			VkShaderModule shaderModule;
			VK_CHECK_RESULT(vkCreateShaderModule(device, &moduleCreateInfo, nullptr, &shaderModule));

			VkPipelineShaderStageCreateInfo& shaderStage = m_PipelineShaderStageCreateInfos.emplace_back();
			shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStage.stage = stage;
			shaderStage.module = shaderModule;
			shaderStage.pName = "main";
		}

	}

}