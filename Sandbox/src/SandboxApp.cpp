#include <Siho.h>

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
	}

	void OnEvent(Siho::Event& event) override
	{
		SH_TRACE("{0}", event);
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
