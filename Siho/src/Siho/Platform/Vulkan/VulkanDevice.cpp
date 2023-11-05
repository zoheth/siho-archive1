#include "shpch.h"
#include "VulkanDevice.h"

#include "VulkanContext.h"

namespace Siho {

	VulkanPhysicalDevice::VulkanPhysicalDevice()
	{
		const auto vkInstance = VulkanContext::GetInstance();

		uint32_t gpuCount = 0;
		vkEnumeratePhysicalDevices(vkInstance, &gpuCount, nullptr);
		SH_CORE_ASSERT(gpuCount > 0, "");
		// Enumerate devices
		std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
		VK_CHECK_RESULT(vkEnumeratePhysicalDevices(vkInstance, &gpuCount, physicalDevices.data()));

		VkPhysicalDevice selectedDevice = VK_NULL_HANDLE;
		// Select discrete GPU if available, otherwise integrated GPU
		for (const VkPhysicalDevice physicalDevice : physicalDevices)
		{
			vkGetPhysicalDeviceProperties(physicalDevice, &m_Properties);
			if (m_Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				selectedDevice = physicalDevice;
				break;
			}
		}

		if (selectedDevice == VK_NULL_HANDLE)
		{
			SH_CORE_TRACE("Could not find a discrete GPU!");
			selectedDevice = physicalDevices.back();
		}

		SH_CORE_ASSERT(selectedDevice != VK_NULL_HANDLE, "Could not find a suitable GPU!");
		m_Handle = selectedDevice;

		vkGetPhysicalDeviceFeatures(m_Handle, &m_Features);
		vkGetPhysicalDeviceMemoryProperties(m_Handle, &m_MemoryProperties);

		uint32_t queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(m_Handle, &queueFamilyCount, nullptr);
		SH_CORE_ASSERT(queueFamilyCount > 0, "Could not find a queue family with at least one queue!");
		m_QueueFamilyProperties.resize(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(m_Handle, &queueFamilyCount, m_QueueFamilyProperties.data());

		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(m_Handle, nullptr, &extensionCount, nullptr);
		if (extensionCount > 0)
		{
			std::vector<VkExtensionProperties> extensions(extensionCount);
			if (vkEnumerateDeviceExtensionProperties(m_Handle, nullptr, &extensionCount, &extensions.front()) == VK_SUCCESS)
			{
				SH_CORE_TRACE("Selected physical device has {0} extensions:", extensionCount);
				for (auto& extension : extensions)
				{
					m_SupportedExtensions.insert(extension.extensionName);
					SH_CORE_TRACE("\t{0}", extension.extensionName);
				}
			}
		}

		// Queue families
		// Desired queues need to be requested upon logical device creation
		// Due to differing queue family configurations of Vulkan implementations this can be a bit tricky, especially if the application
		// requests different queue types

		// Get queue family indices for graphics and compute
		// Note that the indices may overlap depending on the implementation

		static constexpr float defaultQueuePriority(0.0f);

		int requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
		m_QueueFamilyIndices = GetQueueFamilyIndices(requestedQueueTypes);

		// Graphics queue
		if (requestedQueueTypes & VK_QUEUE_TRANSFER_BIT)
		{
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = m_QueueFamilyIndices.Graphics;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &defaultQueuePriority;
			m_QueueCreateInfos.push_back(queueCreateInfo);
		}

		// Dedicated compute queue
		if (requestedQueueTypes & VK_QUEUE_COMPUTE_BIT)
		{
			if (m_QueueFamilyIndices.Compute != m_QueueFamilyIndices.Graphics)
			{
				// If compute family index differs, we need an additional queue create info for the compute queue
				VkDeviceQueueCreateInfo queueCreateInfo = {};
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfo.queueFamilyIndex = m_QueueFamilyIndices.Compute;
				queueCreateInfo.queueCount = 1;
				queueCreateInfo.pQueuePriorities = &defaultQueuePriority;
				m_QueueCreateInfos.push_back(queueCreateInfo);
			}
		}

		// Dedicated transfer queue
		if (requestedQueueTypes & VK_QUEUE_TRANSFER_BIT)
		{
			if ((m_QueueFamilyIndices.Transfer != m_QueueFamilyIndices.Graphics) && (m_QueueFamilyIndices.Transfer != m_QueueFamilyIndices.Compute))
			{
				// If compute family index differs, we need an additional queue create info for the compute queue
				VkDeviceQueueCreateInfo queueCreateInfo = {};
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfo.queueFamilyIndex = m_QueueFamilyIndices.Transfer;
				queueCreateInfo.queueCount = 1;
				queueCreateInfo.pQueuePriorities = &defaultQueuePriority;
				m_QueueCreateInfos.push_back(queueCreateInfo);
			}
		}

		m_DepthFormat = FindDepthFormat();
		SH_CORE_ASSERT(m_DepthFormat != VK_FORMAT_UNDEFINED, "Depth format not supported!");
	}
	VulkanPhysicalDevice::~VulkanPhysicalDevice()
	{
	}

	bool VulkanPhysicalDevice::IsExtensionSupported(const std::string& extensionName) const
	{
		return m_SupportedExtensions.find(extensionName) != m_SupportedExtensions.end();
	}

	uint32_t VulkanPhysicalDevice::GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties) const
	{
		// Iterate over all memory types available for the device used in this example
		for (uint32_t i = 0; i < m_MemoryProperties.memoryTypeCount; i++)
		{
			if ((typeBits & 1) == 1)
			{
				if ((m_MemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
				{
					return i;
				}
			}
			typeBits >>= 1;
		}

		SH_CORE_ASSERT(false, "Could not find a suitable memory type!");
		return UINT32_MAX;
	}

	std::unique_ptr<VulkanPhysicalDevice> VulkanPhysicalDevice::Select()
	{
		return std::make_unique<VulkanPhysicalDevice>();
	}

	VkFormat VulkanPhysicalDevice::FindDepthFormat() const
	{
		// Since all depth formats may be optional, we need to find a suitable depth format to use
		// Start with the highest precision packed format
		const std::vector<VkFormat> depthFormats = { 
			VK_FORMAT_D32_SFLOAT_S8_UINT, 
			VK_FORMAT_D32_SFLOAT, 
			VK_FORMAT_D24_UNORM_S8_UINT, 
			VK_FORMAT_D16_UNORM_S8_UINT, 
			VK_FORMAT_D16_UNORM };

		// TODO: Move to VulkanPhysicalDevice
		for (auto& format : depthFormats)
		{
			VkFormatProperties formatProps;
			vkGetPhysicalDeviceFormatProperties(m_Handle, format, &formatProps);
			// Format must support depth stencil attachment for optimal tiling
			if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
				return format;
		}
		return VK_FORMAT_UNDEFINED;
	}

	VulkanPhysicalDevice::QueueFamilyIndices VulkanPhysicalDevice::GetQueueFamilyIndices(int flags) const
	{
		QueueFamilyIndices indices;

		// Dedicated queue for compute
		// Try to find a queue family index that supports compute but not graphics
		if (flags & VK_QUEUE_COMPUTE_BIT)
		{
			for (uint32_t i = 0; i < static_cast<uint32_t>(m_QueueFamilyProperties.size()); i++)
			{
				if ((m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) && ((m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
				{
					indices.Compute = i;
					break;
				}
			}
		}

		// Dedicated queue for transfer
		// Try to find a queue family index that supports transfer but not graphics and compute
		if (flags & VK_QUEUE_TRANSFER_BIT)
		{
			for (uint32_t i = 0; i < static_cast<uint32_t>(m_QueueFamilyProperties.size()); i++)
			{
				if ((m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) && 
					((m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) && 
					((m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
				{
					indices.Transfer = i;
					break;
				}
			}
		}

		// For other queue types or if no separate compute queue is present, return the same queue family index
		if (indices.Compute == -1)
		{
			for (uint32_t i = 0; i < static_cast<uint32_t>(m_QueueFamilyProperties.size()); i++)
			{
				if ((m_QueueFamilyProperties[i].queueFlags & flags) == flags)
				{
					indices.Compute = i;
					break;
				}
			}
		}

		// Get first index with graphics queue family property
		if (flags & VK_QUEUE_GRAPHICS_BIT)
		{
			for (uint32_t i = 0; i < static_cast<uint32_t>(m_QueueFamilyProperties.size()); i++)
			{
				if (m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					indices.Graphics = i;
					break;
				}
			}
		}

		return indices;
	}

	VulkanDevice::VulkanDevice(VulkanPhysicalDevice& physicalDevice, VkPhysicalDeviceFeatures enabledFeatures)
		: m_PhysicalDevice(physicalDevice), m_EnabledFeatures(enabledFeatures)
	{
		std::vector<const char*> deviceExtensions;

		SH_CORE_ASSERT(m_PhysicalDevice.IsExtensionSupported(VK_KHR_SWAPCHAIN_EXTENSION_NAME), "Physical device does not support VK_KHR_swapchain extension!");
		deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		if (m_PhysicalDevice.IsExtensionSupported(VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME))
			deviceExtensions.push_back(VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME);

		deviceExtensions.push_back(VK_KHR_MAINTENANCE1_EXTENSION_NAME);

		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(m_PhysicalDevice.m_QueueCreateInfos.size());
		deviceCreateInfo.pQueueCreateInfos = m_PhysicalDevice.m_QueueCreateInfos.data();
		deviceCreateInfo.pEnabledFeatures = &m_EnabledFeatures;

		// VkPhysicalDeviceFeatures2 allows Vulkan to introduce new device features without altering the API.
		// It holds certain features, and through a pointer (pNext), it can be linked to additional feature structures.
		VkPhysicalDeviceFeatures2 physicalDeviceFeatures2 = {};

		if (m_PhysicalDevice.IsExtensionSupported(VK_EXT_DEBUG_MARKER_EXTENSION_NAME))
		{
			deviceExtensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
			m_EnableDebugMarkers = true;
		}

		if (!deviceExtensions.empty())
		{
			deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
			deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
		}

		VK_CHECK_RESULT(vkCreateDevice(m_PhysicalDevice.m_Handle, &deviceCreateInfo, nullptr, &m_Handle));

		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.queueFamilyIndex = m_PhysicalDevice.m_QueueFamilyIndices.Graphics;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		VK_CHECK_RESULT(vkCreateCommandPool(m_Handle, &commandPoolCreateInfo, nullptr, &m_CommandPool));

		commandPoolCreateInfo.queueFamilyIndex = m_PhysicalDevice.m_QueueFamilyIndices.Compute;
		VK_CHECK_RESULT(vkCreateCommandPool(m_Handle, &commandPoolCreateInfo, nullptr, &m_ComputeCommandPool));

		vkGetDeviceQueue(m_Handle, m_PhysicalDevice.m_QueueFamilyIndices.Graphics, 0, &m_Queue);
		vkGetDeviceQueue(m_Handle, m_PhysicalDevice.m_QueueFamilyIndices.Compute, 0, &m_ComputeQueue);
	}

	VulkanDevice::~VulkanDevice()
	{
	}

	void VulkanDevice::Destory()
	{
		vkDestroyCommandPool(m_Handle, m_CommandPool, nullptr);
		vkDestroyCommandPool(m_Handle, m_ComputeCommandPool, nullptr);

		vkDeviceWaitIdle(m_Handle);
		vkDestroyDevice(m_Handle, nullptr);
	}
	VkCommandBuffer VulkanDevice::GetCommandBuffer(bool begin, bool compute)
	{
		VkCommandBuffer cmdBuffer;

		VkCommandBufferAllocateInfo cmdBufAllocateInfo = {};
		cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdBufAllocateInfo.commandPool = compute ? m_ComputeCommandPool : m_CommandPool;
		cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cmdBufAllocateInfo.commandBufferCount = 1;

		VK_CHECK_RESULT(vkAllocateCommandBuffers(m_Handle, &cmdBufAllocateInfo, &cmdBuffer));

		// If requested, also start the new command buffer
		if (begin)
		{
			VkCommandBufferBeginInfo cmdBufferBeginInfo{};
			cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			VK_CHECK_RESULT(vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo));
		}

		return cmdBuffer;
	}
	void VulkanDevice::FlushCommandBuffer(VkCommandBuffer commandBuffer)
	{
		FlushCommandBuffer(commandBuffer, m_Queue);
	}

	void VulkanDevice::FlushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue)
	{
		const uint64_t DEFAULT_FENCE_TIMEOUT = 100000000000;

		SH_CORE_ASSERT(commandBuffer != VK_NULL_HANDLE);

		VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffer));

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		// Create fence to ensure that the command buffer has finished executing
		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = 0;
		VkFence fence;
		VK_CHECK_RESULT(vkCreateFence(m_Handle, &fenceCreateInfo, nullptr, &fence));

		{
			static std::mutex submissionLock;
			std::scoped_lock<std::mutex> lock(submissionLock);

			// Submit to the queue
			VK_CHECK_RESULT(vkQueueSubmit(queue, 1, &submitInfo, fence));
		}

		// Wait for the fence to signal that command buffer has finished executing
		VK_CHECK_RESULT(vkWaitForFences(m_Handle, 1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT));

		vkDestroyFence(m_Handle, fence, nullptr);
		vkFreeCommandBuffers(m_Handle, m_CommandPool, 1, &commandBuffer);
	}

	VkCommandBuffer VulkanDevice::CreateSecondaryCommandBuffer()
	{
		VkCommandBuffer cmdBuffer;

		VkCommandBufferAllocateInfo cmdBufAllocateInfo = {};
		cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdBufAllocateInfo.commandPool = m_CommandPool;
		cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
		cmdBufAllocateInfo.commandBufferCount = 1;

		VK_CHECK_RESULT(vkAllocateCommandBuffers(m_Handle, &cmdBufAllocateInfo, &cmdBuffer));
		return cmdBuffer;
	}

}
