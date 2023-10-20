#include "shpch.h"
#include "VulkanShaderCompiler.h"

#include "Siho/Utilities/StringUtils.h"

#include <shaderc/shaderc.hpp>
#include <libshaderc_util/file_finder.h>

namespace Siho {
	VulkanShaderCompiler::VulkanShaderCompiler(const std::filesystem::path& shaderSourcePath, bool disableOptimization)
		:m_ShaderSourcePath(shaderSourcePath), m_DisableOptimization(disableOptimization)
	{
		
	}

	bool VulkanShaderCompiler::Reload(bool forceCompile)
	{
		m_ShaderSources.clear();
		m_StagesMetadata.clear();
		m_SPIRVDebugData.clear();
		m_SPIRVData.clear();

		Utils::CreateCacheDirectoryIfNeeded();
		const std::string source = Utils::ReadFileAndSkipBOM(m_ShaderSourcePath);
		SH_CORE_ASSERT(!source.empty(), "Shader source file not found!");

		SH_CORE_TRACE("[Renderer] Compiling shader: {}", m_ShaderSourcePath.string());
		m_ShaderSources = PreProcess(source);
	}

	Ref<VulkanShader> VulkanShaderCompiler::Compile(const std::filesystem::path& shaderSourcePath, bool forceCompile, bool disableOptimization)
	{
		std::string path = shaderSourcePath.string();

		Ref<VulkanShader> shader = Ref<VulkanShader>::Create(path, forceCompile, disableOptimization);

		Ref<VulkanShaderCompiler> compiler = Ref<VulkanShaderCompiler>::Create(shaderSourcePath, disableOptimization);
	}
	std::map<VkShaderStageFlagBits, std::string> VulkanShaderCompiler::PreProcess(const std::string& source)
	{
		return PreProcessGLSL(source);
	}

	std::map<VkShaderStageFlagBits, std::string> VulkanShaderCompiler::PreProcessGLSL(const std::string& source)
	{
		std::map<VkShaderStageFlagBits, std::string> shaderSources = ShaderPreprocessor::PreprocessShader(source, m_AcknowledgedMacros);

		static shaderc::Compiler compiler;

		shaderc_util::FileFinder fileFinder;
		fileFinder.search_path().emplace_back("Resources/Shaders/Include/GLSL/"); //Main include directory
		fileFinder.search_path().emplace_back("Resources/Shaders/Include/Common/"); //Shared include directory

		// UNDONE 1020
	}

}