#pragma once
#include "Base.h"

#include "Window.h"
#include "Siho/Core/LayerStack.h"
#include "Siho/Events/Event.h"
#include "Siho/Events/ApplicationEvent.h"

#include "Siho/ImGui/ImGuiLayer.h"

namespace Siho {

	struct ApplicationSpecs
	{
		std::string Name = "Siho";
		uint32_t WindowWidth = 1600, WindowHeight = 900;

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
		
		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
	private:
		std::unique_ptr<Window> m_Window;
		ApplicationSpecs m_Specification;
		bool m_Running = true;
		bool m_Minimized = false;

		ImGuiLayer* m_ImGuiLayer;
		LayerStack m_LayerStack;

		RenderThread m_RenderThread;

		PerformanceTimers m_PerformanceTimers;

		static Application* s_Instance;

		friend class Renderer;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}

