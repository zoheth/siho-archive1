workspace "Siho"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

VULKAN_SDK = os.getenv("VULKAN_SDK")


LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"

-- Include directories relative to root folder
IncludeDir = {}
IncludeDir["GLFW"] = "Siho/vendor/GLFW/include"
IncludeDir["Glad"] = "Siho/vendor/Glad/include"
IncludeDir["ImGui"] = "Siho/vendor/imgui"
IncludeDir["glm"] = "Siho/vendor/glm"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

group "Dependencies"
	include "Siho/vendor/GLFW"
	include "Siho/vendor/Glad"
	include "Siho/vendor/imgui"

group ""

project "Siho"
	location "Siho"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "shpch.h"
	pchsource "Siho/src/shpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",

		"%{IncludeDir.VulkanSDK}",
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib",
		"%{Library.Vulkan}"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"SH_PLATFORM_WINDOWS",
			"SH_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "SH_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "SH_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "SH_DIST"
		runtime "Release"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs
	{
		"Siho/vendor/spdlog/include",
		"Siho/src",
		"Siho/vendor",
		"%{IncludeDir.glm}"
	}

	links
	{
		"Siho"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"SH_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "SH_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "SH_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "SH_DIST"
		runtime "Release"
		optimize "on"