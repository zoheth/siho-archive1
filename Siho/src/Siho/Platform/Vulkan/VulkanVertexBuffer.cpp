#include "shpch.h"
#include "VulkanVertexBuffer.h"

#include "VulkanContext.h"
#include "Siho/Renderer/Renderer.h"

namespace Siho {


	VulkanVertexBuffer::VulkanVertexBuffer(void* data, uint32_t size, VertexBufferUsage usage /*= VertexBufferUsage::Static*/)
		:m_Size(size)
	{
		m_LocalData = Buffer::CreateFromCopy(data, size);

		Ref<VulkanVertexBuffer> instance = this;
		Renderer::Submit([instance]() mutable
			{
				auto device = VulkanContext::GetCurrentDevice();
				VulkanAllocator allocator("VertexBuffer");

				VkBufferCreateInfo bufferInfo = {};
				bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				bufferInfo.size = instance->m_Size;
				bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
				bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
				VkBuffer stagingBuffer;
				VmaAllocation stagingBufferAllocation = allocator.AllocateBuffer(bufferInfo, VMA_MEMORY_USAGE_CPU_TO_GPU, stagingBuffer);

				uint8_t* destData = allocator.MapMemory<uint8_t>(stagingBufferAllocation);
				memcpy(destData, instance->m_LocalData.Data, instance->m_LocalData.Size);
				allocator.UnmapMemory(stagingBufferAllocation);

				bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
				instance->m_MemoryAllocation = allocator.AllocateBuffer(bufferInfo, VMA_MEMORY_USAGE_GPU_ONLY, instance->m_VulkanBuffer);

				VkCommandBuffer copyCmd = device->GetCommandBuffer(true);

				VkBufferCopy copyRegion = {};
				copyRegion.size = instance->m_Size;
				vkCmdCopyBuffer(copyCmd, stagingBuffer, instance->m_VulkanBuffer, 1, &copyRegion);

				device->FlushCommandBuffer(copyCmd);

				allocator.DestroyBuffer(stagingBuffer, stagingBufferAllocation);
			});
	}

	VulkanVertexBuffer::VulkanVertexBuffer(uint32_t size, VertexBufferUsage usage /*= VertexBufferUsage::Dynamic*/)
		: m_Size(size)
	{
		m_LocalData.Allocate(size);

		Ref<VulkanVertexBuffer> instance = this;
		Renderer::Submit([instance]() mutable
			{
				auto device = VulkanContext::GetCurrentDevice();
				VulkanAllocator allocator("VertexBuffer");

				VkBufferCreateInfo bufferInfo = {};
				bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				bufferInfo.size = instance->m_Size;
				bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

				instance->m_MemoryAllocation = allocator.AllocateBuffer(bufferInfo, VMA_MEMORY_USAGE_CPU_TO_GPU, instance->m_VulkanBuffer);
			});
	}



	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		VkBuffer buffer = m_VulkanBuffer;
		VmaAllocation allocation = m_MemoryAllocation;
		Renderer::Submit()
	}

	void VulkanVertexBuffer::SetData(void* data, uint32_t size, uint32_t offset /*= 0*/)
	{

	}

	void VulkanVertexBuffer::RenderThread_SetData(void* buffer, uint32_t size, uint32_t offset /*= 0*/)
	{

	}

}