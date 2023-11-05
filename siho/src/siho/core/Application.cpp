#include "shpch.h"
#include "Application.h"

#include "siho/events/ApplicationEvent.h"

#include "siho/platform/Vulkan/VulkanContext.h"
#include "siho/platform/Vulkan/VulkanSwapChain.h"

#include "Input.h"

namespace Siho {

	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecs& specification)
		:m_Specification(specification), m_RenderThread(specification.CoreThreadingPolicy)
	{
		s_Instance = this;

		m_RenderThread.Run();

		m_Window = Window::Create();
		m_Window->SetEventCallback([this](Event& e) { this->OnEvent(e); });

		m_RenderContext = RendererContext::Create();

		m_ImGuiLayer = ImGuiLayer::Create();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{

	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
	}

	void Application::Run()
	{
		bool isFirst = true;
		while (m_Running)
		{
			m_Window->ProcessEvents();

			Ref<VulkanContext> context = m_Window->GetRenderContext();
			VulkanSwapChain& swapChain = context->GetSwapChain();

			context->BeginFrame();

			VkCommandBufferBeginInfo cmdBufInfo = {};
			cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			cmdBufInfo.pNext = nullptr;

			// The assignment just copies a handle, not the entire command buffer object.
			// Therefore, drawCommandBuffer and swapChain.GetCurrentDrawCommandBuffer() point to the same command buffer.
			context->Wait();
			VkCommandBuffer drawCommandBuffer = swapChain.GetCurrentDrawCommandBuffer();

			VK_CHECK_RESULT(vkBeginCommandBuffer(drawCommandBuffer, &cmdBufInfo));

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();

			VK_CHECK_RESULT(vkEndCommandBuffer(drawCommandBuffer));

			// On Render thread
			
			context->SwapBuffers();
		}
	}
	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& e) { return this->OnWindowClose(e); });
		

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.m_Handled)
				break;
		}
	}
	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
};