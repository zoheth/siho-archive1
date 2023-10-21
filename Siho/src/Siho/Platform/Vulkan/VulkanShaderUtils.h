#pragma once
#include <shaderc/shaderc.h>

#include <vulkan/vulkan_core.h>
#include "Siho/Renderer/Shader.h"


namespace Siho::ShaderUtils {

	inline static std::string_view VKStageToShaderMacro(const VkShaderStageFlagBits stage)
	{
		if (stage == VK_SHADER_STAGE_VERTEX_BIT)   return "__VERTEX_STAGE__";
		if (stage == VK_SHADER_STAGE_FRAGMENT_BIT) return "__FRAGMENT_STAGE__";
		if (stage == VK_SHADER_STAGE_COMPUTE_BIT)  return "__COMPUTE_STAGE__";
		SH_CORE_ASSERT(false, "Unknown shader stage.");
		return "UNKNOWN";
	}

	inline static VkShaderStageFlagBits ShaderTypeFromString(const std::string_view type)
	{
		if (type == "vert")	return VK_SHADER_STAGE_VERTEX_BIT;
		if (type == "frag")	return VK_SHADER_STAGE_FRAGMENT_BIT;
		if (type == "comp")	return VK_SHADER_STAGE_COMPUTE_BIT;

		return VK_SHADER_STAGE_ALL;
	}

	inline static const char* ShaderStageToString(const VkShaderStageFlagBits stage)
	{
		switch (stage)
		{
		case VK_SHADER_STAGE_VERTEX_BIT:    return "vert";
		case VK_SHADER_STAGE_FRAGMENT_BIT:  return "frag";
		case VK_SHADER_STAGE_COMPUTE_BIT:   return "comp";
		}
		SH_CORE_ASSERT(false);
		return "UNKNOWN";
	}

	inline static shaderc_shader_kind ShaderStageToShaderC(const VkShaderStageFlagBits stage)
	{
		switch (stage)
		{
		case VK_SHADER_STAGE_VERTEX_BIT:    return shaderc_vertex_shader;
		case VK_SHADER_STAGE_FRAGMENT_BIT:  return shaderc_fragment_shader;
		case VK_SHADER_STAGE_COMPUTE_BIT:   return shaderc_compute_shader;
		}
		SH_CORE_ASSERT(false);
		return {};
	}
}