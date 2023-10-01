#pragma once

#ifdef SH_PLATFORM_WINDOWS
	#ifdef SH_BUILD_DLL
		#define SIHO_API __declspec(dllexport)
	#else
		#define SIHO_API __declspec(dllimport)
	#endif
#else
	#error Siho only support Windows!
#endif
