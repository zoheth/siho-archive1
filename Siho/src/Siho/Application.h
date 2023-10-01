#pragma once
#include "Core.h"

namespace Siho {

	class SIHO_API Application
	{
	public:
		Application();
		virtual ~Application();
		void Run();
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}

