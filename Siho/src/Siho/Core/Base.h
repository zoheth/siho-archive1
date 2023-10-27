#pragma once

#include <memory>
#include "Ref.h"

#ifdef SH_PLATFORM_WINDOWS
#if SH_DYNAMIC_LINK
	#ifdef SH_BUILD_DLL
		#define SIHO_API __declspec(dllexport)
	#else
		#define SIHO_API __declspec(dllimport)
	#endif
#else
	#define SIHO_API
#endif
#else
	#error Siho only support Windows!
#endif

#define SH_EXPAND_VARGS(x) x

#ifdef SH_DEBUG
	#define SH_ENABLE_ASSERTS
#endif

#ifdef SH_ENABLE_ASSERTS
	#define SH_ASSERT_NO_MESSAGE(condition) { if(!(condition)) { SH_ERROR("Assertion Failed"); __debugbreak(); } }
	#define SH_ASSERT_MESSAGE(condition, ...) { if(!(condition)) { SH_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }

	#define SH_ASSERT_RESOLVE(arg1, arg2, macro, ...) macro
	#define SH_GET_ASSERT_MACRO(...) SH_EXPAND_VARGS(SH_ASSERT_RESOLVE(__VA_ARGS__, SH_ASSERT_MESSAGE, SH_ASSERT_NO_MESSAGE))
	
	#define SH_ASSERT(...) SH_EXPAND_VARGS( SH_GET_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__) )
	#define SH_CORE_ASSERT(...) SH_EXPAND_VARGS( SH_GET_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__) )
#else
	#define SH_ASSERT(x, ...)
	#define SH_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define SH_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)
