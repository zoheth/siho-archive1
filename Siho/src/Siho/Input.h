#pragma once

#include "Siho/Core.h"

namespace Siho {

	class SIHO_API Input
	{
	public:
		static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }
	protected:
		virtual bool IsKeyPressedImpl(int keycode) = 0;
	private:
		static Input* s_Instance;
	};
}