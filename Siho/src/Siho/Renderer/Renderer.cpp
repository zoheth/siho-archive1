#include "shpch.h"
#include "Renderer.h"

#include "Shader.h"
#include "ShaderPack.h"

namespace Siho {
	static RendererConfig s_Config;
	
	void Renderer::Init()
	{
		// UNDONE 1020
		// Renderer::GetShaderLibrary()->Load("Resources/Shaders/HazelPBR_Static.glsl");
	}

	struct RendererData
	{
		Ref<ShaderLibrary> m_ShaderLibrary;

		std::unordered_map<std::string, std::string> GlobalShaderMacros;
	};

	static RendererData* s_Data = nullptr;

	Ref<ShaderLibrary> Renderer::GetShaderLibrary()
	{
		return s_Data->m_ShaderLibrary;
	}
	RendererConfig& Renderer::GetConfig()
	{
		return s_Config;
	}

	const std::unordered_map<std::string, std::string>& Renderer::GetGlobalShaderMacros()
	{
		return s_Data->GlobalShaderMacros;
	}

}