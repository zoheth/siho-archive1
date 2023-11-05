#pragma once

#include <string>

#include <vk_mem_alloc.h>

#include "Vulkan.h"
#include "VulkanDevice.h"

namespace Siho {

	struct GpuMemoryStats
	{
		uint64_t Used = 0;
		uint64_t Free = 0;
	};

	class VulkanAllocator
	{
	public:
		VulkanAllocator() = default;
		VulkanAllocator(const std::string& tag);
		~VulkanAllocator();

		// void Allocate(VkMemoryRequirements requirements, VkDeviceMemory* memory, VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		VmaAllocation AllocateBuffer(VkBufferCreateInfo bufferInfo, VmaMemoryUsage usage, VkBuffer& outBuffer);
		VmaAllocation AllocateImage(VkImageCreateInfo imageInfo, VmaMemoryUsage usage, VkImage& outImage, VkDeviceSize* allocatedSize = nullptr);
		void Free(VmaAllocation allocation);
		void DestroyBuffer(VkBuffer buffer, VmaAllocation allocation);
		void DestroyImage(VkImage image, VmaAllocation allocation);

		template<typename T>
		T* MapMemory(VmaAllocation allocation)
		{
			T* data;
			vmaMapMemory(VmaAllocator::GetVmaAllocator(), allocation, (void**)&data);
			return data;
		}

		void UnmapMemory(VmaAllocation allocation);

		static void DumpStats();
		// static GpuMemoryStats GetStats();

		static void Init(const VulkanDevice& device);
		static void Shutdown();

		static VmaAllocator& GetVmaAllocator();
	private:
		std::string m_Tag;
	};
}