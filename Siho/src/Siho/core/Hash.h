#pragma once

#include <string>

namespace Siho {

	class Hash
	{
	public:
		static constexpr uint32_t GenerateFNVHash(std::string_view str)
		{
			constexpr uint32_t FNV_prime = 16777619u;
			constexpr uint32_t FNV_offset_basis = 2166136261u;

			const size_t length = str.length();
			const char* data = str.data();

			uint32_t hash = FNV_offset_basis;
			for (size_t i = 0; i < length; i++)
			{
				hash ^= *data++;
				hash *= FNV_prime;
			}
			hash ^= '\0';
			hash *= FNV_prime;

			return hash;
		}
	};
}