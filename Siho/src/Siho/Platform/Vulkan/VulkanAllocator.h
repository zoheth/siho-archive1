#pragma once

#include <string>
#include "Vulkan.h"
#include "VulkanDevice.h"

namespace Siho {

	class VulkanAllocator
	{
	public:
		VulkanAllocator() = default;
		VulkanAllocator(const std::string& tag);
		VulkanAllocator(const Ref<VulkanDevice>& device, const std::string& tag = "");
		~VulkanAllocator();

		void Allocate(VkMemoryRequirements requirements, VkDeviceMemory* memory, VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	private:
		Ref<VulkanDevice> m_Device;
		std::string m_Tag;
	};
}