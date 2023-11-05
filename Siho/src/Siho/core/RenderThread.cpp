#include "shpch.h"
#include "RenderThread.h"

#include "siho/renderer/Renderer.h"

namespace Siho {

	struct RenderThreadData
	{
		// Critical section for thread synchronization. Use with EnterCriticalSection() & LeaveCriticalSection().
		CRITICAL_SECTION CriticalSection;

		// Condition variable for thread coordination. Typically used with SleepConditionVariableCS() for waiting and WakeAllConditionVariable() for signaling.
		CONDITION_VARIABLE ConditionVariable;

		RenderThread::State State = RenderThread::State::Idle;
	};

	RenderThread::RenderThread(ThreadingPolicy threadingPolicy)
		:m_ThreadingPolicy(threadingPolicy), m_RenderThread("Render Thread")
	{
		m_Data = std::make_unique<RenderThreadData>();

		if (m_ThreadingPolicy == ThreadingPolicy::MultiThreaded)
		{
			InitializeCriticalSection(&m_Data->CriticalSection);
			InitializeConditionVariable(&m_Data->ConditionVariable);
		}
	}

	RenderThread::~RenderThread()
	{
		if (m_ThreadingPolicy == ThreadingPolicy::MultiThreaded)
		{
			DeleteCriticalSection(&m_Data->CriticalSection);
		}
	}

	void RenderThread::Run()
	{
		m_IsRunning = true;
		if (m_ThreadingPolicy == ThreadingPolicy::MultiThreaded)
			m_RenderThread.Dispatch(Renderer::RenderThreadFunc, this);
	}

	void RenderThread::Terminate()
	{
		m_IsRunning = false;
		Pump();

		if (m_ThreadingPolicy == ThreadingPolicy::MultiThreaded)
			m_RenderThread.Join();
	}

	void RenderThread::Wait(State waitForState) const
	{
		if (m_ThreadingPolicy == ThreadingPolicy::SingleThreaded)
			return;
		EnterCriticalSection(&m_Data->CriticalSection);
		while (m_Data->State != waitForState)
		{
			SleepConditionVariableCS(&m_Data->ConditionVariable, &m_Data->CriticalSection, INFINITE);
		}
		LeaveCriticalSection(&m_Data->CriticalSection);
	}

	void RenderThread::WaitAndSet(State waitForState, State setToState)
	{
	}

	void RenderThread::Set(State setToState)
	{
	}

	void RenderThread::NextFrame()
	{
	}

	void RenderThread::BlockUntilRenderComplete()
	{
	}

	void RenderThread::Kick()
	{
	}

	void RenderThread::Pump()
	{
	}
}
