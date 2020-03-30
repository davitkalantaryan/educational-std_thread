// 
// file:			doocs/thread.hpp
// created on:		2020 Mar 30
// created by:		D. Kalantaryan
//

#ifndef DOOCS_THREAD_HPP
#define DOOCS_THREAD_HPP


#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <stdint.h>
#else
#include <thread>
#endif

namespace doocs {

#ifndef _WIN32

typedef ::std::thread	thread;

#else   // #ifndef _WIN32

class ThreadModifier;

class thread
{
	friend class ThreadModifier;
public:
	typedef HANDLE native_handle_type;
	typedef uint64_t id;
	thread() noexcept;
	thread(const thread&) = delete;
	thread(thread&& other) noexcept;
	template< class Function, class... Args >
	explicit thread(Function&& f, Args&&... args);
	~thread();
	native_handle_type native_handle();
	void join();
	bool joinable() const noexcept;
	id get_id() const noexcept;
	void detach();

protected:
	native_handle_type	m_handle;
	union{
		struct {
			uint64_t	shouldRun : 1;
			uint64_t	started : 1;
			uint64_t	finished : 1;
			uint64_t	threadId : 32;
		}st;
		uint64_t	allBits;
	}m_state;
};

#endif  // #ifndef _WIN32

}  // namespace doocs {


#ifndef DOOCS_THREAD_IMPL_HPP
#include "thread.impl.hpp"
#endif

#endif   // #ifndef DOOCS_THREAD_HPP
