#pragma once

#include "Siho/Core/Window.h"
#include "Siho/Renderer/RendererContext.h"
#include "Siho/Platform/Vulkan/VulkanSwapChain.h"

#include <GLFW/glfw3.h>

namespace Siho {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		virtual void ProcessEvents() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		// Window attributes
		void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		void* GetNativeWindow() const override { return m_Window; }

		virtual VulkanSwapChain& GetSwapChain() override;
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;
			EventCallbackFn EventCallback;
		};
		WindowData m_Data;

		VulkanSwapChain m_SwapChain;
	};
	
}

