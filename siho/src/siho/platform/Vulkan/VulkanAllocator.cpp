#include "shpch.h"
#include "VulkanAllocator.h"

#include "VulkanContext.h"


#if SH_LOG_RENDERER_ALLOCATIONS
#define SH_ALLOCATOR_LOG(...) SH_CORE_TRACE(__VA_ARGS__)
#else
#define SH_ALLOCATOR_LOG(...)
#endif

namespace Siho {

	struct VulkanAllocatorData
	{
		VmaAllocator Allocator;
		uint64_t TotalAllocatedBytes = 0;
	};

	static VulkanAllocatorData* s_Data = nullptr;

	VulkanAllocator::VulkanAllocator(const std::string& tag)
	{
		throw std::logic_error("VulkanAllocator::VulkanAllocator(const std::string& tag) is not implemented!");
	}
#if 0
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
#endif
	VmaAllocation VulkanAllocator::AllocateBuffer(VkBufferCreateInfo bufferInfo, VmaMemoryUsage usage, VkBuffer& outBuffer)
	{
		SH_CORE_TRACE(bufferInfo.size);

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = usage;

		VmaAllocation allocation;
		vmaCreateBuffer(s_Data->Allocator, &bufferInfo, &allocInfo, &outBuffer, &allocation, nullptr);

		VmaAllocationInfo info{};
		vmaGetAllocationInfo(s_Data->Allocator, allocation, &info);
		SH_ALLOCATOR_LOG("VulkanAllocator ({0}): allocated buffer of size {1} (total: {2})", m_Tag, bufferInfo.size, info.size);
		{
			s_Data->TotalAllocatedBytes += info.size;
			SH_ALLOCATOR_LOG("VulkanAllocator ({0}): total allocated bytes: {1}", m_Tag, s_Data->TotalAllocatedBytes);
		}

		return allocation;
	}


	VmaAllocation VulkanAllocator::AllocateImage(VkImageCreateInfo imageInfo, VmaMemoryUsage usage, VkImage& outImage, VkDeviceSize* allocatedSize /*= nullptr*/)
	{
		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = usage;

		VmaAllocation allocation;
		vmaCreateImage(s_Data->Allocator, &imageInfo, &allocInfo, &outImage, &allocation, nullptr);

		VmaAllocationInfo info{};
		vmaGetAllocationInfo(s_Data->Allocator, allocation, &info);
		if (allocation)
			*allocatedSize = info.size;
		SH_ALLOCATOR_LOG("VulkanAllocator ({0}): allocated image of size {1} (total: {2})", m_Tag, info.size, info.size);
		{
			s_Data->TotalAllocatedBytes += info.size;
			SH_ALLOCATOR_LOG("VulkanAllocator ({0}): total allocated bytes: {1}", m_Tag, s_Data->TotalAllocatedBytes);
		}

		if (allocatedSize)
			*allocatedSize = info.size;

		return allocation;
	}



	void VulkanAllocator::Free(VmaAllocation allocation)
	{
		vmaFreeMemory(s_Data->Allocator, allocation);
	}

	void VulkanAllocator::DestroyBuffer(VkBuffer buffer, VmaAllocation allocation)
	{
		SH_CORE_ASSERT(buffer, "Buffer is null!");
		SH_CORE_ASSERT(allocation, "Allocation is null!");
		vmaDestroyBuffer(s_Data->Allocator, buffer, allocation);
	}

	void VulkanAllocator::DestroyImage(VkImage image, VmaAllocation allocation)
	{
		SH_CORE_ASSERT(image, "Image is null!");
		SH_CORE_ASSERT(allocation, "Allocation is null!");
		vmaDestroyImage(s_Data->Allocator, image, allocation);
	}

	void VulkanAllocator::UnmapMemory(VmaAllocation allocation)
	{
		vmaUnmapMemory(s_Data->Allocator, allocation);
	}

	void VulkanAllocator::DumpStats()
	{

	}

	//Siho::GpuMemoryStats VulkanAllocator::GetStats()
	//{
	//	/*const auto& memoryProperties = VulkanContext::GetCurrentDevice()->GetPhysicalDevice()->GetMemoryProperties();
	//	std::vector<VmaBudget> budgets(memoryProperties.memoryHeapCount);
	//	vmaGetBudget(s_Data->Allocator, budgets.data());*/
	//}

	void VulkanAllocator::Init(const VulkanDevice& device)
	{
		SH_CORE_ASSERT(!s_Data, "VulkanAllocator already exists!");
		s_Data = hnew VulkanAllocatorData();

		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3;
		allocatorInfo.physicalDevice = device.GetPhysicalDevice().GetHandle();
		allocatorInfo.device = device.GetHandle();
		allocatorInfo.instance = VulkanContext::GetInstance();
		vmaCreateAllocator(&allocatorInfo, &s_Data->Allocator);
	}

	void VulkanAllocator::Shutdown()
	{
		vmaDestroyAllocator(s_Data->Allocator);

		hdelete s_Data;
		s_Data = nullptr;
	}

	VmaAllocator& VulkanAllocator::GetVmaAllocator()
	{
		return s_Data->Allocator;
	}

}