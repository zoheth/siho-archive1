#pragma once
#include "Vulkan.h"

namespace Siho {
	class Device;

	template <typename THandle,
		VkObjectType OBJECT_TYPE,
		typename Device = Siho::Device>
	class VulkanResource {
	public:
		VulkanResource(const VulkanResource&) = delete;
		VulkanResource& operator=(const VulkanResource&) = delete;

	protected:
		THandle m_Handle;
		Device* device;
		std::string debug_name;
	};
}  // namespace Siho