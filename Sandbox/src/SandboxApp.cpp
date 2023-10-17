#include <Siho.h>

#include "imgui/imgui.h"

class ExampleLayer : public Siho::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate() override
	{
		SH_INFO("ExampleLayer::Update");
		if(Siho::Input::IsKeyPressed(SH_KEY_TAB))
			SH_TRACE("Tab key is pressed (poll)!");
	}

	virtual void OnImGuiRender() override
	{
		ImGui::ShowDemoWindow();
		ImGui::Begin("Test");
		ImGui::Text("Hello World");
		ImGui::End();
	}

	void OnEvent(Siho::Event& event) override
	{
		SH_TRACE("{0}", event);
		if (event.GetEventType() == Siho::EventType::KeyPressed)
		{
			Siho::KeyPressedEvent& e = (Siho::KeyPressedEvent&)event;
			if (e.GetKeyCode() == SH_KEY_TAB)
				SH_TRACE("Tab key is pressed (event)!");
			SH_TRACE("{0}", (char)e.GetKeyCode());
		}
	}
};

class Sandbox : public Siho::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}
	~Sandbox(){}
};


Siho::Application* Siho::CreateApplication()
{
	return new Sandbox();
}
