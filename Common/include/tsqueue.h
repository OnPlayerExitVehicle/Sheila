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
		//index _back;

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
			//std::memcpy(_data, oldBuffer + _front, sizeof(T) * rightSideSize);
            for(size_t i = _front, j = 0U; i < rightSideSize; i++, j++)
            {
                _data[j] = std::move(oldBuffer[i]);
            }
			const size_t rest = _length - rightSideSize;
			if (rest > 0)
			{
				//std::memcpy(_data + rightSideSize, oldBuffer, sizeof(T) * rest);

                for(size_t i = rightSideSize, j = 0U; i < rest; i++, j++)
                {
                    _data[i] = std::move(oldBuffer[j]);
                }
			}

			delete[] oldBuffer;

			_capacity = newCapacity;
		}

	public:
		tsqueue() : _front(0), /*_back(0),*/ _length(0), _capacity(1), _data(new T[1]) { }

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
				/*T* oldBuffer = _data;
				int64 oldCapacity = _capacity;
				_data = new T[_capacity *= 2];

				int64 rightSideSize = oldCapacity - _front;
				std::memcpy(_data, oldBuffer + _front, sizeof(T) * rightSideSize);
				int64 rest = _length - rightSideSize;
				if(rest > 0)
				{
					std::memcpy(_data + rightSideSize, oldBuffer, sizeof(T) * rest);
				}

				delete[] oldBuffer;*/
				Resize(_capacity * 2);
			}

			/*index i = (_front + _length) % _capacity;
			_data[i] = value;
			_length++;*/
			_data[(_front + _length++) % _capacity] = std::move(value);

			//std::cout << "After Enqueue = " << *this;

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

//		friend std::ostream& operator << (std::ostream& _lhs, const tsqueue& _rhs)
//		{
//			_lhs << std::format("Front = {}, Length = {}, Capacity = {}", _rhs._front, _rhs._length, _rhs._capacity) << std::endl;
//
//			uint64 empty = _rhs._capacity - _rhs._length;
//			uint64 right = std::min(_rhs._capacity - _rhs._front, _rhs._length);
//			uint64 left = _rhs._length - right;
//
//			/*long long empty = _rhs._capacity - _rhs._length;
//			long long right = _rhs._capacity - _rhs._front;
//			long long left = _rhs._length - right + empty;*/
//
//			for (uint64 i = 0; i < _rhs._capacity; i++)
//			{
//				if (i == _rhs._front) _lhs << 'f';
//				if ((_rhs._front + _rhs._length) >= _rhs._capacity)
//					if (i <= _rhs._front - empty)
//						_lhs << "[]";
//					else _lhs << std::format("[{}]", _rhs._data[i]);
//				else
//					if (_rhs._front + _rhs._length <= i)
//						_lhs << "[]";
//					else _lhs << std::format("[{}]", _rhs._data[i]);
//
//				/*if((_rhs._front + _rhs._length) % _rhs._capacity <= i) _lhs << "[]";
//				else _lhs << std::format("[{}]", _rhs._data[i]);*/
//			}
//
//			/*for(int64 i = 0; i < left; i++)
//			{
//				_lhs << std::format("[{}]", _rhs._data[i]);
//			}
//
//			for(int64 i = 0; i < empty; i++)
//			{
//				_lhs << "[]";
//			}
//
//			_lhs << 'f';
//
//			for(int64 i = 0; i < right; i++)
//			{
//				_lhs << std::format("[{}]", _rhs._data[_rhs._front + i]);
//			}*/
//
//			return (_lhs << std::endl);
//		}

		inline bool Empty() const
		{
			//std::scoped_lock lock(_data_mutex);
			return _length <= 0;
		}
		inline constexpr int Length() const { return _length; }
	};

	//template <typename T>
	//using tsqueue = tsqueue;
}
