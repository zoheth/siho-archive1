#include "shpch.h"
#include "VulkanSwapChain.h"


// Macro to get a procedure address based on a vulkan instance
#define GET_INSTANCE_PROC_ADDR(inst, entrypoint)                        \
{                                                                       \
	fp##entrypoint = reinterpret_cast<PFN_vk##entrypoint>(vkGetInstanceProcAddr(inst, "vk"#entrypoint)); \
	SH_CORE_ASSERT(fp##entrypoint);                                     \
}

// Macro to get a procedure address based on a vulkan device
#define GET_DEVICE_PROC_ADDR(dev, entrypoint)                           \
{                                                                       \
	fp##entrypoint = reinterpret_cast<PFN_vk##entrypoint>(vkGetDeviceProcAddr(dev, "vk"#entrypoint));   \
	SH_CORE_ASSERT(fp##entrypoint);                                     \
}

namespace
{
	PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR;
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR;
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR;
	PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
	PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
	PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;
	PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR;
	PFN_vkQueuePresentKHR fpQueuePresentKHR;
}

// Nvidia extensions
PFN_vkCmdSetCheckpointNV fpCmdSetCheckpointNV;
PFN_vkGetQueueCheckpointDataNV fpGetQueueCheckpointDataNV;

VKAPI_ATTR void VKAPI_CALL vkCmdSetCheckpointNV(
	VkCommandBuffer                             commandBuffer,
	const void* pCheckpointMarker)
{
	fpCmdSetCheckpointNV(commandBuffer, pCheckpointMarker);
}

namespace Siho {
	void VulkanSwapChain::Init(VkInstance instance, const VulkanDevice& device)
	{
		m_Instance = instance;
		m_Device = device;

		const VkDevice deviceHandle = device->GetHandle();
		
		// In Vulkan, many functions can be dynamically provided by the driver. 
		// The vkGetDeviceProcAddr and vkGetInstanceProcAddr functions enable 
		// applications to retrieve the addresses of these functions, thereby 
		// facilitating their invocation.

		// GET_DEVICE_PROC_ADDR(deviceHandle, CreateSwapchainKHR); Macro expands to:
		// fpCreateSwapchainKHR = reinterpret_cast<PFN_vkCreateSwapchainKHR>(vkGetDeviceProcAddr(deviceHandle, "vkCreateSwapchainKHR"));
		// SH_CORE_ASSERT(fpCreateSwapchainKHR); 
		GET_DEVICE_PROC_ADDR(deviceHandle, CreateSwapchainKHR);
		GET_DEVICE_PROC_ADDR(deviceHandle, DestroySwapchainKHR);
		GET_DEVICE_PROC_ADDR(deviceHandle, GetSwapchainImagesKHR);
		GET_DEVICE_PROC_ADDR(deviceHandle, AcquireNextImageKHR);
		GET_DEVICE_PROC_ADDR(deviceHandle, QueuePresentKHR);

		// GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfaceSupportKHR); Macro expands to:
		// fpGetPhysicalDeviceSurfaceSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceSupportKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceSupportKHR"));
		// SH_CORE_ASSERT(fpGetPhysicalDeviceSurfaceSupportKHR);
		GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfaceSupportKHR);
		GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfaceCapabilitiesKHR);
		GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfaceFormatsKHR);
		GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfacePresentModesKHR);

		GET_INSTANCE_PROC_ADDR(instance, CmdSetCheckpointNV);
		GET_INSTANCE_PROC_ADDR(instance, GetQueueCheckpointDataNV);
	}

	VulkanSwapChain::VulkanSwapChain(VulkanDevice& device)
		:m_Device(device)
	{
	}

	void VulkanSwapChain::InitSurface(GLFWwindow* windowHandle)
	{
		const VkPhysicalDevice physicalDevice = m_Device->GetPhysicalDevice()->GetHandle();

		glfwCreateWindowSurface(m_Instance, windowHandle, nullptr, &m_Surface);

		uint32_t queueCount;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, nullptr);
		SH_CORE_ASSERT(queueCount > 0, "Vulkan: No queue families found!");

		std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, queueFamilyProperties.data());

		// Iterate over each queue to learn whether it supports presenting:
		// Find a queue with present support
		// Will be used to present the swap chain images to the windowing system
		std::vector<VkBool32> supportsPresent(queueCount);
		for (uint32_t i = 0; i < queueCount; i++)
		{
			fpGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, m_Surface, &supportsPresent[i]);
		}

		// Search for a graphics and a present queue in the array of queue
		// families, try to find one that supports both
		uint32_t grapgicsQueueNodeIndex = UINT32_MAX;
		uint32_t presentQueueNodeIndex = UINT32_MAX;
		for (uint32_t i = 0; i < queueCount; i++)
		{
			if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
			{
				if (grapgicsQueueNodeIndex == UINT32_MAX)
				{
					grapgicsQueueNodeIndex = i;
				}

				if (supportsPresent[i] == VK_TRUE)
				{
					grapgicsQueueNodeIndex = i;
					presentQueueNodeIndex = i;
					break;
				}
			}
		}
		if (presentQueueNodeIndex == UINT32_MAX)
		{
			// If there's no queue that supports both present and graphics
			// try to find a separate present queue
			for (uint32_t i = 0; i < queueCount; ++i)
			{
				if (supportsPresent[i] == VK_TRUE)
				{
					presentQueueNodeIndex = i;
					break;
				}
			}
		}
		SH_CORE_ASSERT(grapgicsQueueNodeIndex != UINT32_MAX && presentQueueNodeIndex != UINT32_MAX, "Vulkan: Could not find a graphics and/or presenting queue!");
		
		m_QueueNodeIndex = grapgicsQueueNodeIndex;

		FindImageFormatAndColorSpace();
	}
	/**
	 * Creates a Vulkan swap chain with the specified width, height, and v-sync option.
	 *
	 * The function performs the following crucial steps:
	 * 1. Validates and ensures compatibility between the swap chain setup and the surface.
	 * 2. Selects a suitable present mode with consideration for v-sync, impacting visual output and performance.
	 * 3. Defines and creates the swap chain to manage the images that will be presented on the surface.
	 * 4. Implements synchronization via semaphores and fences to coordinate rendering operations.
	 * 5. Defines and creates a render pass, orchestrating the rendering operations and generating visual output.
	 *
	 * @param width: Pointer to the desired width of the swap chain.
	 * @param height: Pointer to the desired height of the swap chain.
	 * @param vsync: Boolean flag to enable or disable v-sync.
	 */
	void VulkanSwapChain::Create(uint32_t* width, uint32_t* height, bool vsync)
	{
		VkDevice device = m_Device->GetHandle();
		VkPhysicalDevice physicalDevice = m_Device->GetPhysicalDevice()->GetHandle();

		VkSwapchainKHR oldSwapchain = m_SwapChain;

		// Get physical device surface properties and formats
		VkSurfaceCapabilitiesKHR surfCaps;
		VK_CHECK_RESULT(fpGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_Surface, &surfCaps));

		// Get available present modes
		uint32_t presentModeCount;
		VK_CHECK_RESULT(fpGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface, &presentModeCount, nullptr));
		SH_CORE_ASSERT(presentModeCount > 0, "Vulkan: No present modes found!");

		std::vector<VkPresentModeKHR> presentModes(presentModeCount);
		VK_CHECK_RESULT(fpGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface, &presentModeCount, presentModes.data()));

		VkExtent2D swapchainExtent = {};
		// If width (and height) equals the special value 0xFFFFFFFF, the size of the surface will be set by the swapchain
		if (surfCaps.currentExtent.width == static_cast<uint32_t>(-1))
		{
			// If the surface size is undefined, the size is set to
			// the size of the images requested.
			swapchainExtent.width = *width;
			swapchainExtent.height = *height;
		}
		else
		{
			// If the surface size is defined, the swap chain size must match
			swapchainExtent = surfCaps.currentExtent;
			*width = surfCaps.currentExtent.width;
			*height = surfCaps.currentExtent.height;
		}

		m_Width = *width;
		m_Height = *height;

		// Select a present mode for the swapchain

		// The VK_PRESENT_MODE_FIFO_KHR mode must always be present as per spec
		// This mode waits for the vertical blank ("v-sync")
		VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

		// If v-sync is not requested, try to find a mailbox mode
		// It's the lowest latency non-tearing present mode available
		if (!vsync)
		{
			for (size_t i = 0; i < presentModeCount; i++)
			{
				if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
					break;
				}
				if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
				{
					swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
				}
			}
		}

		swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;

		// Determine the number of images
		uint32_t desiredNumberOfSwapchainImages = surfCaps.minImageCount + 1;
		if ((surfCaps.maxImageCount > 0) && (desiredNumberOfSwapchainImages > surfCaps.maxImageCount))
		{
			desiredNumberOfSwapchainImages = surfCaps.maxImageCount;
		}

		// Find the transformation of the surface
		VkSurfaceTransformFlagsKHR preTransform;
		if (surfCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
		{
			// We prefer a non-rotated transform
			preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		}
		else
		{
			preTransform = surfCaps.currentTransform;
		}

		// Find a supported composite alpha format (not all devices support alpha opaque)
		VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		// Simply select the first composite alpha format available
		std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
		};
		for (auto& compositeAlphaFlag : compositeAlphaFlags)
		{
			if (surfCaps.supportedCompositeAlpha & compositeAlphaFlag)
			{
				compositeAlpha = compositeAlphaFlag;
				break;
			};
		}

		VkSwapchainCreateInfoKHR swapchainCI = {};
		swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCI.pNext = nullptr;
		swapchainCI.surface = m_Surface;
		swapchainCI.minImageCount = desiredNumberOfSwapchainImages;
		swapchainCI.imageFormat = m_ColorFormat;
		swapchainCI.imageColorSpace = m_ColorSpace;
		swapchainCI.imageExtent = { swapchainExtent.width, swapchainExtent.height };
		swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchainCI.preTransform = static_cast<VkSurfaceTransformFlagBitsKHR>(preTransform);
		swapchainCI.imageArrayLayers = 1;
		swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainCI.queueFamilyIndexCount = 0;
		swapchainCI.pQueueFamilyIndices = nullptr;
		swapchainCI.presentMode = swapchainPresentMode;
		swapchainCI.oldSwapchain = oldSwapchain;
		// Setting clipped to VK_TRUE allows the implementation to discard rendering outside of the surface area
		swapchainCI.clipped = VK_TRUE;
		swapchainCI.compositeAlpha = compositeAlpha;

		// Enable transfer source on swap chain images if supported
		if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
		{
			swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		}
		// Enable transfer destination on swap chain images if supported
		if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		{
			swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}

		VK_CHECK_RESULT(fpCreateSwapchainKHR(device, &swapchainCI, nullptr, &m_SwapChain));

		// If an existing swap chain is re-created, destroy the old swap chain
		// This also cleans up all the presentable images
		if (oldSwapchain != VK_NULL_HANDLE)
		{
			for (uint32_t i = 0; i < m_ImageCount; i++)
			{
				vkDestroyImageView(device, m_Buffers[i].view, nullptr);
			}
			fpDestroySwapchainKHR(device, oldSwapchain, nullptr);
		}
		VK_CHECK_RESULT(fpGetSwapchainImagesKHR(device, m_SwapChain, &m_ImageCount, nullptr));

		// Get the swap chain images
		m_Images.resize(m_ImageCount);
		VK_CHECK_RESULT(fpGetSwapchainImagesKHR(device, m_SwapChain, &m_ImageCount, m_Images.data()));

		// Get the swap chain buffers containing the image and imageview
		m_Buffers.resize(m_ImageCount);
		for (uint32_t i = 0; i < m_ImageCount; i++)
		{
			VkImageViewCreateInfo colorAttachmentView = {};
			colorAttachmentView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			colorAttachmentView.pNext = nullptr;
			colorAttachmentView.format = m_ColorFormat;
			colorAttachmentView.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G,
												VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
			colorAttachmentView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			colorAttachmentView.subresourceRange.baseMipLevel = 0;
			colorAttachmentView.subresourceRange.levelCount = 1;
			colorAttachmentView.subresourceRange.baseArrayLayer = 0;
			colorAttachmentView.subresourceRange.layerCount = 1;
			colorAttachmentView.viewType = VK_IMAGE_VIEW_TYPE_2D;
			colorAttachmentView.flags = 0;

			m_Buffers[i].image = m_Images[i];

			colorAttachmentView.image = m_Buffers[i].image;

			VK_CHECK_RESULT(vkCreateImageView(device, &colorAttachmentView, nullptr, &m_Buffers[i].view));
		}

		CreateDrawBuffers();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Create synchronization objects
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		// Create a semaphore used to synchronize image presentation
		// Ensures that the image is displayed before we start submitting new commands to the queue
		VK_CHECK_RESULT(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &m_Semaphores.presentComplete));
		// Create a semaphore used to synchronize command submission
		// Ensures that the image is not presented until all commands have been submitted and executed
		VK_CHECK_RESULT(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &m_Semaphores.renderComplete));

		// Set up submit info structure
		// Semaphores will stay the same during application lifetime
		// Command buffer submission info is set by each example
		VkPipelineStageFlags pipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		m_SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		m_SubmitInfo.pWaitDstStageMask = &pipelineStages;
		m_SubmitInfo.waitSemaphoreCount = 1;
		m_SubmitInfo.pWaitSemaphores = &m_Semaphores.presentComplete;
		m_SubmitInfo.signalSemaphoreCount = 1;
		m_SubmitInfo.pSignalSemaphores = &m_Semaphores.renderComplete;

		// Wait fences to sync command buffer access
		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		m_WaitFences.resize(m_DrawCommandBuffers.size());
		for (auto& fence : m_WaitFences)
		{
			VK_CHECK_RESULT(vkCreateFence(device, &fenceCreateInfo, nullptr, &fence));
		}

		// CreateDepthStencil();

		VkFormat depthFormat = m_Device->GetPhysicalDevice()->GetDepthFormat();

		// Render Pass
		std::array<VkAttachmentDescription, 2> attachments = {};
		// Color attachment
		attachments[0].format = m_ColorFormat;
		attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		// Depth attachment
		attachments[1].format = depthFormat;
		attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorReference = {};
		colorReference.attachment = 0;
		colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthReference = {};
		depthReference.attachment = 1;
		depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDescription = {};
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorReference;
		//subpassDescription.pDepthStencilAttachment = &depthReference;
		subpassDescription.inputAttachmentCount = 0;
		subpassDescription.pInputAttachments = nullptr;
		subpassDescription.preserveAttachmentCount = 0;
		subpassDescription.pPreserveAttachments = nullptr;
		subpassDescription.pResolveAttachments = nullptr;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;// static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpassDescription;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VK_CHECK_RESULT(vkCreateRenderPass(m_Device->GetHandle(), &renderPassInfo, nullptr, &m_RenderPass));

		CreateFramebuffer();
	}

	void VulkanSwapChain::OnResize(uint32_t width, uint32_t height)
	{
		SH_CORE_WARN("VulkanContext::OnResize");
		VkDevice device = m_Device->GetHandle();

		vkDeviceWaitIdle(device);

		Create(&width, &height);
		// Recreate the frame buffers
		vkDestroyImageView(device, m_DepthStencil.view, nullptr);
		vkDestroyImage(device, m_DepthStencil.image, nullptr);
		vkFreeMemory(device, m_DepthStencil.mem, nullptr);
		// CreateDepthStencil();

		for (const auto& framebuffer : m_Framebuffers)
		{
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}
		CreateFramebuffer();

		// Command buffers need to be recreated as they may store
		// references to the recreated frame buffer
		vkFreeCommandBuffers(device, m_CommandPool, static_cast<uint32_t>(m_DrawCommandBuffers.size()), m_DrawCommandBuffers.data());
		CreateDrawBuffers();

		vkDeviceWaitIdle(device);
	}

	void VulkanSwapChain::Wait()
	{
		VK_CHECK_RESULT(vkWaitForFences(m_Device->GetHandle(), 1, &m_WaitFences[m_CurrentBufferIndex], VK_TRUE, UINT64_MAX));
	}

	void VulkanSwapChain::BeginFrame()
	{
		//VK_CHECK_RESULT(vkWaitForFences(m_Device->GetHandle(), 1, &m_WaitFences[m_CurrentBufferIndex], VK_TRUE, UINT64_MAX));
		VK_CHECK_RESULT(AcquireNextImage(m_Semaphores.presentComplete, &m_CurrentBufferIndex));
	}

	void VulkanSwapChain::Present()
	{
		constexpr uint64_t DEFAULT_FENCE_TIMEOUT = 100000000000;

		// Use a fence to wait until the command buffer has finished execution before using it again
		VK_CHECK_RESULT(vkResetFences(m_Device->GetHandle(), 1, &m_WaitFences[m_CurrentBufferIndex]));

		// Pipeline stage at which the queue submission will wait (via pWaitSemaphores)
		constexpr VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		// The submit info structure specifies a command buffer queue submission batch
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pWaitDstStageMask = &waitStageMask;
		submitInfo.pWaitSemaphores = &m_Semaphores.presentComplete;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &m_Semaphores.renderComplete;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pCommandBuffers = &m_DrawCommandBuffers[m_CurrentBufferIndex];
		submitInfo.commandBufferCount = 1;

		// Submit to the graphics queue passing a wait fence
		VK_CHECK_RESULT(vkQueueSubmit(m_Device->GetQueue(), 1, &submitInfo, m_WaitFences[m_CurrentBufferIndex]));

		// Present the current buffer to the swap chain
		// Pass the semaphore signaled by the command buffer submission from the submit info as the wait semaphore for swap chain presentation
		// This ensures that the image is not presented to the windowing system until all commands have been submitted
		const VkResult result = QueuePresent(m_Device->GetQueue(), m_CurrentBufferIndex, m_Semaphores.renderComplete);

		if (result != VK_SUCCESS || result == VK_SUBOPTIMAL_KHR)
		{
			if (result == VK_ERROR_OUT_OF_DATE_KHR)
			{
				// Swap chain is no longer compatible with the surface and needs to be recreated
				OnResize(m_Width, m_Height);
				return;
			}
			else
			{
				VK_CHECK_RESULT(result);
			}
		}
	}

	void VulkanSwapChain::Cleanup()
	{
		const VkDevice device = m_Device->GetHandle();

		if (m_SwapChain)
		{
			for (uint32_t i = 0; i < m_ImageCount; i++)
			{
				vkDestroyImageView(device, m_Buffers[i].view, nullptr);
			}
		}
		if (m_Surface)
		{
			fpDestroySwapchainKHR(device, m_SwapChain, nullptr);
			vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		}

		vkDestroyCommandPool(device, m_CommandPool, nullptr);

		m_Surface = VK_NULL_HANDLE;
		m_SwapChain = VK_NULL_HANDLE;
	}

	VkResult VulkanSwapChain::AcquireNextImage(VkSemaphore presentCompleteSemaphore, uint32_t* imageIndex)
	{
		// By setting timeout to UINT64_MAX we will always wait until the next image has been acquired or an actual error is thrown
		// With that we don't have to handle VK_NOT_READY
		return fpAcquireNextImageKHR(m_Device->GetHandle(), m_SwapChain, UINT64_MAX, presentCompleteSemaphore, static_cast<VkFence>(nullptr), imageIndex);
	}

	VkResult VulkanSwapChain::QueuePresent(VkQueue queue, uint32_t imageIndex, VkSemaphore waitSemaphore) const
	{
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_SwapChain;
		presentInfo.pImageIndices = &imageIndex;
		// Check if a wait semaphore has been specified to wait for before presenting the image
		if (waitSemaphore != VK_NULL_HANDLE)
		{
			presentInfo.pWaitSemaphores = &waitSemaphore;
			presentInfo.waitSemaphoreCount = 1;
		}
		return fpQueuePresentKHR(queue, &presentInfo);
	}

	void VulkanSwapChain::CreateFramebuffer()
	{
		// Create frame buffers for every swap chain image
		m_Framebuffers.resize(m_ImageCount);
		for (uint32_t i = 0; i < m_ImageCount; i++)
		{
			std::array<VkImageView, 2> attachments;
			attachments[0] = m_Buffers[i].view;
			attachments[1] = m_DepthStencil.view;

			VkFramebufferCreateInfo frameBufferCreateInfo = {};
			frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			frameBufferCreateInfo.pNext = nullptr;
			frameBufferCreateInfo.renderPass = m_RenderPass;
			frameBufferCreateInfo.attachmentCount = 1;
			frameBufferCreateInfo.pAttachments = attachments.data();
			frameBufferCreateInfo.width = m_Width;
			frameBufferCreateInfo.height = m_Height;
			frameBufferCreateInfo.layers = 1;

			VK_CHECK_RESULT(vkCreateFramebuffer(m_Device->GetHandle(), &frameBufferCreateInfo, nullptr, &m_Framebuffers[i]));
		}
	}
