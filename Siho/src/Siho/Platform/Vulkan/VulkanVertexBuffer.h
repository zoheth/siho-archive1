#pragma once

#include "Siho/Renderer/VertexBuffer.h"
#include "Siho/Core/Buffer.h"
#include "VulkanAllocator.h"

namespace Siho {

	class VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(void* data, uint32_t size, VertexBufferUsage usage = VertexBufferUsage::Static);
		VulkanVertexBuffer(uint32_t size, VertexBufferUsage usage = VertexBufferUsage::Dynamic);

		virtual ~VulkanVertexBuffer() override;

		virtual void SetData(void* data, uint32_t size, uint32_t offset = 0) override;
		virtual void RenderThread_SetData(void* buffer, uint32_t size, uint32_t offset = 0) override;
		virtual void Bind() const override {};

	private:
		uint32_t m_Size;
		Buffer m_LocalData;

		VkBuffer m_VulkanBuffer = nullptr;
		VmaAllocation m_MemoryAllocation;
	};
}