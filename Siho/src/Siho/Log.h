#pragma once

#include <memory>

#include "Core.h"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Siho {
	class SIHO_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

#define SH_CORE_TRACE(...)	::Siho::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define SH_CORE_INFO(...)	::Siho::Log::GetCoreLogger()->info(__VA_ARGS__)
#define SH_CORE_WARN(...)	::Siho::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define SH_CORE_ERROR(...)	::Siho::Log::GetCoreLogger()->error(__VA_ARGS__)
#define SH_CORE_FATAL(...)	::Siho::Log::GetCoreLogger()->fatal(__VA_ARGS__)

#define SH_TRACE(...)		::Siho::Log::GetClientLogger()->trace(__VA_ARGS__)
#define SH_INFO(...)		::Siho::Log::GetClientLogger()->info(__VA_ARGS__)
#define SH_WARN(...)		::Siho::Log::GetClientLogger()->warn(__VA_ARGS__)
#define SH_ERROR(...)		::Siho::Log::GetClientLogger()->error(__VA_ARGS__)
#define SH_FATAL(...)		::Siho::Log::GetClientLogger()->fatal(__VA_ARGS__)
