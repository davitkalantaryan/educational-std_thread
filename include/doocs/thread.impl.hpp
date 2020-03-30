// 
// file:			doocs/thread.hpp
// created on:		2020 Mar 30
// created by:		D. Kalantaryan
//

#ifndef DOOCS_THREAD_IMPL_HPP
#define DOOCS_THREAD_IMPL_HPP
#ifdef _WIN32

#ifndef DOOCS_THREAD_HPP
#ifdef __INTELLISENSE__
#include "thread.hpp"
#else
#error do not include this header directly
#endif
#endif

#include <functional>
#include <stdlib.h>

namespace doocs{namespace threadN{
static void TestFunctionForType(){}
static auto s_fncVoid = ::std::bind(TestFunctionForType);
typedef decltype(s_fncVoid)	TypeFunc;
DWORD WINAPI ThreadStartRoutine(LPVOID a_lpThreadParameter);
struct SThreadArgStruct {
	TypeFunc*			pFunc;
	::doocs::thread*	thisObj;
	void (*functionCleaner)(void*);
};
}}  // namespace doocs{namespace threadN{


template< class Function, class... Args >
doocs::thread::thread(Function&& a_f, Args&&... a_args)
{
	threadN::SThreadArgStruct* pThreadArg = static_cast<threadN::SThreadArgStruct*>(malloc(sizeof(threadN::SThreadArgStruct)));
	m_state.allBits = 0;
	if(pThreadArg){
		DWORD threadId;
		auto bndRes = ::std::bind(a_f, a_args...);
		typedef decltype(bndRes) TypeBndRes;
		pThreadArg->pFunc = reinterpret_cast<threadN::TypeFunc*>(new TypeBndRes(bndRes) );
		pThreadArg->thisObj = this;
		pThreadArg->functionCleaner = [](void*a_fncPointer){delete static_cast<TypeBndRes*>(a_fncPointer);}  ;
		m_state.st.shouldRun = 1;
		m_handle=CreateThread(nullptr, 0, &threadN::ThreadStartRoutine,pThreadArg,0,&threadId);
		m_state.st.threadId = static_cast<uint64_t>(threadId);
	}
	else{
		// function is noexcept, so no exception
		m_handle = nullptr;
	}
}

#endif   // #ifdef _WIN32
#endif   // #ifndef DOOCS_THREAD_HPP
