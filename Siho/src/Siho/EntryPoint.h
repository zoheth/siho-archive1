#pragma once

#ifdef SH_PLATFORM_WINDOWS

extern Siho::Application* Siho::CreateApplication();

int main(int args, char** argv)
{
	auto app = Siho::CreateApplication();
	app->Run();
	delete app;
}
#endif