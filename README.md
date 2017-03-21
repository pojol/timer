# timer
一个简单的Timer（只有一个头文件），拥有简单的绑定方式，基于min-heap实现。

###using
#####binding function
```c++
void test_timer_delay_1000ms(const char *str)
{
	std::cout << str << std::endl;
}

using namespace gsf::utils;
Timer::instance().add_timer(delay_milliseconds(1000)
    , makeTimerHandler(test_timer_delay_1000ms, "hello,timer!"));
    
//print hello,timer!
```

#####binding class member functions
```c++
class TimerTest
{
public:
	void pt(int count) 
	{
		std::cout << count << std::endl;
	}
};

using namespace gsf::utils;
TimerTest *tt = new TimerTest();
for (int i = 0; i < 100; ++i)
{
	Timer::instance().add_timer(delay_milliseconds(i * 100)
	    , makeTimerHandler(&TimerTest::pt, tt, i));
}
//print 0 1 2 3 4 5 ... 99 
```

#####timer
- [x] 基于min-heap (插入删除复杂度O(log(n))，获取最小元素复杂度O(1)
- [x] 支持毫秒级的延时触发
- [x] 支持固定时间点更新 天（例如每天的早上6点10分更新
- [ ] 支持固定时间点更新 周
- [ ] 支持固定时间点更新 月

#####depend
* c++11

MIT License
Copyright (c) 2016 pojol
