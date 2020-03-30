// 
// file:			main_thread_test01.cpp
// created on:		2020 Mar 30
// created by:		D. Kalantaryan
//

#include <doocs/thread.hpp>
#include <iostream>

static void ThreadFunction1()
{
	int nIter = 0;
	while (1) {
		::std::cout << "+++++ iter=" << ++nIter << ::std::endl;
		Sleep(100);
	}
}

class AThrClass
{
public:
	void FuncMunc()
	{
		int nIter = 0;
		for(int i=0;i<100;++i){
			::std::cout << "---- iter("<< this <<")=" << ++nIter << ::std::endl;
			Sleep(100);
		}
	}
};

int main()
{
	AThrClass aCls;
	::doocs::thread aThread1(ThreadFunction1);
	::doocs::thread aThread2(&AThrClass::FuncMunc,&aCls);

	aThread2.join();
	aThread1.join();

	return 0;
}
