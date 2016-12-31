#ifndef _TIMER_HEADER_
#define _TIMER_HEADER_

#include <stdint.h>
#include <memory>
#include <map>

#include <chrono>
#include <ctime>

namespace gsf
{
	namespace utils
	{

		/**!
			min-heap
		*/

		template <typename T>
		struct min_heap
		{
			T** p;
			unsigned n, a;
		};

		template <typename T>
		static inline void	     min_heap_ctor(min_heap<T>* s);

		template <typename T>
		static inline void	     min_heap_dtor(min_heap<T>* s);

		template <typename T>
		static inline void	     min_heap_elem_init(T* e);

		template <typename T>
		static inline int	     min_heap_elt_is_top(const T *e);

		template <typename T>
		static inline int	     min_heap_elem_greater(T *a, T *b);

		template <typename T>
		static inline int	     min_heap_empty(min_heap<T>* s);

		template <typename T>
		static inline unsigned	 min_heap_size(min_heap<T>* s);

		template <typename T>
		static inline T*		 min_heap_top(min_heap<T>* s);

		template <typename T>
		static inline int	     min_heap_reserve(min_heap<T>* s, unsigned n);

		template <typename T>
		static inline int	     min_heap_push(min_heap<T>* s, T* e);

		template <typename T>
		static inline T*		 min_heap_pop(min_heap<T>* s);

		template <typename T>
		static inline int	     min_heap_erase(min_heap<T>* s, T* e);

		template <typename T>
		static inline void	     min_heap_shift_up_(min_heap<T>* s, unsigned hole_index, T* e);

		template <typename T>
		static inline void	     min_heap_shift_down_(min_heap<T>* s, unsigned hole_index, T* e);

		template <typename T>
		int min_heap_elem_greater(T *a, T *b)
		{
			return (a->tp_ > b->tp_);
		}

		template <typename T>
		void min_heap_ctor(min_heap<T>* s) { s->p = 0; s->n = 0; s->a = 0; }

		template <typename T>
		void min_heap_dtor(min_heap<T>* s) { if (s->p) free(s->p); }

		template <typename T>
		void min_heap_elem_init(T* e) { e->min_heap_idx = -1; }

		template <typename T>
		int min_heap_empty(min_heap<T>* s) { return 0u == s->n; }

		template <typename T>
		unsigned min_heap_size(min_heap<T>* s) { return s->n; }

		template <typename T>
		T* min_heap_top(min_heap<T>* s) { return s->n ? *s->p : 0; }

		template <typename T>
		int min_heap_push(min_heap<T>* s, T* e)
		{
			if (min_heap_reserve(s, s->n + 1))
				return -1;
			min_heap_shift_up_(s, s->n++, e);
			return 0;
		}

		template <typename T>
		T* min_heap_pop(min_heap<T>* s)
		{
			if (s->n)
			{
				T* e = *s->p;
				min_heap_shift_down_(s, 0u, s->p[--s->n]);
				e->min_heap_idx = -1;
				return e;
			}
			return 0;
		}

		template <typename T>
		int min_heap_elt_is_top(const T *e)
		{
			return e->min_heap_idx == 0;
		}

		template <typename T>
		int min_heap_erase(min_heap<T>* s, T* e)
		{
			if (-1 != e->min_heap_idx)
			{
				T *last = s->p[--s->n];
				unsigned parent = (e->min_heap_idx - 1) / 2;

				if (e->min_heap_idx > 0 && min_heap_elem_greater(s->p[parent], last))
					min_heap_shift_up_(s, e->min_heap_idx, last);
				else
					min_heap_shift_down_(s, e->min_heap_idx, last);
				e->min_heap_idx = -1;
				return 0;
			}
			return -1;
		}

		template <typename T>
		int min_heap_reserve(min_heap<T>* s, unsigned n)
		{
			if (s->a < n)
			{
				T** p;
				unsigned a = s->a ? s->a * 2 : 8;
				if (a < n)
					a = n;
				if (!(p = (T**)realloc(s->p, a * sizeof *p)))
					return -1;
				s->p = p;
				s->a = a;
			}
			return 0;
		}

