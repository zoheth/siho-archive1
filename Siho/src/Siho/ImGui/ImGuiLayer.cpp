#include "shpch.h"
#include "ImGuiLayer.h"

#include "Siho/Platform/Vulkan/VulkanImGuiLayer.h"

#include <imgui.h>

namespace Siho {

	ImGuiLayer* ImGuiLayer::Create()
	{
		return new VulkanImGuiLayer();
	}

}
