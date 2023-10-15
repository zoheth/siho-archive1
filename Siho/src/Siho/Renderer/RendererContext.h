#pragma once

#include "Siho/Core/Ref.h"

namespace Siho {

	class RendererContext : public RefCounted
	{
	public:
		virtual void Create() = 0;
		virtual void SwapBuffers() = 0;
	};
}