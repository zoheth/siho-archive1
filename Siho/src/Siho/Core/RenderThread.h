#pragma once

#include <atomic>

#include "Thread.h"

namespace Siho {

	struct RenderThreadData;

	enum class ThreadingPolicy
	{
		// MultiThreaded will create a Render Thread
		None = 0, SingleThreaded, MultiThreaded
	};

	class RenderThread
	{
	public:
		enum class State
		{
			Idle = 0, Bust, Kick
		};
	public:
		RenderThread(ThreadingPolicy coreThreadingPolicy);
		~RenderThread();

		void Run();
		bool IsRunning() const;
	private:
		std::unique_ptr<RenderThreadData> m_Data;
		ThreadingPolicy m_ThreadingPolicy;

		Thread m_RenderThread;

		bool m_IsRunning = false;

		std::atomic<uint32_t> m_AppThreadFrame = 0;
	};
}