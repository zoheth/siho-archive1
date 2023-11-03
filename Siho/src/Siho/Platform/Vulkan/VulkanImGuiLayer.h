#pragma once

#include "Siho/ImGui/ImguiLayer.h"

namespace Siho {

	class VulkanImGuiLayer : public ImGuiLayer
	{
	public:
		VulkanImGuiLayer();
		VulkanImGuiLayer(const std::string& name);
		~VulkanImGuiLayer() override;

		void Begin() override;
		void End() override;
		void OnAttach() override;
		void OnDetach() override;
		void OnImGuiRender() override;
	private:
		float m_Time = 0.0f;
	};
}