		template <typename T>
		void min_heap_shift_up_(min_heap<T>* s, unsigned hole_index, T* e)
		{
			unsigned parent = (hole_index - 1) / 2;
			while (hole_index && min_heap_elem_greater(s->p[parent], e))
			{
				(s->p[hole_index] = s->p[parent])->min_heap_idx = hole_index;
				hole_index = parent;
				parent = (hole_index - 1) / 2;
			}
			(s->p[hole_index] = e)->min_heap_idx = hole_index;
		}

		template <typename T>
		void min_heap_shift_down_(min_heap<T>* s, unsigned hole_index, T* e)
		{
			unsigned min_child = 2 * (hole_index + 1);
			while (min_child <= s->n)
			{
				min_child -= min_child == s->n || min_heap_elem_greater(s->p[min_child], s->p[min_child - 1]);
				if (!(min_heap_elem_greater(e, s->p[min_child])))
					break;
				(s->p[hole_index] = s->p[min_child])->min_heap_idx = hole_index;
				hole_index = min_child;
				min_child = 2 * (hole_index + 1);
			}
			(s->p[hole_index] = e)->min_heap_idx = hole_index;
		}

		/**!
			timer handler
		*/

		class TimerHandler
		{
		public:
			TimerHandler();

			virtual ~TimerHandler();

			virtual void handleTimeout() = 0;
		};

		inline TimerHandler::TimerHandler()
		{

		}
		inline TimerHandler::~TimerHandler()
		{

		}

		template <typename T>
		class TTimerHandler;

		class TimerHandler;
		typedef std::shared_ptr<TimerHandler> TimerHandlerPtr;

		//R ()
		template <typename R>
		class TTimerHandler<R()>:public TimerHandler
		{
		public:
			typedef R(*FUNC_TYPE)();
			TTimerHandler(FUNC_TYPE func);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
		};

		template <typename R>
		inline TimerHandlerPtr makeTimerHandler(R(*func)())
		{
			return std::make_shared<TTimerHandler<R()>>(func);
		}

		template <typename R>
		inline TTimerHandler<R()>::TTimerHandler(typename TTimerHandler<R()>::FUNC_TYPE func) :
			m_func(func)
		{
		}

		template <typename R>
		inline void TTimerHandler<R()>::handleTimeout()
		{
			(*m_func)();
		}

		//R (P1)
		template <typename R, typename P1>
		class TTimerHandler<R(P1)>:public TimerHandler
		{
		public:
			typedef R(*FUNC_TYPE)(P1);
			TTimerHandler(FUNC_TYPE func, P1 p1);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			P1			m_p1;
		};

		template <typename R, typename P1>
		inline TimerHandlerPtr makeTimerHandler(R(*func)(P1), P1 p1)
		{
			return std::make_shared<TTimerHandler<R(P1)>>(func, p1);
		}

		template <typename R, typename P1>
		inline TTimerHandler<R(P1)>::TTimerHandler(typename TTimerHandler<R(P1)>::FUNC_TYPE func, P1 p1) :
			m_func(func),
			m_p1(p1)
		{
		}

		template <typename R, typename P1>
		inline void TTimerHandler<R(P1)>::handleTimeout()
		{
			(*m_func)(m_p1);
		}

		//R (P1,P2)
		template <typename R, typename P1, typename P2>
		class TTimerHandler<R(P1, P2)>:public TimerHandler
		{
		public:
			typedef R(*FUNC_TYPE)(P1, P2);
			TTimerHandler(FUNC_TYPE func, P1 p1, P2 p2);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			P1			m_p1;
			P2			m_p2;
		};

		template <typename R, typename P1, typename P2>
		inline TimerHandlerPtr makeTimerHandler(R(*func)(P1, P2), P1 p1, P2 p2)
		{
			return std::make_shared<TTimerHandler<R(P1, P2)>>(func, p1, p2);
		}

		template <typename R, typename P1, typename P2>
		inline TTimerHandler<R(P1, P2)>::TTimerHandler(typename TTimerHandler<R(P1, P2)>::FUNC_TYPE func, P1 p1, P2 p2) :
			m_func(func),
			m_p1(p1),
			m_p2(p2)
		{
		}

		template <typename R, typename P1, typename P2>
		inline void TTimerHandler<R(P1, P2)>::handleTimeout()
		{
			(*m_func)(m_p1, m_p2);
		}

