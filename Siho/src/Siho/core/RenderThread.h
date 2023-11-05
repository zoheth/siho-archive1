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
			Idle = 0, Busy, Kick
		};
	public:
		explicit RenderThread(ThreadingPolicy coreThreadingPolicy);
		~RenderThread();

		RenderThread(const RenderThread&) = delete;
		RenderThread& operator=(const RenderThread&) = delete;
		RenderThread(RenderThread&&) = delete;
		RenderThread& operator=(RenderThread&&) = delete;


		void Run();
		bool IsRunning() const { return m_IsRunning; }
		void Terminate();

		void Wait(State waitForState) const;
		void WaitAndSet(State waitForState, State setToState);
		void Set(State setToState);

		void NextFrame();
		void BlockUntilRenderComplete();
		void Kick();

		void Pump();
	private:
		std::unique_ptr<RenderThreadData> m_Data;
		ThreadingPolicy m_ThreadingPolicy;

		Thread m_RenderThread;

		bool m_IsRunning = false;

		std::atomic<uint32_t> m_AppThreadFrame = 0;
	};
}