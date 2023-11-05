#include "shpch.h"
#include "Shader.h"

#include "Siho/Renderer/RendererAPI.h"
#include "Siho/Renderer/ShaderPack.h"
#include "Siho/Platform/Vulkan/VulkanShader.h"


namespace Siho {

	Ref<Shader> Shader::Create(const std::string& filepath, bool forceCompile /*= false*/, bool disableOptimization /*= false*/)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPIType::Vulkan:
			return Ref<VulkanShader>::Create(filepath, forceCompile, disableOptimization);
			break;
		}
		return nullptr;
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
		// shader = VulkanShaderCompiler::Compile()

	}

}

