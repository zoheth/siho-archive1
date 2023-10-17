#include "shpch.h"
#include "VulkanContext.h"

#include <GLFW/glfw3.h>

namespace Siho {
#ifdef SH_DEBUG
	static bool s_Validation = true;
#else
	static bool s_Validation = false;
#endif

	static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugReportCallback(
		VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objType,
		uint64_t obj,
		size_t location,
		int32_t code,
		const char* layerPrefix,
		const char* msg,
		void* userData)
	{
		(void)flags; (void)obj; (void)location; (void)code; (void)userData; (void)layerPrefix; // Unused arguments
		SH_CORE_WARN("VulkanDebugCallback:\n  Object Type: {0}\n  Message: {1}", static_cast<int>(objType), msg);
		return VK_FALSE;
	}

	VulkanContext::VulkanContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		SH_CORE_ASSERT(windowHandle, "Window handle is null!")
	}

	VulkanContext::~VulkanContext()
	{
		m_SwapChain.Cleanup();
		m_Device->Destory();

		vkDestroyInstance(s_VulkanInstance, nullptr);
	}

	void VulkanContext::Create()
	{
		SH_CORE_INFO("Creating Vulkan Context...");

		SH_CORE_ASSERT(glfwVulkanSupported(), "Vulkan is not supported!");

		////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Application info
		////////////////////////////////////////////////////////////////////////////////////////////////////////
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Siho";
		appInfo.pEngineName = "Siho";
		appInfo.engineVersion = VK_API_VERSION_1_3;

		////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Extensions and Validation layers
		////////////////////////////////////////////////////////////////////////////////////////////////////////
#define VK_KHR_WIN32_SURFACE_EXTENSION_NAME "VK_KHR_win32_surface"
		std::vector<const char*> instanceExtensions = {
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME
		};
		if (s_Validation)
		{
			instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
			instanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
		}

		VkInstanceCreateInfo instanceCreateInfo = {};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pNext = nullptr;
		instanceCreateInfo.pApplicationInfo = &appInfo;
		instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
		instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();

		if (s_Validation)
		{
			const char* validationLayerName = "VK_LAYER_KHRONOS_validation";
			uint32_t instanceLayerCount;
			vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
			std::vector<VkLayerProperties> instanceLayerProperties(instanceLayerCount);
			vkEnumerateInstanceLayerProperties(&instanceLayerCount, instanceLayerProperties.data());
			bool validationLayerFound = false;
			SH_CORE_TRACE("Vulkan Instance Layers:");
			for (VkLayerProperties& layerProperty : instanceLayerProperties)
			{
				SH_CORE_TRACE("\t{0}", layerProperty.layerName);
				if (strcmp(layerProperty.layerName, validationLayerName) == 0)
				{
					validationLayerFound = true;
					break;
				}
			}
			if (validationLayerFound)
			{
				instanceCreateInfo.enabledLayerCount = 1;
				instanceCreateInfo.ppEnabledLayerNames = &validationLayerName;
			}
			else
			{
				SH_CORE_ERROR("Validation layer VK_LAYER_KHRONOS_validation not found!");
			}
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Instance and Surface Creation
		////////////////////////////////////////////////////////////////////////////////////////////////////////
		VK_CHECK_RESULT(vkCreateInstance(&instanceCreateInfo, nullptr, &s_VulkanInstance));

		if (s_Validation)
		{
			auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(s_VulkanInstance, "vkCreateDebugReportCallbackEXT");
			SH_CORE_ASSERT(vkCreateDebugReportCallbackEXT, "Could not load vkCreateDebugReportCallbackEXT");
			VkDebugReportCallbackCreateInfoEXT debugReportCallbackCreateInfo = {};
			debugReportCallbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
			debugReportCallbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
			debugReportCallbackCreateInfo.pfnCallback = VulkanDebugReportCallback;
			debugReportCallbackCreateInfo.pUserData = nullptr;
			VK_CHECK_RESULT(vkCreateDebugReportCallbackEXT(s_VulkanInstance, &debugReportCallbackCreateInfo, nullptr, &m_DebugReportCallback));
		}

		m_PhysicalDevice = VulkanPhysicalDevice::Select();

		VkPhysicalDeviceFeatures enabledFeatures;
		memset(&enabledFeatures, 0, sizeof(VkPhysicalDeviceFeatures));
		enabledFeatures.samplerAnisotropy = VK_TRUE;
		enabledFeatures.robustBufferAccess = VK_TRUE;

		m_Device = Ref<VulkanDevice>::Create(m_PhysicalDevice, enabledFeatures);

		m_SwapChain.Init(s_VulkanInstance, m_Device);
		m_SwapChain.InitSurface(m_WindowHandle);

		uint32_t width = 1280, height = 720;
		m_SwapChain.Create(&width, &height);

		// Pipeline Cache
		VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
		pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		VK_CHECK_RESULT(vkCreatePipelineCache(m_Device->GetHandle(), &pipelineCacheCreateInfo, nullptr, &m_PipelineCache));
	}

	void VulkanContext::SwapBuffers()
	{
		m_SwapChain.Present();
	}

	void VulkanContext::OnResize(uint32_t width, uint32_t height)
	{
		m_SwapChain.OnResize(width, height);
	}

	void VulkanContext::Wait()
	{
		m_SwapChain.Wait();
	}

	void VulkanContext::BeginFrame()
	{
		m_SwapChain.BeginFrame();
	}

}
