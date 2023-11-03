#include "shpch.h"
#include "RenderCommandQueue.h"

namespace Siho {

	RenderCommandQueue::RenderCommandQueue()
	{
		m_CommandBuffer = new uint8_t[10 * 1024 * 1024]; // 10MB
		m_CommandBufferPtr = m_CommandBuffer;
		memset(m_CommandBuffer, 0, 10 * 1024 * 1024);
	}

	RenderCommandQueue::~RenderCommandQueue()
	{
		delete[] m_CommandBuffer;
	}

	void* RenderCommandQueue::Allocate(RenderCommandFn func, uint32_t size)
	{
		*reinterpret_cast<RenderCommandFn*>(m_CommandBufferPtr) = func;
		m_CommandBufferPtr += sizeof(RenderCommandFn);

		*reinterpret_cast<uint32_t*>(m_CommandBufferPtr) = size;
		m_CommandBufferPtr += sizeof(uint32_t);

		void* memory = m_CommandBufferPtr;
		m_CommandBufferPtr += size;

		m_CommandCount++;
		return memory;
	}

	void RenderCommandQueue::Execute()
	{
		uint8_t* buffer = m_CommandBuffer;
		for (uint32_t i = 0; i < m_CommandCount; i++)
		{
			// Get the function pointer
			const RenderCommandFn func = *reinterpret_cast<RenderCommandFn*>(buffer);
			buffer += sizeof(RenderCommandFn);

			// Get the size of the data
			const uint32_t size = *reinterpret_cast<uint32_t*>(buffer);
			buffer += sizeof(uint32_t);

			// Call the function with the data
			func(buffer);
			// Increment the buffer by the size of the data
			buffer += size;
		}

		m_CommandBufferPtr = m_CommandBuffer;
		m_CommandCount = 0;
	}
}