#include "shpch.h"
#include "VertexBuffer.h"

#include "Siho/Renderer/RendererAPI.h"
#include "Siho/Platform/Vulkan/VulkanVertexBuffer.h"

namespace Siho {

	inline static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:    return 4;
		case ShaderDataType::Float2:   return 4 * 2;
		case ShaderDataType::Float3:   return 4 * 3;
		case ShaderDataType::Float4:   return 4 * 4;
		case ShaderDataType::Mat3:     return 4 * 3 * 3;
		case ShaderDataType::Mat4:     return 4 * 4 * 4;
		case ShaderDataType::Int:      return 4;
		case ShaderDataType::Int2:     return 4 * 2;
		case ShaderDataType::Int3:     return 4 * 3;
		case ShaderDataType::Int4:     return 4 * 4;
		case ShaderDataType::Bool:     return 1;
		}

		SH_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	VertexBufferElement::VertexBufferElement(ShaderDataType type, const std::string& name, bool normalized /*= false*/)
		: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
	{
	}

	inline uint32_t VertexBufferElement::GetComponentCount() const
	{
		switch (Type)
		{
		case ShaderDataType::Float:   return 1;
		case ShaderDataType::Float2:  return 2;
		case ShaderDataType::Float3:  return 3;
		case ShaderDataType::Float4:  return 4;
		case ShaderDataType::Mat3:    return 3 * 3;
		case ShaderDataType::Mat4:    return 4 * 4;
		case ShaderDataType::Int:     return 1;
		case ShaderDataType::Int2:    return 2;
		case ShaderDataType::Int3:    return 3;
		case ShaderDataType::Int4:    return 4;
		case ShaderDataType::Bool:    return 1;
		}

		SH_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}


	Ref<VertexBuffer> VertexBuffer::Create(void* data, uint32_t size, VertexBufferUsage usage /*= VertexBufferUsage::Static*/)
	{
		switch (RendererAPI::Current())
		{
		case  RendererAPIType::None: return nullptr;
		case RendererAPIType::Vulkan: return Ref<VulkanVertexBuffer>::Create(data, size, usage);
		}
		SH_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size, VertexBufferUsage usage /*= VertexBufferUsage::Dynamic*/)
	{
		switch (RendererAPI::Current())
		{
		case RendererAPIType::None:    return nullptr;
		case RendererAPIType::Vulkan:  return Ref<VulkanVertexBuffer>::Create(size, usage);
		}
		SH_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

}