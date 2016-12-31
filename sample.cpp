#include <stdio.h>
#include "timer.h"

#include <iostream>
#include <random>

#if defined(WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

static int count_ = 0;
class TimerTest
{
public:
	void pt(int count) 
	{
		std::cout << count << std::endl;
	}
};

class RecommendTest
{
public:
	RecommendTest()
	{
		using namespace gsf::utils;
		timer_event_ptr_ = Timer::instance().add_timer(delay_milliseconds(3000)
			, makeTimerHandler(&RecommendTest::pt, this, std::string("hello!")));
	}

	~RecommendTest()
	{
		if (timer_event_ptr_){
			if (gsf::utils::Timer::instance().rmv_timer(timer_event_ptr_) == 0){ //! succ
				delete timer_event_ptr_;
				timer_event_ptr_ = nullptr;
			}
		}
	}

	void pt(std::string str)
	{
		std::cout << str.c_str() << std::endl;

		delete timer_event_ptr_;
		timer_event_ptr_ = nullptr;
	}

private:
	gsf::utils::TimerEvent *timer_event_ptr_;
};

void test_timer_delay_1000ms(const char *str)
{
	std::cout << str << std::endl;
}

int main()
{
	using namespace gsf::utils;

	Timer::instance().add_timer(delay_milliseconds(1000), makeTimerHandler(test_timer_delay_1000ms, "hello,timer!"));

	TimerTest *tt = new TimerTest();

	for (int i = 0; i < 100; ++i)
	{
		Timer::instance().add_timer(delay_milliseconds(i * 100), makeTimerHandler(&TimerTest::pt, tt, i));
	}

	while (1)
	{
		Timer::instance().update();
#if defined(WIN32)
		Sleep(1);
#else
		usleep(1000);
#endif
	}

	return 0;
}