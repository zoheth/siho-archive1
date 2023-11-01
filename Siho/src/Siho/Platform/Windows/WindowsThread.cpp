#include "shpch.h"
#include "Siho/Core/Thread.h"

#include <Windows.h>

namespace Siho {

	Thread::Thread(const std::string& name)
		: m_Name(name)
	{
	}

	void Thread::SetName(const std::string& name)
	{
		HANDLE threadHandle = m_Thread.native_handle();

		std::wstring wName(name.begin(), name.end());
		SetThreadDescription(threadHandle, wName.c_str());
		// Set the thread's affinity to run only on the fourth CPU core.
		// The affinity mask of 8 (binary: 0b1000) allows the thread to run only on the fourth core.
		// This may be useful for performance reasons or to segregate specific tasks to a particular core.
		SetThreadAffinityMask(threadHandle, 8);
	}

	void Thread::Join()
	{
		m_Thread.join();
	}


	ThreadSignal::ThreadSignal(const std::string& name, bool manualReset /*= false*/)
	{
		std::wstring wName(name.begin(), name.end());
		m_SignalHandle = CreateEvent(NULL, manualReset, FALSE, wName.c_str());
	}

	void ThreadSignal::Wait()
	{
		WaitForSingleObject(m_SignalHandle, INFINITE);
	}

	void ThreadSignal::Signal()
	{
		SetEvent(m_SignalHandle);
	}

	void ThreadSignal::Reset()
	{
		ResetEvent(m_SignalHandle);
	}
}