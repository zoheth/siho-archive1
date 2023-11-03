#pragma once

namespace Siho {

	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type);

	/**
	 * Represents a single attribute within a vertex in a vertex buffer.
	 *
	 * In graphics programming, a vertex can have multiple attributes such as position, color, normals, texture coordinates, etc.
	 * Each VertexBufferElement describes the type, size, and layout of one of these attributes within the buffer.
	 *
	 * For example, an element might describe the position of a vertex using three floats (Float3), or the color using four floats (Float4).
	 */
	struct VertexBufferElement
	{
		std::string Name;
		ShaderDataType Type;
		uint32_t Size;
		uint32_t Offset;	// Byte offset of the attribute within a single vertex structure.
		bool Normalized;	// Whether the data should be normalized (typically used for colors or normals).

		VertexBufferElement() = default;

		VertexBufferElement(ShaderDataType type, std::string name, bool normalized = false);

		uint32_t GetComponentCount() const;
	};

	class VertexBufferLayout
	{
	public:
		VertexBufferLayout() = default;

		VertexBufferLayout(const std::initializer_list<VertexBufferElement>& elements)
			: m_Elements(elements)
		{
			CalculateOffsetsAndStride();
		}

		uint32_t GetStride() const { return m_Stride; }
		const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; }
		uint32_t GetElementCount() const { return static_cast<uint32_t>(m_Elements.size()); }

		[[nodiscard]] std::vector<VertexBufferElement>::iterator begin() { return m_Elements.begin(); }
		[[nodiscard]] std::vector<VertexBufferElement>::iterator end() { return m_Elements.end(); }
		[[nodiscard]] std::vector<VertexBufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		[[nodiscard]] std::vector<VertexBufferElement>::const_iterator end() const { return m_Elements.end(); }
	private:
		void CalculateOffsetsAndStride()
		{
			uint32_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}
	private:
		std::vector<VertexBufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};

	enum class VertexBufferUsage
	{
		None = 0, Static = 1, Dynamic = 2
	};

	class VertexBuffer
	{
	public:

		virtual void SetData(void* buffer, uint32_t size, uint32_t offset = 0) = 0;
		virtual void RenderThread_SetData(void* buffer, uint32_t size, uint32_t offset = 0) = 0;
		virtual void Bind() const = 0;

		virtual uint64_t GetSize() const = 0;
		virtual uint32_t GetRendererId() const = 0;

		static std::shared_ptr<VertexBuffer> Create(void* data, uint32_t size, VertexBufferUsage usage = VertexBufferUsage::Static);
		static std::shared_ptr<VertexBuffer> Create(uint32_t size, VertexBufferUsage usage = VertexBufferUsage::Dynamic);
	};
}