#pragma once

#include "siho/core/Base.h"

namespace Siho {
	
	struct Buffer
	{
		void* Data = nullptr;
		uint64_t Size = 0;

		Buffer() = default;

		Buffer(void* data, uint64_t size)
			: Data(data), Size(size)
		{
		}

		static Buffer CreateFromCopy(void* data, uint64_t size)
		{
			Buffer buffer;
			buffer.Allocate(size);
			memcpy(buffer.Data, data, size);
			return buffer;
		}

		void Allocate(uint64_t size)
		{
			delete[](byte*)Data;
			Data = nullptr;

			if (size == 0)
				return;

			Data = hnew byte[size];
			Size = size;
		}

		void Release()
		{
			delete[](byte*)Data;
			Data = nullptr;
			Size = 0;
		}

		void ZeroInitialize()
		{
			if (Data)
				memset(Data, 0, Size);
		}

		template<typename T>
		T& Read(uint64_t offset = 0) const
		{
			SH_CORE_ASSERT(offset + sizeof(T) <= Size, "Buffer overflow!");
			return *(T*)((byte*)Data + offset);
		}

		byte* ReadBytes(uint64_t size, uint64_t offset) const
		{
			SH_CORE_ASSERT(offset + size <= Size, "Buffer overflow!");
			byte* buffer = hnew byte[size];
			memcpy(buffer, (byte*)Data + offset, size);
			return buffer;
		}

		void Write(const void* data, uint64_t size, uint64_t offset = 0)
		{
			SH_CORE_ASSERT(offset + size <= Size, "Buffer overflow!");
			memcpy((byte*)Data + offset, data, size);
		}

		operator bool() const { return Data; }

		byte& operator[](uint64_t index)
		{
			SH_CORE_ASSERT(index < Size, "Buffer overflow!");
			return ((byte*)Data)[index];
		}

		byte& operator[](uint64_t index) const
		{
			SH_CORE_ASSERT(index < Size, "Buffer overflow!");
			return ((byte*)Data)[index];
		}

		template<typename T>
		T* As() const 
		{
			return (T*)Data;
		}
	};


}