		//R (P1,P2,P3)
		template <typename R, typename P1, typename P2, typename P3>
		class TTimerHandler<R(P1, P2, P3)>:public TimerHandler
		{
		public:
			typedef R(*FUNC_TYPE)(P1, P2, P3);
			TTimerHandler(FUNC_TYPE func, P1 p1, P2 p2, P3 p3);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
		};

		template <typename R, typename P1, typename P2, typename P3>
		inline TimerHandlerPtr makeTimerHandler(R(*func)(P1, P2, P3), P1 p1, P2 p2, P3 p3)
		{
			return std::make_shared<TTimerHandler<R(P1, P2, P3)>>(func, p1, p2, p3);
		}

		template <typename R, typename P1, typename P2, typename P3>
		inline TTimerHandler<R(P1, P2, P3)>::TTimerHandler(typename TTimerHandler<R(P1, P2, P3)>::FUNC_TYPE func, P1 p1, P2 p2, P3 p3) :
			m_func(func),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3)
		{
		}

		template <typename R, typename P1, typename P2, typename P3>
		inline void TTimerHandler<R(P1, P2, P3)>::handleTimeout()
		{
			(*m_func)(m_p1, m_p2, m_p3);
		}

		//R (P1,P2,P3,P4)
		template <typename R, typename P1, typename P2, typename P3, typename P4>
		class TTimerHandler<R(P1, P2, P3, P4)>:public TimerHandler
		{
		public:
			typedef R(*FUNC_TYPE)(P1, P2, P3, P4);
			TTimerHandler(FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
			P4			m_p4;
		};

