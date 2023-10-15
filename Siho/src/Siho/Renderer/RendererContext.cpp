#include "shpch.h"
#include "RendererContext.h"

#include "Siho/Platform/Vulkan/VulkanContext.h"

namespace Siho {
	Ref<RendererContext> RendererContext::Create(GLFWwindow* windowHandle)
	{
		return Ref<VulkanContext>::Create(windowHandle);
	}
}
