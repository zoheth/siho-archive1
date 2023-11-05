#pragma once

#include <string>
#include <thread>


namespace Siho {
	// This header defines the cross-platform interface for threads and thread signals.
	// The platform-specific implementation for Windows can be found in WindowsThread.cpp.
	class Thread
	{
	public:
		Thread(std::string name);

		template<typename Function, typename... Args>
		void Dispatch(Function&& function, Args&&... args)
		{
			m_Thread = std::thread(function, args...);
			SetName(m_Name);
		}

		void SetName(const std::string& name);

		void Join();
	private:
		std::string m_Name;
		std::thread m_Thread;
	};

	class ThreadSignal
	{
	public:
		ThreadSignal(const std::string&name, bool manualReset = false);

		void Wait() const;
		void Signal() const;
		void Reset() const;
	private:
		void* m_SignalHandle = nullptr;
	};
}