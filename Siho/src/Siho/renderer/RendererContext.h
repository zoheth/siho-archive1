#pragma once

namespace Siho {

	class RendererContext
	{
	public:
		RendererContext() = default;
		virtual ~RendererContext() = default;

		RendererContext(const RendererContext&) = delete;
		RendererContext& operator=(const RendererContext&) = delete;
		RendererContext(RendererContext&&) = delete;
		RendererContext& operator=(RendererContext&&) = delete;

		virtual void Init() = 0;

		static std::unique_ptr<RendererContext> Create();
	};
}