#if 0
	void VulkanSwapChain::CreateDepthStencil()
	{
		VkFormat depthFormat = m_Device->GetPhysicalDevice()->GetDepthFormat();

		VkImageCreateInfo imageCreateInfo = {};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = depthFormat;
		imageCreateInfo.extent = { m_Width, m_Height, 1 };
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

		VkDevice device = m_Device->GetHandle();
		VK_CHECK_RESULT(vkCreateImage(device, &imageCreateInfo, nullptr, &m_DepthStencil.image));
		VkMemoryRequirements memReqs{};
		vkGetImageMemoryRequirements(device, m_DepthStencil.image, &memReqs);
		m_Allocator.Allocate(memReqs, &m_DepthStencil.mem);
		VK_CHECK_RESULT(vkBindImageMemory(device, m_DepthStencil.image, m_DepthStencil.mem, 0));

		VkImageViewCreateInfo imageViewCreateInfo = {};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.image = m_DepthStencil.image;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = depthFormat;
		imageViewCreateInfo.subresourceRange = {};
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		// Stencil aspect should only be set on depth + stencil formats (VK_FORMAT_D16_UNORM_S8_UINT..VK_FORMAT_D32_SFLOAT_S8_UINT
		if (depthFormat >= VK_FORMAT_D16_UNORM_S8_UINT)
			imageViewCreateInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

		VK_CHECK_RESULT(vkCreateImageView(device, &imageViewCreateInfo, nullptr, &m_DepthStencil.view));
	}
