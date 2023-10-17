#include "shpch.h"
#include "Renderer.h"

namespace Siho {
	static RendererConfig s_Config;
	
	void Renderer::Init()
	{

	}
	RendererConfig& Renderer::GetConfig()
	{
		return s_Config;
	}
}