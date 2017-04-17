#ifndef _TIMER_HEADER_
#define _TIMER_HEADER_

#include <stdint.h>
#include <memory>
#include <map>

#include <chrono>
#include <ctime>

#include "min_heap.h"
#include "timer_handler.h"

namespace gsf
{
	namespace utils
	{
		/**!
			timer!
		*/

		template <typename T>
		struct timer_traits
		{
			typedef typename T::type type;
		};


		struct delay_milliseconds_tag {};
		struct delay_milliseconds
		{
			typedef delay_milliseconds_tag type;

			delay_milliseconds(uint32_t milliseconds)
				: milliseconds_(milliseconds)
			{}

			uint32_t milliseconds() const { return milliseconds_; }
		private:
			uint32_t milliseconds_;
		};

		/**!
			fixed
		*/
		struct delay_day_tag {};
		struct delay_day
		{
			typedef delay_day_tag type;

			delay_day(uint32_t hour, uint32_t minute)
				: hour_(hour)
				, minute_(minute)
			{}

			uint32_t Hour() const { return hour_; }
			uint32_t Minute() const { return minute_; }

		private:
			uint32_t hour_;
			uint32_t minute_;
		};


		struct delay_week_tag {};
		struct delay_week
		{
			typedef delay_week_tag type;

			delay_week(uint32_t day, uint32_t hour)
				: day_(day)
				, hour_(hour)
			{}

			uint32_t Day() const { return day_; }
			uint32_t Hour() const { return hour_; }

		private:
			uint32_t day_;
			uint32_t hour_;
		};


		struct delay_month_tag {};
		struct delay_month
		{
			typedef delay_month_tag type;

			delay_month(uint32_t day, uint32_t hour)
				: day_(day)
				, hour_(hour)
			{}

			uint32_t Day() const { return day_; }
			uint32_t Hour() const { return hour_; }

		private:
			uint32_t day_;
			uint32_t hour_;
		};

		typedef std::shared_ptr<TimerHandler> TimerHandlerPtr;

		struct TimerEvent
		{
			TimerHandlerPtr timer_handler_ptr_;
			std::chrono::system_clock::time_point tp_;
			int32_t min_heap_idx;
		};

		class Timer
		{
		public:

			~Timer();
			static Timer& instance();

			template <typename T>
			TimerEvent * add_timer(T delay, TimerHandlerPtr timer_handler_ptr);

			int rmv_timer(TimerEvent *e);

			void update();

		private:
			Timer();
			static Timer* instance_;

			TimerEvent * update_delay(delay_milliseconds delay, TimerHandlerPtr handler, delay_milliseconds_tag);
			TimerEvent * update_delay(delay_day delay, TimerHandlerPtr handler, delay_day_tag);
			TimerEvent * update_delay(delay_week delay, TimerHandlerPtr handler, delay_week_tag);
			TimerEvent * update_delay(delay_month delay, TimerHandlerPtr handler, delay_month_tag);

		private:

			min_heap<TimerEvent> min_heap_;
		};

		Timer::~Timer()
		{

		}

		Timer::Timer()
		{
			min_heap_ctor(&min_heap_);
		}

		Timer& Timer::instance()
		{
			if (instance_ == NULL)
			{
				instance_ = new gsf::utils::Timer();
			}
			return *instance_;
		}

		TimerEvent * Timer::update_delay(delay_milliseconds delay, TimerHandlerPtr handler, delay_milliseconds_tag)
		{
			auto _tp = std::chrono::system_clock::now() + std::chrono::milliseconds(delay.milliseconds());

			TimerEvent *_event = new TimerEvent();
			_event->timer_handler_ptr_ = handler;
			_event->tp_ = _tp;

			min_heap_push(&min_heap_, _event);

			return _event;
		}

		TimerEvent * Timer::update_delay(delay_day delay, TimerHandlerPtr handler, delay_day_tag)
		{
			using namespace std::chrono;
			//! 

			typedef duration<int, std::ratio<60 * 60 * 24>> dur_day;
			time_point<system_clock, dur_day> _today = time_point_cast<dur_day>(system_clock::now());

			time_point<system_clock, seconds> _second = time_point_cast<seconds>(system_clock::now());

			uint32_t _passed_second = static_cast<uint32_t>(_second.time_since_epoch().count() - _today.time_since_epoch().count() * 24 * 60 * 60);
			uint32_t _space_second = delay.Hour() * 60 * 60 + delay.Minute() * 60;

			TimerEvent *_event = new TimerEvent();
			if (_space_second > _passed_second){
				_event->tp_ = _second + seconds(_space_second - _passed_second);
			}
			else {
				_event->tp_ = _second + seconds((24 * 60 * 60) - _passed_second - _space_second);
			}

			_event->timer_handler_ptr_ = handler;

			min_heap_push(&min_heap_, _event);

			return _event;
		}

		TimerEvent * Timer::update_delay(delay_week delay, TimerHandlerPtr handler, delay_week_tag)
		{
			return nullptr;
		}

		TimerEvent * Timer::update_delay(delay_month delay, TimerHandlerPtr handler, delay_month_tag)
		{
			return nullptr;
		}

		int Timer::rmv_timer(TimerEvent *e)
		{
			return min_heap_erase(&min_heap_, e);
		}

		template <typename T>
		TimerEvent * gsf::utils::Timer::add_timer(T delay, TimerHandlerPtr timer_handler_ptr)
		{
			return update_delay(delay, timer_handler_ptr, typename timer_traits<T>::type());
		}

		void Timer::update()
		{
			using namespace std::chrono;

			if (!min_heap_empty(&min_heap_))
			{
				TimerEvent *_event_ptr = min_heap_top(&min_heap_);
				auto _now = time_point_cast<milliseconds>(system_clock::now());

				while (_event_ptr->tp_ < _now)
				{
					min_heap_pop(&min_heap_);

					_event_ptr->timer_handler_ptr_->handleTimeout();

					if (!min_heap_empty(&min_heap_)){
						_event_ptr = min_heap_top(&min_heap_);
					}
					else {
						break;
					}
				}
			}
		}


		gsf::utils::Timer * gsf::utils::Timer::instance_ = nullptr;
	}
}

#endif
