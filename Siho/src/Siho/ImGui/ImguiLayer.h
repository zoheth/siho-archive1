#pragma once

#include "Siho/Layer.h"

namespace Siho {

	class SIHO_API ImguiLayer : public Layer
	{
	public:
		ImguiLayer();
		~ImguiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate() override;
		virtual void OnEvent(Event& e) override;

	private:
		float m_Time = 0.0f;
	};

}