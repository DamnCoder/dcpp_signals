/*
 * connection.h
 *
 *  Created on: 9 de oct. de 2016
 *      Author: jorge
 */

#ifndef DC_SIGNALS_CONNECTION_H_
#define DC_SIGNALS_CONNECTION_H_

#import <functional>

namespace dc
{
	// GenericClass is a fake class, ONLY used to provide a type.
	class GenericClass;
	
	// The size of a single inheritance member function pointer.
	const int SINGLE_MEMFUNCPTR_SIZE = sizeof(void (GenericClass::*)());

	template<typename ReturnType> class CConnection;
	
	/**
	 * Base class for connections. Its purpose is to act as an wrapper for all the callbacks
	 */
	template <typename ReturnType, typename... Args>
	class CConnection<ReturnType(Args...)>
	{
	protected:
		// Generic types for functions
		using TMemberFunctionPtr = ReturnType (GenericClass::*)(Args...);
		using TFunctionPtr = ReturnType (*)(Args...);
	
	public:
		CConnection():
			m_pCaller(0),
			m_pMemberFunction(0),
			m_pFunction(0)
		{}
		/*
		template <typename T>
		CConnection(T& ref)
		{
			m_pFunction = reinterpret_cast<TFunctionPtr>(std::addressof(ref));
			Bind(this, &CConnection::InvokeStaticFunctionPtr<T>);
		}
		
		template <typename T>
		CConnection(T* ptr)
		{
			m_pFunction = reinterpret_cast<TFunctionPtr>(ptr);
			Bind(this, &CConnection::InvokeFunctionPtr);
		}
		
		template <typename TInstance, typename TMemberFunction>
		CConnection(TInstance* instance, TMemberFunction function)
		{
			Bind(instance, function);
		}
		*/
		CConnection(const CConnection<ReturnType(Args...)>& copy):
			m_pCaller(copy.m_pCaller),
			m_pMemberFunction(copy.m_pMemberFunction),
			m_pFunction(copy.m_pFunction)
		{}
		
		~CConnection()
		{
			/*
			m_pCaller = 0;
			m_pMemberFunction = 0;
			m_pFunction = 0;
			*/
		}
	public:
		template <typename T>
		inline
		void Bind(T& reference)
		{
			m_pFunction = reinterpret_cast<TFunctionPtr>(std::addressof(reference));
			Bind(this, &CConnection::InvokeStaticFunctionPtr<T>);
		}
		
		template <typename T>
		inline
		void Bind(T* pointer)
		{
			m_pFunction = reinterpret_cast<TFunctionPtr>(pointer);
			Bind(this, &CConnection::InvokeFunctionPtr);
		}
		 
		template <typename TInstance, typename TMemberFunction>
		inline
		void Bind(TInstance* instance, TMemberFunction function)
		{
			// We are casting the type of 'function' to the generic type TMemberFunctionPtr
			// so we don't need to keep the original type TInstance
			m_pMemberFunction = reinterpret_cast<TMemberFunctionPtr>(function);
			m_pCaller = reinterpret_cast<GenericClass *>(instance);
		}
		
		inline
		void Unbind()
		{
			
		}
		
		ReturnType operator() (Args... args) const
		{
			// Here is the reason why we need 'm_pCaller' to be of a generic class type, so it is compatible with the right hand operand '->*'
			return (m_pCaller->*m_pMemberFunction)(args...);
		}
		
		template <typename TInstance, typename TMemberFunction>
		const bool Equals(TInstance* instance, TMemberFunction function) const
		{
			return m_pCaller == reinterpret_cast<GenericClass *>(instance) && m_pMemberFunction == reinterpret_cast<TMemberFunctionPtr>(function) && m_pFunction == 0;
		}
		
		template <typename T>
		const bool Equals(T& reference) const
		{
			return Equals(std::addressof(reference));
		}
		
		template <typename T>
		const bool Equals(T* pointer) const
		{
			return m_pFunction == reinterpret_cast<TFunctionPtr>(pointer);
		}
		
		const bool operator== (const CConnection<ReturnType(Args...)>& connection) const
		{
			if(this == &connection)
				return true;

			return m_pCaller == connection.m_pCaller && m_pFunction == connection.m_pFunction && m_pMemberFunction == connection.m_pMemberFunction;
		}
		
	private:

		template< typename T>
		ReturnType InvokeStaticFunctionPtr(Args... args) const
		{
			return (reinterpret_cast<T*>(m_pFunction)->operator())(args...);
		}

		ReturnType InvokeFunctionPtr(Args... args) const
		{
			// NOTE!! Very important to have the * when calling the m_pFunction
			return (*m_pFunction)(args...);
		}

		// These functions are required for invoking the stored function
		inline
		GenericClass*		CallerPtr() const { return m_pCaller; }
		
		inline
		TMemberFunctionPtr	MemberFunctionPtr() const { return m_pMemberFunction; }
		
	private:
		GenericClass*		m_pCaller;

		TMemberFunctionPtr	m_pMemberFunction;
		TFunctionPtr		m_pFunction;
	};
} /* namespace dc */

#endif /* DC_SIGNALS_CONNECTION_H_ */