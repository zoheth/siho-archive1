#pragma once

#ifdef SH_PLATFORM_WINDOWS
#include <Windows.h>
#endif

#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <random>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "siho/core/Application.h"
#include "siho/core/Base.h"
#include "siho/core/Log.h"
#include "siho/core/Memory.h"