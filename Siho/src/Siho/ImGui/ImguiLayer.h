#pragma once

#include "Siho/Layer.h"

#include "Siho/Events/ApplicationEvent.h"
#include "Siho/Events/KeyEvent.h"
#include "Siho/Events/MouseEvent.h"

namespace Siho {

	class SIHO_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;
		
		void Begin();
		void End();
	private:
		float m_Time = 0.0f;
	};

}