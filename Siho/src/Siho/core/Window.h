#pragma once

#include "Base.h"
#include "siho/events/Event.h"
#include "siho/renderer/RendererContext.h"

namespace Siho {
	class VulkanSwapChain;
	class RendererContext;

	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const std::string& title = "Siho Engine",
			unsigned int width = 1280,
			unsigned int height = 720)
			: Title(title), Width(width), Height(height)
		{
		}
	};

	// Interface representing a desktop system based Window
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void ProcessEvents() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		virtual VulkanSwapChain& GetSwapChain() = 0;


		// Every platform has its own implementation
		static std::unique_ptr<Window> Create(const WindowProps& props = WindowProps());
	};
}