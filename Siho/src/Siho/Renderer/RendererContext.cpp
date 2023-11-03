#include "shpch.h"
#include "RendererContext.h"

#include "Siho/Platform/Vulkan/VulkanContext.h"

namespace Siho {
	std::shared_ptr<RendererContext> RendererContext::Create(GLFWwindow* windowHandle)
	{
		return std::make_shared<VulkanContext>(windowHandle);
	}
}
