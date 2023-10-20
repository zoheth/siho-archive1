#include "shpch.h"
#include "Renderer.h"

namespace Siho {
	static RendererConfig s_Config;
	
	void Renderer::Init()
	{
		// UNDONE 1020
		Renderer::GetShaderLibrary()->Load("Resources/Shaders/HazelPBR_Static.glsl");
	}

	struct RendererData
	{
		Ref<ShaderLibrary> m_ShaderLibrary;
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
}