#include "shpch.h"
#include "VulkanShader.h"

namespace Siho {
	VulkanShader::VulkanShader(const std::string& filepath, bool forceCompile, bool disableOptimization)
		:m_AssetPath(filepath), m_DisableOptimization(disableOptimization)
	{
		size_t found = filepath.find_last_of("/\\");
		m_Name = found != std::string::npos ? filepath.substr(found + 1) : filepath;
		found = m_Name.find_last_of('.');
		m_Name = found != std::string::npos ? m_Name.substr(0, found) : m_Name;
	}
}