#endif

	void VulkanSwapChain::CreateDrawBuffers()
	{
		// Create one command buffer per swap chain image and reuse for rendering
		m_DrawCommandBuffers.resize(m_ImageCount);

		// TODO: Move this somewhere maybe?
		VkCommandPoolCreateInfo cmdPoolInfo = {};
		cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		cmdPoolInfo.queueFamilyIndex = m_QueueNodeIndex;
		cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		VK_CHECK_RESULT(vkCreateCommandPool(m_Device->GetHandle(), &cmdPoolInfo, nullptr, &m_CommandPool));

		VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.commandPool = m_CommandPool;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(m_DrawCommandBuffers.size());
		VK_CHECK_RESULT(vkAllocateCommandBuffers(m_Device->GetHandle(), &commandBufferAllocateInfo, m_DrawCommandBuffers.data()));
	}

	void VulkanSwapChain::FindImageFormatAndColorSpace()
	{
		const VkPhysicalDevice physicalDevice = m_Device->GetPhysicalDevice()->GetHandle();

		// Get list of supported surface formats
		uint32_t formatCount;
		VK_CHECK_RESULT(fpGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &formatCount, nullptr));
		SH_CORE_ASSERT(formatCount > 0, "Vulkan: No surface formats found!");

		std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
		VK_CHECK_RESULT(fpGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &formatCount, surfaceFormats.data()));

		// If the surface format list only includes one entry with VK_FORMAT_UNDEFINED,
		// there is no preferered format, so we assume VK_FORMAT_B8G8R8A8_UNORM
		if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
		{
			m_ColorFormat = VK_FORMAT_B8G8R8A8_UNORM;
			m_ColorSpace = surfaceFormats[0].colorSpace;
		}
		else
		{
			// iterate over the list of available surface format and
			// check for the presence of VK_FORMAT_B8G8R8A8_UNORM
			bool found_B8G8R8A8_UNORM = false;
			for (auto&& surfaceFormat : surfaceFormats)
			{
				if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
				{
					m_ColorFormat = surfaceFormat.format;
					m_ColorSpace = surfaceFormat.colorSpace;
					found_B8G8R8A8_UNORM = true;
					break;
				}
			}

			// in case VK_FORMAT_B8G8R8A8_UNORM is not available
			// select the first available color format
			if (!found_B8G8R8A8_UNORM)
			{
				m_ColorFormat = surfaceFormats[0].format;
				m_ColorSpace = surfaceFormats[0].colorSpace;
			}
		}
	}
}

