#include <Siho.h>

class Sandbox : public Siho::Application
{
public:
	Sandbox(){}
	~Sandbox(){}
};


Siho::Application* Siho::CreateApplication()
{
	return new Sandbox();
}
