#pragma once

#include "stdint.h"

namespace Siho {

	class RefCounted
	{
	public:
		void IncRefCount() const
		{
			m_RefCount++;
		}
		void DecRefCount() const
		{
			m_RefCount--;
		}

		uint32_t GetRefCount() const { return m_RefCount; }
	private:
		mutable uint32_t m_RefCount = 0; // TODO: atomic
	};

	// Ref is a template class designed to manage the lifecycle of objects derived
	// from RefCounted. It takes ownership of RefCounted objects, increments their 
	// reference count when copied, and decrements the reference count when destroyed, 
	// safely deleting the RefCounted object when its reference count reaches zero.
	template<typename T>
	class Ref
	{
	public:
		Ref()
			: m_Instance(nullptr)
		{}

		Ref(std::nullptr_t n)
			: m_Instance(nullptr)
		{}

		Ref(T* instance)
			:m_Instance(instance)
		{
			static_assert(std::is_base_of<RefCounted, T>::value, "T must inherit from RefCounted");

			IncRef();
		}

		Ref(const Ref<T>& other)
		{
			// Copy constructor: Accepts an lvalue reference of `Ref` with the same type `T`,
			m_Instance = other.m_Instance;
			IncRef();
		}

		template<typename T2>
		Ref(const Ref<T2>& other)
		{
			// Copy constructor: Accepts an lvalue reference of `Ref` with a potentially different type `T2`,
			m_Instance = (T*)other.m_Instance;
			IncRef();
		}
		
		template<typename T2>
		Ref(Ref<T2>&& other)
		{
			// Move constructor: Accepts an rvalue reference of `Ref` with a potentially different type `T2`,
			// safely casts and moves the managed instance from `other` to this `Ref`, 
			// and leaves `other` in a valid, empty state.
			m_Instance = (T*)other.m_Instance;
			other.m_Instance = nullptr;
		}

		~Ref()
		{
			DecRef();
		}

		Ref& operator=(std::nullptr_t)
		{
			DecRef();
			m_Instance = nullptr;
			return *this;
		}

		Ref& operator=(const Ref<T>& other)
		{
			other.IncRef();
			DecRef();
			m_Instance = other.m_Instance;
			return *this;
		}

		template<typename T2>
		Ref& operator=(const Ref<T2>& other)
		{
			other.IncRef();
			DecRef();

			m_Instance = other.m_Instance;
			return *this;
		}

		template<typename T2>
		Ref& operator=(Ref<T2>&& other)
		{
			DecRef();

			m_Instance = other.m_Instance;
			other.m_Instance = nullptr;
			return *this;
		}

		operator bool() { return m_Instance != nullptr; }
		operator bool() const { return m_Instance != nullptr; }

		T* operator->() { return m_Instance; }
		const T* operator->() const { return m_Instance; }

		T& operator*() { return *m_Instance; }
		const T& operator*() const { return *m_Instance; }

		T* Raw() { return  m_Instance; }
		const T* Raw() const { return  m_Instance; }

		void Reset(T* instance = nullptr)
		{
			DecRef();
			m_Instance = instance;
		}

		template<typename T2>
		Ref<T2> As() const
		{
			return Ref<T2>(*this);
		}

		// Variadic template arguments allow us to forward arguments to the constructor of T.
		template<typename... Args>
		static Ref<T> Create(Args&&... args)
		{
			return Ref<T>(new T(std::forward<Args>(args)...));
		}

		bool operator==(const Ref<T>& other) const
		{
			return m_Instance == other.m_Instance;
		}

		bool operator!=(const Ref<T>& other) const
		{
			return !(*this == other);
		}

		bool EqualsObject(const Ref<T>& other)
		{
			if (!m_Instance || !other.m_Instance)
				return false;

			return *m_Instance == *other.m_Instance;
		}
	private:
		void IncRef() const
		{
			if (m_Instance)
				m_Instance->IncRefCount();
		}

		void DecRef() const
		{
			if (m_Instance)
			{
				m_Instance->DecRefCount();
				if (m_Instance->GetRefCount() == 0)
				{
					delete m_Instance;
				}
			}
		}
	private:
		// Allows instances of Ref with different template types to interact with each other.
		template<class T2>
		friend class Ref;
		T* m_Instance;
	};
}