#pragma once

#include "Siho/Core/Ref.h"

#include "Vulkan.h"

#include <unordered_set>

namespace Siho {

	class VulkanPhysicalDevice : public RefCounted
	{
	public:
		struct QueueFamilyIndices
		{
			int32_t Graphics = -1;
			int32_t Compute = -1;
			int32_t Transfer = -1;
		};
	public:
		VulkanPhysicalDevice();
		~VulkanPhysicalDevice();

		bool IsExtensionSupported(const std::string& extensionName) const;
		uint32_t GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties) const;

		VkPhysicalDevice GetHandle() const { return m_Handle; }
		const QueueFamilyIndices& GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }
		
		const VkPhysicalDeviceProperties& GetProperties() const { return m_Properties; }
		const VkPhysicalDeviceLimits& GetMemoryLimits() const { return m_Properties.limits; }
		const VkPhysicalDeviceMemoryProperties& GetMemoryProperties() const { return m_MemoryProperties; }

		VkFormat GetDepthFormat() const { return m_DepthFormat; }

		static Ref<VulkanPhysicalDevice> Select();
	private:
		VkFormat FindDepthFormat() const;
		QueueFamilyIndices GetQueueFamilyIndices(int queueFlags) const;
	private:
		QueueFamilyIndices m_QueueFamilyIndices;

		VkPhysicalDevice m_Handle = nullptr;
		VkPhysicalDeviceProperties m_Properties;
		VkPhysicalDeviceFeatures m_Features;
		VkPhysicalDeviceMemoryProperties m_MemoryProperties;

		VkFormat m_DepthFormat = VK_FORMAT_UNDEFINED;

		std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;
		std::unordered_set<std::string> m_SupportedExtensions;
		std::vector<VkDeviceQueueCreateInfo> m_QueueCreateInfos;

		friend class VulkanDevice;
	};

	class VulkanDevice : public RefCounted
	{
	public:
		VulkanDevice(const Ref<VulkanPhysicalDevice>& physicalDevice, VkPhysicalDeviceFeatures enabledFeatures);
		~VulkanDevice();

		void Destory();

		VkQueue GetQueue() { return m_Queue; }
		VkQueue GetComputeQueue() { return m_ComputeQueue; }

		VkCommandBuffer GetCommandBuffer(bool begin, bool compute = false);
		void FlushCommandBuffer(VkCommandBuffer commandBuffer);
		void FlushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue);

		VkCommandBuffer CreateSecondaryCommandBuffer();
		const Ref<VulkanPhysicalDevice>& GetPhysicalDevice() const { return m_PhysicalDevice; }
		VkDevice GetHandle() const { return m_Handle; }

	private:
		VkDevice m_Handle = nullptr;
		Ref<VulkanPhysicalDevice> m_PhysicalDevice;
		VkPhysicalDeviceFeatures m_EnabledFeatures;
		VkCommandPool m_CommandPool, m_ComputeCommandPool;

		VkQueue m_Queue;
		VkQueue m_ComputeQueue;

		// 
		bool m_EnableDebugMarkers = false;
	};
}