#pragma once

#include "shpch.h"

namespace Siho {

	class RenderCommandQueue
	{
	public:
		// RenderCommandFn is a function pointer type representing a render command. 
		// It takes a generic void* argument, akin to C's malloc/free usage of void*.
		typedef void(*RenderCommandFn)(void*);

		RenderCommandQueue();
		~RenderCommandQueue();

		void* Allocate(RenderCommandFn func, uint32_t size);

		void Execute();
	private:
		uint8_t* m_CommandBuffer;
		uint8_t* m_CommandBufferPtr;
		uint32_t m_CommandCount = 0;
	};
}