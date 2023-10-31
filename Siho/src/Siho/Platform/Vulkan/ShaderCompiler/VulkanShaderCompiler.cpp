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
		// const VkShaderStageFlagBits changedStages = 
		
		for (const auto& [stage, source] : m_ShaderSources)
		{
			std::vector<uint32_t>& spirv = m_SPIRVData[stage];
			// Disable optimization for compute shaders because of shaderc internal error
			bool optimize = !m_DisableOptimization && stage != VK_SHADER_STAGE_COMPUTE_BIT;
			std::string error = Compile(spirv, stage, { false, optimize });
			if (!error.empty())
			{
				SH_CORE_ERROR(error);
				return false;
			}
			spirv = m_SPIRVDebugData[stage];
			error = Compile(spirv, stage, { true, false });
			if (!error.empty())
			{
				SH_CORE_ERROR(error);
				return false;
			}
		}
		return true;
	}

	Ref<VulkanShader> VulkanShaderCompiler::Compile(const std::filesystem::path& shaderSourcePath, bool forceCompile, bool disableOptimization)
	{
		std::string path = shaderSourcePath.string();

		Ref<VulkanShader> shader = Ref<VulkanShader>::Create(path, forceCompile, disableOptimization);

		Ref<VulkanShaderCompiler> compiler = Ref<VulkanShaderCompiler>::Create(shaderSourcePath, disableOptimization);
		compiler->Reload(forceCompile);

		shader->LoadAndCreateShaders(compiler->GetSPIRVData());
		// TODO Reflection

	}

	std::string VulkanShaderCompiler::Compile(std::vector<uint32_t>& outputBinary, const VkShaderStageFlagBits stage, CompilationOptions options)
	{
		const std::string& source = m_ShaderSources.at(stage); // Use at to get an exception if the stage doesn't exist

		static shaderc::Compiler compiler;
		shaderc::CompileOptions compileOptions;
		compileOptions.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
		compileOptions.SetWarningsAsErrors();
		if (options.GenerateDebugInfo)
			compileOptions.SetGenerateDebugInfo();
		if (options.Optimize && !m_DisableOptimization)
			compileOptions.SetOptimizationLevel(shaderc_optimization_level_performance);

		// Compile shader
		const shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, ShaderUtils::ShaderStageToShaderC(stage), m_ShaderSourcePath.string().c_str(), compileOptions);

		if (module.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			return fmt::format("[Renderer] Failed to compile shader \"{}\"'s {} shader.\nError: {}",
				m_ShaderSourcePath.string(),
				ShaderUtils::ShaderStageToString(stage),
				module.GetErrorMessage());
		}
		outputBinary = { module.cbegin(), module.cend() };
		return {};
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
			for (const auto& [key, value] : globalMacros)
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