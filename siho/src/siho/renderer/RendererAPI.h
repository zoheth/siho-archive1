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

		static RendererAPIType Current() { return s_CurrentRendererAPI; }
	private:
		// Using 'inline' allows for the definition of the static member variable directly within the class. 
		// Without it, a separate definition would be required in a source file to prevent linkage issues.
		inline static RendererAPIType s_CurrentRendererAPI = RendererAPIType::Vulkan;
	};

}