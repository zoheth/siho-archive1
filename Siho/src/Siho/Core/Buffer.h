#pragma once

#include "Siho/Core/Base.h"

namespace Siho {
	
	struct Buffer
	{
		void* Data = nullptr;
		uint32_t Size = 0;

		Buffer() = default;

		Buffer(void* data, uint32_t size)
			: Data(data), Size(size)
		{
		}
	};
}