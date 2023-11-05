#pragma once
#include "Base.h"
#include "RenderThread.h"

#include "Window.h"
#include "siho/core/LayerStack.h"
#include "siho/events/Event.h"
#include "siho/events/ApplicationEvent.h"

#include "siho/imgui/ImGuiLayer.h"

namespace Siho {

	struct ApplicationSpecs
	{
		std::string Name = "Siho";
		uint32_t WindowWidth = 1600, WindowHeight = 900;
		ThreadingPolicy CoreThreadingPolicy = ThreadingPolicy::MultiThreaded;

	};

	class Application
	{
		using EventCallbackFn = std::function<void(Event&)>;
	public:
		struct PerformanceTimers
		{
			float MainThreadWorkTime = 0.0f;
			float MainThreadIdleTime = 0.0f;
			float RenderThreadWorkTime = 0.0f;
			float RenderThreadIdleTime = 0.0f;
		};
	public:
		Application(const ApplicationSpecs& specification);
		virtual ~Application();


		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;
		Application(Application&&) = delete;
		Application& operator=(Application&&) = delete;

		
		void Run();
		void Close();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		static Application& Get() { return *s_Instance; }
		RendererContext& GetRenderContext() const { return *m_RenderContext; }
		Window& GetWindow() const { return *m_Window; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
	private:
		std::unique_ptr<Window> m_Window;
		std::unique_ptr<RendererContext> m_RenderContext;

		ApplicationSpecs m_Specification;
		bool m_Running = true;
		bool m_Minimized = false;

		ImGuiLayer* m_ImGuiLayer;
		LayerStack m_LayerStack;

		RenderThread m_RenderThread;

		PerformanceTimers m_PerformanceTimers;

		static Application* s_Instance;

		friend class Renderer;
	protected:
		inline static bool s_IsRuntime = false;
	};

	// To be defined in CLIENT
	Application* CreateApplication(int argc, char** argv);
}

