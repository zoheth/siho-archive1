#pragma once
#include <shaderc/shaderc.h>

#include <vulkan/vulkan_core.h>
#include "Siho/Renderer/Shader.h"


namespace Siho::ShaderUtils {
	inline static VkShaderStageFlagBits ShaderTypeFromString(const std::string_view type)
	{
		if (type == "vert")	return VK_SHADER_STAGE_VERTEX_BIT;
		if (type == "frag")	return VK_SHADER_STAGE_FRAGMENT_BIT;
		if (type == "comp")	return VK_SHADER_STAGE_COMPUTE_BIT;

		return VK_SHADER_STAGE_ALL;
	}
}