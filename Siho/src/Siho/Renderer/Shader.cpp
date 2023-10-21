#include "shpch.h"
#include "Shader.h"

namespace Siho {

	Ref<Shader> Shader::Create(const std::string& filepath, bool forceCompile /*= false*/, bool disableOptimization /*= false*/)
	{

	}

	void ShaderLibrary::Load(std::string_view path, bool forceCompile, bool disableOptimization)
	{
		Ref<Shader> shader;
		/*if (!forceCompile && m_ShaderPack)
		{
			if (m_ShaderPack->Contains(path))
				shader = m_ShaderPack->LoadShader(path);
		}*/

		// UNDONE 1020
		shader = VulkanShaderCompiler::Compile()

	}

}

