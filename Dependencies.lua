-- SihoExternal.lua

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/Siho/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Siho/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/Siho/vendor/imgui"
IncludeDir["glm"] = "%{wks.location}/Siho/vendor/glm"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
IncludeDir["shaderc_util"] = "%{wks.location}/Siho/vendor/shaderc/libshaderc_util/include"
IncludeDir["shaderc"] = "%{wks.location}/Siho/vendor/shaderc/include"

LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
Library["ShaderC_Utils_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_utild.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["ShaderC_Utils_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_util.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"
