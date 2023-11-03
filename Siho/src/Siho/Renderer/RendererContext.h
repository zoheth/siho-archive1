#pragma once

struct GLFWwindow;

namespace Siho {

	class RendererContext
	{
	public:
		virtual ~RendererContext() = default;

		RendererContext(const RendererContext&) = delete;
		RendererContext& operator=(const RendererContext&) = delete;
		RendererContext(RendererContext&&) = delete;
		RendererContext& operator=(RendererContext&&) = delete;

		virtual void Create() = 0;
		virtual void BeginFrame() = 0;
		virtual void SwapBuffers() = 0;

		virtual void OnResize(uint32_t width, uint32_t height) = 0;

		static std::shared_ptr<RendererContext> Create(GLFWwindow* windowHandle);
	};
}