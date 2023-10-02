#include "Application.h"

#include "Siho/Events/ApplicationEvent.h"
#include "Siho/Log.h"

namespace Siho {
	Application::Application()
	{

	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		WindowResizeEvent e(1200, 720);
		SH_TRACE(e);

		while (true);
	}
};