#include "shpch.h"
#include "VulkanShaderCompiler.h"

namespace Siho {
	VulkanShaderCompiler::VulkanShaderCompiler(const std::filesystem::path& shaderSourcePath, bool disableOptimization)
	{
		
	}

	Ref<VulkanShader> VulkanShaderCompiler::Compile(const std::filesystem::path& shaderSourcePath, bool forceCompile, bool disableOptimization)
	{
		std::string path = shaderSourcePath.string();

		Ref<VulkanShader> shader = Ref<VulkanShader>::Create(path, forceCompile, disableOptimization);

		Ref<VulkanShaderCompiler> compiler = Ref<VulkanShaderCompiler>::Create(shaderSourcePath, disableOptimization);
	}
}