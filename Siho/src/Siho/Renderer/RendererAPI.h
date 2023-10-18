#pragma once

namespace Siho {

	enum class RendererAPIType
	{
		None,
		Vulkan
	};

	class RendererAPI
	{
	public:
		virtual void Init() = 0;
		virtual void Shutdown() = 0;
	};

}