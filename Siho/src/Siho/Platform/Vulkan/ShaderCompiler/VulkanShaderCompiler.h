#pragma once


#include "Siho/Platform/Vulkan/VulkanShader.h"

#include <filesystem>

namespace Siho {
	
	class VulkanShaderCompiler : public RefCounted
	{
	public:
		VulkanShaderCompiler(const std::filesystem::path& shaderSourcePath, bool disableOptimization = false);

		static Ref<VulkanShader> Compile(const std::filesystem::path& shaderSourcePath, bool forceCompile = false, bool disableOptimization = false);
	};
}