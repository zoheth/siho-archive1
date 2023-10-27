#pragma once

#ifdef SH_PLATFORM_WINDOWS

extern Siho::Application* Siho::CreateApplication();

int main(int args, char** argv)
{
	Siho::Log::Init();
	SH_CORE_WARN("Initialized Log!");
	int a=9;
	SH_INFO("Hello Var={0}", a);
	auto app = Siho::CreateApplication();
	app->Run();
	delete app;
}
#endif