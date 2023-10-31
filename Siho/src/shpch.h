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

#include "Siho/Core/Application.h"
#include "Siho/Core/Base.h"
#include "Siho/Core/Log.h"
#include "Siho/Core/Memory.h"