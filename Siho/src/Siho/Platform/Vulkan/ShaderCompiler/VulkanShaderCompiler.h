#pragma once

#include "vulkan/vulkan.h"

#include "Siho/Platform/Vulkan/VulkanShader.h"

#include "ShaderPreprocessing/ShaderPreprocessor.h"


#include <map>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>

namespace Siho {
	namespace Utils {
		static const char* GetCacheDirectory()
		{
			return "Resources/Cache/Shader/Vulkan";
		}
		static void CreateCacheDirectoryIfNeeded()
		{
			std::string cacheDirectory = GetCacheDirectory();
			if (!std::filesystem::exists(cacheDirectory))
				std::filesystem::create_directories(cacheDirectory);
		}
	}
	struct StageData
	{
		std::unordered_set<IncludeData> Headers;
		uint32_t HashValue{ 0 };
		bool operator==(const StageData& rhs) const noexcept
		{
			return Headers == rhs.Headers && HashValue == rhs.HashValue;
		}
		bool operator!=(const StageData& rhs) const noexcept
		{
			return !(*this == rhs);
		}
	};

	class VulkanShaderCompiler : public RefCounted
	{
	public:
		VulkanShaderCompiler(const std::filesystem::path& shaderSourcePath, bool disableOptimization = false);

		bool Reload(bool forceCompile = false);

		static Ref<VulkanShader> Compile(const std::filesystem::path& shaderSourcePath, bool forceCompile = false, bool disableOptimization = false);

	private:
		std::map<VkShaderStageFlagBits, std::string> PreProcess(const std::string& source);
		std::map<VkShaderStageFlagBits, std::string> PreProcessGLSL(const std::string& source);

	private:
		std::filesystem::path m_ShaderSourcePath;
		bool m_DisableOptimization{ false };

		std::map<VkShaderStageFlagBits, std::string> m_ShaderSources;
		std::map<VkShaderStageFlagBits, std::vector<uint32_t>> m_SPIRVDebugData, m_SPIRVData;

		// Names of macros that are parsed from shader.
		// These are used to reliably get information about what shaders need what macros
		std::unordered_set<std::string> m_AcknowledgedMacros;

		std::map<VkShaderStageFlagBits, StageData> m_StagesMetadata;

		friend class VulkanShader;
	};
}