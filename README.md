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

##### more types
```c++
struct delay_day
struct delay_week
struct delay_month
```


MIT License

Copyright (c) 2016 pojol

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
