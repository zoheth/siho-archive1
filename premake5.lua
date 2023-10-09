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

-- Include directories relative to root folder
IncludeDir = {}
IncludeDir["GLFW"] = "Siho/vendor/GLFW/include"
IncludeDir["Glad"] = "Siho/vendor/Glad/include"
IncludeDir["ImGui"] = "Siho/vendor/imgui"
IncludeDir["glm"] = "Siho/vendor/glm"


include "Siho/vendor/GLFW"
include "Siho/vendor/Glad"
include "Siho/vendor/imgui"

project "Siho"
	location "Siho"
	kind "SharedLib"
	language "C++"
	staticruntime "Off"

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

	includedirs
	{
		"%{prj.name}/vendor/spdlog/include",
		"%{prj.name}/src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"SH_PLATFORM_WINDOWS",
			"SH_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
		}

	filter "configurations:Debug"
		defines "SH_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "SH_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "SH_DIST"
		runtime "Release"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	staticruntime "Off"

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
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"SH_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "SH_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "SH_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "SH_DIST"
		runtime "Release"
		optimize "On"