#include <mutex>
#include <iostream>
#include <format>
#include <cassert>
#include "netexception.h"

namespace networking
{
	template <typename T>
	class tsqueue
	{
		typedef size_t index;
		typedef size_t uint64;

	private:
		T* _data;
		index _front;

		uint64 _length;
		uint64 _capacity;

		std::mutex _data_mutex;
		std::mutex _waiting_mutex;
		std::condition_variable _waiting_cv;

		void Resize(uint64 newCapacity)
		{
			assert(newCapacity > _capacity);

			T* oldBuffer = _data;
			_data = new T[newCapacity];

			size_t rightSideSize = _capacity - _front;
            for(size_t i = _front, j = 0U; i < rightSideSize; i++, j++)
            {
                _data[j] = std::move(oldBuffer[i]);
            }
			const size_t rest = _length - rightSideSize;
			if (rest > 0)
			{
                for(size_t i = rightSideSize, j = 0U; i < rest; i++, j++)
                {
                    _data[i] = std::move(oldBuffer[j]);
                }
			}

			delete[] oldBuffer;

			_capacity = newCapacity;
		}

	public:
		tsqueue() : _front(0), _length(0), _capacity(1), _data(new T[1]) { }

		virtual ~tsqueue()
        {
            delete[] _data;
        }

		tsqueue(const tsqueue&) = delete;
		tsqueue& operator = (const tsqueue&) = delete;


		void Enqueue(T&& value)
		{
			std::scoped_lock lock(_data_mutex);

			if (_length >= _capacity)
			{
				Resize(_capacity * 2);
			}

			_data[(_front + _length++) % _capacity] = std::move(value);

			std::unique_lock<std::mutex> ul(_waiting_mutex);
			_waiting_cv.notify_one();
		}

		T&& Dequeue()
		{
			std::scoped_lock lock(_data_mutex);

			if (Empty()) throw netexception("Trying to access memory out of bounds!");

			_length--;

            index oldFront = _front;
            _front = ((_front + 1) % _capacity);
			return std::move(_data[oldFront]);

			//std::cout << "After Dequeue = " << *this;
		}

		void Wait()
		{
			while(Empty())
			{
				std::unique_lock ul(_waiting_mutex);
				_waiting_cv.wait(ul);
			}
		}

		inline bool Empty() const
		{
			return _length <= 0;
		}
		inline constexpr int Length() const { return _length; }
	};
}
