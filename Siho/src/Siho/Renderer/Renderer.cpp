#include "shpch.h"
#include "Renderer.h"

#include "Shader.h"
#include "ShaderPack.h"

namespace Siho
{
	struct RendererData;
}

namespace 
{
	Siho::RendererConfig s_Config;
	Siho::RendererData* s_Data = nullptr;
}

namespace Siho {
	
	void Renderer::Init()
	{
		// UNDONE 1020
		Renderer::GetShaderLibrary()->Load("Resources/Shaders/HazelPBR_Static.glsl");
	}

	struct RendererData
	{
		Ref<ShaderLibrary> m_ShaderLibrary;

		std::unordered_map<std::string, std::string> GlobalShaderMacros;
	};


	Ref<ShaderLibrary> Renderer::GetShaderLibrary()
	{
		return s_Data->m_ShaderLibrary;
	}

	void Renderer::WaitAndRender(RenderThread* renderThread)
	{
		auto& performanceTimer = Application::Get().m_PerformanceTimers;

		// Wait for kick, then set render thread to busy
		{
			Timer
		}
	}

	void Renderer::RenderThreadFunc(RenderThread* renderThread)
	{
		while (renderThread->IsRunning())
		{
			WaitAndRender(renderThread);
		}
	}

	RendererConfig& Renderer::GetConfig()
	{
		return s_Config;
	}

	uint32_t Renderer::GetCurrentFrameIndex()
	{

	}

	uint32_t Renderer::RT_GetCurrentFrameIndex()
	{
		// Swapchain owns the Render Thread frame index
		return Application::Get().GetWindow().GetSwapChain().GetCurrentBufferIndex();
	}

	Siho::RenderCommandQueue& Renderer::GetRenderResourceReleaseQueue(uint32_t index)
	{

	}

	const std::unordered_map<std::string, std::string>& Renderer::GetGlobalShaderMacros()
	{
		return s_Data->GlobalShaderMacros;
	}

}