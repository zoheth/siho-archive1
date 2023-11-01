#include "shpch.h"
#include "RenderThread.h"

namespace Siho {

	struct RenderThreadData
	{
		// Critical section for thread synchronization. Use with EnterCriticalSection() & LeaveCriticalSection().
		CRITICAL_SECTION m_CriticalSection;

		// Condition variable for thread coordination. Typically used with SleepConditionVariableCS() for waiting and WakeAllConditionVariable() for signaling.
		CONDITION_VARIABLE m_ConditionVariable;

		RenderThread::State m_State = RenderThread::State::Idle;
	};

	RenderThread::RenderThread(ThreadingPolicy threadingPolicy)
		:m_RenderThread("Render Thread"), m_ThreadingPolicy(threadingPolicy)
	{
		m_Data = std::make_unique<RenderThreadData>();

		if (m_ThreadingPolicy == ThreadingPolicy::MultiThreaded)
		{
			InitializeCriticalSection(&m_Data->m_CriticalSection);
			InitializeConditionVariable(&m_Data->m_ConditionVariable);
		}
	}

	RenderThread::~RenderThread()
	{
		if (m_ThreadingPolicy == ThreadingPolicy::MultiThreaded)
		{
			DeleteCriticalSection(&m_Data->m_CriticalSection);
		}
	}

	void RenderThread::Run()
	{
		m_IsRunning = true;
		if (m_ThreadingPolicy == ThreadingPolicy::MultiThreaded)
			m_RenderThread.Dispatch(Renderer::RenderThreadFunc)
	}
}