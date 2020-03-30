// 
// file:			thread.cpp
// created on:		2020 Mar 30
// created by:		D. Kalantaryan
//

#include <doocs/thread.hpp>

static int s_nCleanupStarted = 0;

class DoocsThreadInitializeCleanup{
public:
	DoocsThreadInitializeCleanup(){}
	~DoocsThreadInitializeCleanup(){
		s_nCleanupStarted = 1;
	}
}static s_DoocsThreadInitializeCleanup;

doocs::thread::thread() noexcept
	:
	m_handle(nullptr)
{
	m_state.allBits = 0;
}


doocs::thread::thread(thread&& a_other) noexcept
	:
	m_handle(a_other.m_handle)
{
	m_state.allBits = a_other.m_state.allBits;
	a_other.m_handle = nullptr;
	a_other.m_state.allBits = 0;
}


doocs::thread::~thread()
{
}


doocs::thread::native_handle_type doocs::thread::native_handle()
{
	return m_handle;
}


doocs::thread::id doocs::thread::get_id() const noexcept
{
	return m_state.st.threadId;
}


void doocs::thread::detach()
{
	CloseHandle(m_handle);
	m_handle = nullptr;
	m_state.allBits = 0;
}


bool doocs::thread::joinable() const noexcept
{
	return m_handle ? true : false;
}


void doocs::thread::join()
{
	if(!m_handle){m_state.allBits = 0;return;}
	if((!m_state.st.started)&&((!m_state.st.shouldRun)||s_nCleanupStarted)){
		TerminateThread(m_handle,0);
		CloseHandle(m_handle);
		m_handle = nullptr;
		m_state.allBits = 0;
		return;
	}
	if(s_nCleanupStarted){
		while(!m_state.st.finished){
			Sleep(1);
		}
		CloseHandle(m_handle);
		m_handle = nullptr;
		m_state.allBits = 0;
		return;
	}

	WaitForSingleObject(m_handle,INFINITE);
}


/*///////////// globals  ////////////////////////////////////////*/
namespace doocs{

class ThreadModifier{
public:
	ThreadModifier(::doocs::thread* a_pThread):m_pThread(a_pThread){m_pThread->m_state.st.started = 1;}
	~ThreadModifier(){m_pThread->m_state.st.finished = 1;}
	uint64_t shouldRun()const {return m_pThread->m_state.st.shouldRun;}
private:
	::doocs::thread* m_pThread;
};

namespace threadN{

DWORD WINAPI ThreadStartRoutine(LPVOID a_lpThreadParameter)
{
	SThreadArgStruct* pThreadArg = static_cast<SThreadArgStruct*>(a_lpThreadParameter);
	::doocs::ThreadModifier aModifier(pThreadArg->thisObj);

	if(aModifier.shouldRun()){
		try{
			(*pThreadArg->pFunc)();
		}
		catch(...){
			(*pThreadArg->functionCleaner)(pThreadArg->pFunc);
			free(pThreadArg);
			pThreadArg = nullptr;
			throw;
		}
	}

	(*pThreadArg->functionCleaner)(pThreadArg->pFunc);
	free(pThreadArg);
	return 0;
}

}}  // namespace doocs{namespace threadN{
