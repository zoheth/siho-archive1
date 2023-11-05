#include "shpch.h"
#include "RendererContext.h"

#include "Siho/Platform/Vulkan/VulkanContext.h"

namespace Siho {
	std::unique_ptr<RendererContext> RendererContext::Create()
	{
		return std::make_unique<VulkanContext>();
	}
}
