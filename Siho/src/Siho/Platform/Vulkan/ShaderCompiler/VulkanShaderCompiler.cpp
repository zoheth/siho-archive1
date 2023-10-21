#include "shpch.h"
#include "VulkanShaderCompiler.h"

#include "Siho/Utilities/StringUtils.h"
#include "Siho/Renderer/Renderer.h"
#include "Siho/Core/Hash.h"

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
		const VkShaderStageFlagBits changedStages = 
		// UNDONE
	}

	Ref<VulkanShader> VulkanShaderCompiler::Compile(const std::filesystem::path& shaderSourcePath, bool forceCompile, bool disableOptimization)
	{
		std::string path = shaderSourcePath.string();

		Ref<VulkanShader> shader = Ref<VulkanShader>::Create(path, forceCompile, disableOptimization);

		Ref<VulkanShaderCompiler> compiler = Ref<VulkanShaderCompiler>::Create(shaderSourcePath, disableOptimization);
		compiler->Reload(forceCompile);

		// UNDONE
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

		for (auto& [stage, shaderSource] : shaderSources)
		{
			shaderc::CompileOptions options;
			options.AddMacroDefinition("__GLSL__");
			options.AddMacroDefinition(std::string(ShaderUtils::VKStageToShaderMacro(stage)));

			const auto& globalMacros = Renderer::GetGlobalShaderMacros();
			for(const auto& [key, value] : globalMacros)
				options.AddMacroDefinition(key, value);

			// TODO Includer
			
			const auto preProcessingResult = compiler.PreprocessGlsl(
				shaderSource, 
				ShaderUtils::ShaderStageToShaderC(stage),
				m_ShaderSourcePath.string().c_str(),
				options);
			if (preProcessingResult.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				SH_CORE_ERROR("[Renderer] Failed to pre-process shader \"{}\"'s {} shader.\nError: {}", 
					m_ShaderSourcePath.string(),
					ShaderUtils::ShaderStageToString(stage),
					preProcessingResult.GetErrorMessage());
				return {};
			}

			m_StagesMetadata[stage].HashValue = Hash::GenerateFNVHash(shaderSource);
			// m_StagesMetadata[stage].Headers = std::move(includer->GetIncludeData());

			// m_AcknowledgedMacros.merge(includer->GetParsedSpecialMacros());

			shaderSource = std::string(preProcessingResult.begin(), preProcessingResult.end());
		}
		return shaderSources;
	}

}