#pragma once

#include "Siho/Renderer/Renderer.h"

#include "Vulkan.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"

struct GLFWwindow;

namespace Siho {

	class VulkanContext : public RendererContext
	{
	public:
		VulkanContext() = default;
		~VulkanContext();

		virtual void Init() override;

		void Wait();

		VulkanDevice& GetDevice() const { return *m_Device; }

		static VkInstance GetInstance() { return s_VulkanInstance; }

		static VulkanContext& Get() { return dynamic_cast<VulkanContext&>(Renderer::GetContext()); }
		static VulkanDevice& GetCurrentDevice() { return Get().GetDevice(); }
	private:

		// Devices
		std::unique_ptr<VulkanPhysicalDevice> m_PhysicalDevice;
		std::unique_ptr<VulkanDevice> m_Device;

		// Vulkan instance
		inline static VkInstance s_VulkanInstance;
		VkDebugUtilsMessengerEXT m_DebugUtilsMessenger{ VK_NULL_HANDLE };
		VkPipelineCache m_PipelineCache = nullptr;

	};
}