		template <typename R, typename P1, typename P2, typename P3, typename P4>
		inline TimerHandlerPtr makeTimerHandler(R(*func)(P1, P2, P3, P4), P1 p1, P2 p2, P3 p3, P4 p4)
		{
			return std::make_shared<TTimerHandler<R(P1, P2, P3, P4)>>(func, p1, p2, p3, p4);
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4>
		inline TTimerHandler<R(P1, P2, P3, P4)>::TTimerHandler(typename TTimerHandler<R(P1, P2, P3, P4)>::FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4) :
			m_func(func),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3),
			m_p4(p4)
		{
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4>
		inline void TTimerHandler<R(P1, P2, P3, P4)>::handleTimeout()
		{
			(*m_func)(m_p1, m_p2, m_p3, m_p4);
		}

		//R (P1,P2,P3,P4,P5)
		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
		class TTimerHandler<R(P1, P2, P3, P4, P5)>:public TimerHandler
		{
		public:
			typedef R(*FUNC_TYPE)(P1, P2, P3, P4, P5);
			TTimerHandler(FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
			P4			m_p4;
			P5			m_p5;
		};

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
		inline TimerHandlerPtr makeTimerHandler(R(*func)(P1, P2, P3, P4, P5), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
		{
			return std::make_shared<TTimerHandler<R(P1, P2, P3, P4, P5)>>(func, p1, p2, p3, p4, p5);
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
		inline TTimerHandler<R(P1, P2, P3, P4, P5)>::TTimerHandler(typename TTimerHandler<R(P1, P2, P3, P4, P5)>::FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) :
			m_func(func),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3),
			m_p4(p4),
			m_p5(p5)
		{
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
		inline void TTimerHandler<R(P1, P2, P3, P4, P5)>::handleTimeout()
		{
			(*m_func)(m_p1, m_p2, m_p3, m_p4, m_p5);
		}

		//R (P1,P2,P3,P4,P5,P6)
		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		class TTimerHandler<R(P1, P2, P3, P4, P5, P6)>:public TimerHandler
		{
		public:
			typedef R(*FUNC_TYPE)(P1, P2, P3, P4, P5, P6);
			TTimerHandler(FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
			P4			m_p4;
			P5			m_p5;
			P6			m_p6;
		};

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		inline TimerHandlerPtr makeTimerHandler(R(*func)(P1, P2, P3, P4, P5, P6), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
		{
			return std::make_shared<TTimerHandler<R(P1, P2, P3, P4, P5, P6)>>(func, p1, p2, p3, p4, p5, p6);
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		inline TTimerHandler<R(P1, P2, P3, P4, P5, P6)>::TTimerHandler(typename TTimerHandler<R(P1, P2, P3, P4, P5, P6)>::FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) :
			m_func(func),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3),
			m_p4(p4),
			m_p5(p5),
			m_p6(p6)
		{
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		inline void TTimerHandler<R(P1, P2, P3, P4, P5, P6)>::handleTimeout()
		{
			(*m_func)(m_p1, m_p2, m_p3, m_p4, m_p5, m_p6);
		}

		//R (P1,P2,P3,P4,P5,P6,P7)
		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		class TTimerHandler<R(P1, P2, P3, P4, P5, P6, P7)>:public TimerHandler
		{
		public:
			typedef R(*FUNC_TYPE)(P1, P2, P3, P4, P5, P6, P7);
			TTimerHandler(FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
			P4			m_p4;
			P5			m_p5;
			P6			m_p6;
			P7			m_p7;
		};

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		inline TimerHandlerPtr makeTimerHandler(R(*func)(P1, P2, P3, P4, P5, P6, P7), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
		{
			return std::make_shared<TTimerHandler<R(P1, P2, P3, P4, P5, P6, P7)>>(func, p1, p2, p3, p4, p5, p6, p7);
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		inline TTimerHandler<R(P1, P2, P3, P4, P5, P6, P7)>::TTimerHandler(typename TTimerHandler<R(P1, P2, P3, P4, P5, P6, P7)>::FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) :
			m_func(func),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3),
			m_p4(p4),
			m_p5(p5),
			m_p6(p6),
			m_p7(p7)
		{
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		inline void TTimerHandler<R(P1, P2, P3, P4, P5, P6, P7)>::handleTimeout()
		{
			(*m_func)(m_p1, m_p2, m_p3, m_p4, m_p5, m_p6, m_p7);
		}

		//R (P1,P2,P3,P4,P5,P6,P7,P8)
		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		class TTimerHandler<R(P1, P2, P3, P4, P5, P6, P7, P8)>:public TimerHandler
		{
		public:
			typedef R(*FUNC_TYPE)(P1, P2, P3, P4, P5, P6, P7, P8);
			TTimerHandler(FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
			P4			m_p4;
			P5			m_p5;
			P6			m_p6;
			P7			m_p7;
			P8			m_p8;
		};

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		inline TimerHandlerPtr makeTimerHandler(R(*func)(P1, P2, P3, P4, P5, P6, P7, P8), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
		{
			return std::make_shared<TTimerHandler<R(P1, P2, P3, P4, P5, P6, P7, P8)>>(func, p1, p2, p3, p4, p5, p6, p7, p8);
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		inline TTimerHandler<R(P1, P2, P3, P4, P5, P6, P7, P8)>::TTimerHandler(typename TTimerHandler<R(P1, P2, P3, P4, P5, P6, P7, P8)>::FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8) :
			m_func(func),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3),
			m_p4(p4),
			m_p5(p5),
			m_p6(p6),
			m_p7(p7),
			m_p8(p8)
		{
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		inline void TTimerHandler<R(P1, P2, P3, P4, P5, P6, P7, P8)>::handleTimeout()
		{
			(*m_func)(m_p1, m_p2, m_p3, m_p4, m_p5, m_p6, m_p7, m_p8);
		}

		//R (P1,P2,P3,P4,P5,P6,P7,P8,P9)
		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
		class TTimerHandler<R(P1, P2, P3, P4, P5, P6, P7, P8, P9)>:public TimerHandler
		{
		public:
			typedef R(*FUNC_TYPE)(P1, P2, P3, P4, P5, P6, P7, P8, P9);
			TTimerHandler(FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
			P4			m_p4;
			P5			m_p5;
			P6			m_p6;
			P7			m_p7;
			P8			m_p8;
			P9			m_p9;
		};

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
		inline TimerHandlerPtr makeTimerHandler(R(*func)(P1, P2, P3, P4, P5, P6, P7, P8, P9), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
		{
			return std::make_shared<TTimerHandler<R(P1, P2, P3, P4, P5, P6, P7, P8, P9)>>(func, p1, p2, p3, p4, p5, p6, p7, p8, p9);
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
		inline TTimerHandler<R(P1, P2, P3, P4, P5, P6, P7, P8, P9)>::TTimerHandler(typename TTimerHandler<R(P1, P2, P3, P4, P5, P6, P7, P8, P9)>::FUNC_TYPE func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9) :
			m_func(func),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3),
			m_p4(p4),
			m_p5(p5),
			m_p6(p6),
			m_p7(p7),
			m_p8(p8),
			m_p9(p9)
		{
		}

		template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
		inline void TTimerHandler<R(P1, P2, P3, P4, P5, P6, P7, P8, P9)>::handleTimeout()
		{
			(*m_func)(m_p1, m_p2, m_p3, m_p4, m_p5, m_p6, m_p7, m_p8, m_p9);
		}

		//R (C::*)()
		template <typename C, typename R>
		class TTimerHandler<R(C::*)()>:public TimerHandler
		{
		public:
			typedef R(C::*FUNC_TYPE)();
			TTimerHandler(FUNC_TYPE func, C * obj);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			C *			m_obj;
		};

		template <typename C, typename R>
		inline TimerHandlerPtr makeTimerHandler(R(C::*func)(), C * obj)
		{
			return std::make_shared<TTimerHandler<R(C::*)()>>(func, obj);
		}

		template <typename C, typename R>
		inline TTimerHandler<R(C::*)()>::TTimerHandler(typename TTimerHandler<R(C::*)()>::FUNC_TYPE func, C * obj) :
			m_func(func),
			m_obj(obj)
		{
		}

		template <typename C, typename R>
		inline void TTimerHandler<R(C::*)()>::handleTimeout()
		{
			(m_obj->*m_func)();
		}

		//R (C::*)(P1)
		template <typename C, typename R, typename P1>
		class TTimerHandler<R(C::*)(P1)>:public TimerHandler
		{
		public:
			typedef R(C::*FUNC_TYPE)(P1);
			TTimerHandler(FUNC_TYPE func, C * obj, P1 p1);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			C *			m_obj;
			P1			m_p1;
		};

		template <typename C, typename R, typename P1>
		inline TimerHandlerPtr makeTimerHandler(R(C::*func)(P1), C * obj, P1 p1)
		{
			return std::make_shared<TTimerHandler<R(C::*)(P1)>>(func, obj, p1);
		}

		template <typename C, typename R, typename P1>
		inline TTimerHandler<R(C::*)(P1)>::TTimerHandler(typename TTimerHandler<R(C::*)(P1)>::FUNC_TYPE func, C * obj, P1 p1) :
			m_func(func),
			m_obj(obj),
			m_p1(p1)
		{
		}

		template <typename C, typename R, typename P1>
		inline void TTimerHandler<R(C::*)(P1)>::handleTimeout()
		{
			(m_obj->*m_func)(m_p1);
		}

		//R (C::*)(P1,P2)
		template <typename C, typename R, typename P1, typename P2>
		class TTimerHandler<R(C::*)(P1, P2)>:public TimerHandler
		{
		public:
			typedef R(C::*FUNC_TYPE)(P1, P2);
			TTimerHandler(FUNC_TYPE func, C * obj, P1 p1, P2 p2);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			C *			m_obj;
			P1			m_p1;
			P2			m_p2;
		};

		template <typename C, typename R, typename P1, typename P2>
		inline TimerHandlerPtr makeTimerHandler(R(C::*func)(P1, P2), C * obj, P1 p1, P2 p2)
		{
			return std::make_shared<TTimerHandler<R(C::*(P1, P2))>>(func, obj, p1, p2);
		}

		template <typename C, typename R, typename P1, typename P2>
		inline TTimerHandler<R(C::*)(P1, P2)>::TTimerHandler(typename TTimerHandler<R(C::*)(P1, P2)>::FUNC_TYPE func, C * obj, P1 p1, P2 p2) :
			m_func(func),
			m_obj(obj),
			m_p1(p1),
			m_p2(p2)
		{
		}

		template <typename C, typename R, typename P1, typename P2>
		inline void TTimerHandler<R(C::*)(P1, P2)>::handleTimeout()
		{
			(m_obj->*m_func)(m_p1, m_p2);
		}

		//R (C::*)(P1,P2,P3)
		template <typename C, typename R, typename P1, typename P2, typename P3>
		class TTimerHandler<R(C::*)(P1, P2, P3)>:public TimerHandler
		{
		public:
			typedef R(C::*FUNC_TYPE)(P1, P2, P3);
			TTimerHandler(FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			C *			m_obj;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
		};

		template <typename C, typename R, typename P1, typename P2, typename P3>
		inline TimerHandlerPtr makeTimerHandler(R(C::*func)(P1, P2, P3), C * obj, P1 p1, P2 p2, P3 p3)
		{
			return std::make_shared<TTimerHandler<R(C::*)(P1, P2, P3)>>(func, obj, p1, p2, p3);
		}

		template <typename C, typename R, typename P1, typename P2, typename P3>
		inline TTimerHandler<R(C::*)(P1, P2, P3)>::TTimerHandler(typename TTimerHandler<R(C::*)(P1, P2, P3)>::FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3) :
			m_func(func),
			m_obj(obj),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3)
		{
		}

		template <typename C, typename R, typename P1, typename P2, typename P3>
		inline void TTimerHandler<R(C::*)(P1, P2, P3)>::handleTimeout()
		{
			(m_obj->*m_func)(m_p1, m_p2, m_p3);
		}

		//R (C::*)(P1,P2,P3,P4)
		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4>
		class TTimerHandler<R(C::*)(P1, P2, P3, P4)>:public TimerHandler
		{
		public:
			typedef R(C::*FUNC_TYPE)(P1, P2, P3, P4);
			TTimerHandler(FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			C *			m_obj;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
			P4			m_p4;
		};

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4>
		inline TimerHandlerPtr makeTimerHandler(R(C::*func)(P1, P2, P3, P4), C * obj, P1 p1, P2 p2, P3 p3, P4 p4)
		{
			return std::make_shared<TTimerHandler<R(C::*)(P1, P2, P3, P4)>>(func, obj, p1, p2, p3, p4);
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4>
		inline TTimerHandler<R(C::*)(P1, P2, P3, P4)>::TTimerHandler(typename TTimerHandler<R(C::*)(P1, P2, P3, P4)>::FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4) :
			m_func(func),
			m_obj(obj),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3),
			m_p4(p4)
		{
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4>
		inline void TTimerHandler<R(C::*)(P1, P2, P3, P4)>::handleTimeout()
		{
			(m_obj->*m_func)(m_p1, m_p2, m_p3, m_p4);
		}

		//R (C::*)(P1,P2,P3,P4,P5)
		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
		class TTimerHandler<R(C::*)(P1, P2, P3, P4, P5)>:public TimerHandler
		{
		public:
			typedef R(C::*FUNC_TYPE)(P1, P2, P3, P4, P5);
			TTimerHandler(FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			C *			m_obj;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
			P4			m_p4;
			P5			m_p5;
		};

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
		inline TimerHandlerPtr makeTimerHandler(R(C::*func)(P1, P2, P3, P4, P5), C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
		{
			return std::make_shared<TTimerHandler<R(C::*)(P1, P2, P3, P4, P5)>>(func, obj, p1, p2, p3, p4, p5);
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
		inline TTimerHandler<R(C::*)(P1, P2, P3, P4, P5)>::TTimerHandler(typename TTimerHandler<R(C::*)(P1, P2, P3, P4, P5)>::FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) :
			m_func(func),
			m_obj(obj),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3),
			m_p4(p4),
			m_p5(p5)
		{
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
		inline void TTimerHandler<R(C::*)(P1, P2, P3, P4, P5)>::handleTimeout()
		{
			(m_obj->*m_func)(m_p1, m_p2, m_p3, m_p4, m_p5);
		}

		//R (C::*)(P1,P2,P3,P4,P5,P6)
		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		class TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6)>:public TimerHandler
		{
		public:
			typedef R(C::*FUNC_TYPE)(P1, P2, P3, P4, P5, P6);
			TTimerHandler(FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			C *			m_obj;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
			P4			m_p4;
			P5			m_p5;
			P6			m_p6;
		};

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		inline TimerHandlerPtr makeTimerHandler(R(C::*func)(P1, P2, P3, P4, P5, P6), C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
		{
			return std::make_shared<TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6)>>(func, obj, p1, p2, p3, p4, p5, p6);
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		inline TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6)>::TTimerHandler(typename TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6)>::FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) :
			m_func(func),
			m_obj(obj),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3),
			m_p4(p4),
			m_p5(p5),
			m_p6(p6)
		{
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		inline void TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6)>::handleTimeout()
		{
			(m_obj->*m_func)(m_p1, m_p2, m_p3, m_p4, m_p5, m_p6);
		}

		//R (C::*)(P1,P2,P3,P4,P5,P6,P7)
		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		class TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7)>:public TimerHandler
		{
		public:
			typedef R(C::*FUNC_TYPE)(P1, P2, P3, P4, P5, P6, P7);
			TTimerHandler(FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			C *			m_obj;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
			P4			m_p4;
			P5			m_p5;
			P6			m_p6;
			P7			m_p7;
		};

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		inline TimerHandlerPtr makeTimerHandler(R(C::*func)(P1, P2, P3, P4, P5, P6, P7), C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
		{
			return std::make_shared<TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7)>>(func, obj, p1, p2, p3, p4, p5, p6, p7);
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		inline TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7)>::TTimerHandler(typename TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7)>::FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7) :
			m_func(func),
			m_obj(obj),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3),
			m_p4(p4),
			m_p5(p5),
			m_p6(p6),
			m_p7(p7)
		{
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		inline void TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7)>::handleTimeout()
		{
			(m_obj->*m_func)(m_p1, m_p2, m_p3, m_p4, m_p5, m_p6, m_p7);
		}

		//R (C::*)(P1,P2,P3,P4,P5,P6,P7,P8)
		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		class TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7, P8)>:public TimerHandler
		{
		public:
			typedef R(C::*FUNC_TYPE)(P1, P2, P3, P4, P5, P6, P7, P8);
			TTimerHandler(FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			C *			m_obj;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
			P4			m_p4;
			P5			m_p5;
			P6			m_p6;
			P7			m_p7;
			P8			m_p8;
		};

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		inline TimerHandlerPtr makeTimerHandler(R(C::*func)(P1, P2, P3, P4, P5, P6, P7, P8), C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
		{
			return std::make_shared<TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7, P8)>>(func, obj, p1, p2, p3, p4, p5, p6, p7, p8);
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		inline TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7, P8)>::TTimerHandler(typename TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7, P8)>::FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8) :
			m_func(func),
			m_obj(obj),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3),
			m_p4(p4),
			m_p5(p5),
			m_p6(p6),
			m_p7(p7),
			m_p8(p8)
		{
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		inline void TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7, P8)>::handleTimeout()
		{
			(m_obj->*m_func)(m_p1, m_p2, m_p3, m_p4, m_p5, m_p6, m_p7, m_p8);
		}

		//R (C::*)(P1,P2,P3,P4,P5,P6,P7,P8,P9)
		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
		class TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9)>:public TimerHandler
		{
		public:
			typedef R(C::*FUNC_TYPE)(P1, P2, P3, P4, P5, P6, P7, P8, P9);
			TTimerHandler(FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9);
			void handleTimeout();
		private:
			FUNC_TYPE	m_func;
			C *			m_obj;
			P1			m_p1;
			P2			m_p2;
			P3			m_p3;
			P4			m_p4;
			P5			m_p5;
			P6			m_p6;
			P7			m_p7;
			P8			m_p8;
			P9			m_p9;
		};

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
		inline TimerHandlerPtr makeTimerHandler(R(C::*func)(P1, P2, P3, P4, P5, P6, P7, P8, P9), C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
		{
			return std::make_shared<TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9)>>(func, obj, p1, p2, p3, p4, p5, p6, p7, p8, p9);
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
		inline TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9)>::TTimerHandler(typename TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9)>::FUNC_TYPE func, C * obj, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9) :
			m_func(func),
			m_obj(obj),
			m_p1(p1),
			m_p2(p2),
			m_p3(p3),
			m_p4(p4),
			m_p5(p5),
			m_p6(p6),
			m_p7(p7),
			m_p8(p8),
			m_p9(p9)
		{
		}

		template <typename C, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
		inline void TTimerHandler<R(C::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9)>::handleTimeout()
		{
			(m_obj->*m_func)(m_p1, m_p2, m_p3, m_p4, m_p5, m_p6, m_p7, m_p8, m_p9);
		}

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
