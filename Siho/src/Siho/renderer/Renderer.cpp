#include "shpch.h"
#include "Renderer.h"

#include "Shader.h"
#include "ShaderPack.h"
#include "siho/core/Timer.h"
#include "siho/platform/Vulkan/VulkanSwapChain.h"

namespace Siho
{
	struct RendererData;
}

namespace 
{
	Siho::RendererConfig s_Config;
	Siho::RendererData* s_Data = nullptr;
	constexpr uint32_t s_RenderCommandQueueCount = 2;
	Siho::RenderCommandQueue* s_CommandQueue[s_RenderCommandQueueCount];
	std::atomic<uint32_t> s_RenderCommandQueueSubmissionIndex = 0;
	Siho::RenderCommandQueue* s_ResourceReleaseQueue[3];
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
			Timer waitTimer;
			renderThread->WaitAndSet(RenderThread::State::Kick, RenderThread::State::Busy);
			performanceTimer.RenderThreadIdleTime = waitTimer.ElapsedMillis();
		}
		Timer workTimer;
		s_CommandQueue[GetRenderQueueIndex()]->Execute();

		renderThread->Set(RenderThread::State::Idle);
		performanceTimer.RenderThreadWorkTime = workTimer.ElapsedMillis();
	}

	void Renderer::SwapQueues()
	{
		s_RenderCommandQueueSubmissionIndex = GetRenderQueueIndex();
	}

	void Renderer::RenderThreadFunc(RenderThread* renderThread)
	{
		while (renderThread->IsRunning())
		{
			WaitAndRender(renderThread);
		}
	}

	uint32_t Renderer::GetRenderQueueIndex()
	{
		return (s_RenderCommandQueueSubmissionIndex + 1) % s_RenderCommandQueueCount;
	}

	uint32_t Renderer::GetRenderQueueSubmissionIndex()
	{
		return s_RenderCommandQueueSubmissionIndex;
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