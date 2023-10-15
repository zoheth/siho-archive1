#include "shpch.h"
#include "VulkanAllocator.h"

#include "VulkanContext.h"

namespace Siho {
	VulkanAllocator::VulkanAllocator(const std::string& tag)
	{
		throw std::logic_error("VulkanAllocator::VulkanAllocator(const std::string& tag) is not implemented!");
	}
	VulkanAllocator::VulkanAllocator(const Ref<VulkanDevice>& device, const std::string& tag)
		: m_Device(device), m_Tag(tag)
	{
	}
	VulkanAllocator::~VulkanAllocator()
	{
	}
	void VulkanAllocator::Allocate(VkMemoryRequirements requirements, VkDeviceMemory* memory, VkMemoryPropertyFlags properties)
	{
		SH_CORE_ASSERT(m_Device);

		SH_CORE_TRACE("VulkanAllocator ({0}): allocating {1} bytes", m_Tag, requirements.size);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = requirements.size;
		auto memoryType = m_Device->GetPhysicalDevice()->GetMemoryTypeIndex(requirements.memoryTypeBits, properties);
		VK_CHECK_RESULT(vkAllocateMemory(m_Device->GetHandle(), &allocInfo, nullptr, memory));
	}
}