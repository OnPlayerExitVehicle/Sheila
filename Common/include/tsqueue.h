#include <mutex>
#include <iostream>
#include <format>
#include <cassert>

namespace networking
{
	template <typename T>
	class tsqueue
	{
		typedef long long index;
		typedef long long int64;

	private:
		T* _data;
		index _front;
		//index _back;

		int64 _length;
		int64 _capacity;

		std::mutex _data_mutex;
		std::mutex _waiting_mutex;
		std::condition_variable _waiting_cv;

		void Resize(int64 newCapacity)
		{
			assert(newCapacity > _capacity);

			T* oldBuffer = _data;
			_data = new T[newCapacity];

			int64 rightSideSize = _capacity - _front;
			std::memcpy(_data, oldBuffer + _front, sizeof(T) * rightSideSize);
			const int64 rest = _length - rightSideSize;
			if (rest > 0)
			{
				std::memcpy(_data + rightSideSize, oldBuffer, sizeof(T) * rest);
			}

			delete[] oldBuffer;

			_capacity = newCapacity;
		}

	public:
		tsqueue() : _front(0), /*_back(0),*/ _length(0), _capacity(4), _data(new T[4]) { }
		virtual ~tsqueue() { }

		tsqueue(const tsqueue&) = delete;
		tsqueue& operator = (const tsqueue&) = delete;


		void Enqueue(const T& value)
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
			_data[(_front + _length++) % _capacity] = value;

			//std::cout << "After Enqueue = " << *this;

			std::unique_lock<std::mutex> ul(_waiting_mutex);
			_waiting_cv.notify_one();
		}

		T Dequeue()
		{
			std::scoped_lock lock(_data_mutex);

			if (Empty()) throw std::exception("Trying to access memory out of bounds!");

			_length--;
			return _data[_front++];

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

		friend std::ostream& operator << (std::ostream& _lhs, const tsqueue& _rhs)
		{
			_lhs << std::format("Front = {}, Length = {}, Capacity = {}", _rhs._front, _rhs._length, _rhs._capacity) << std::endl;

			int64 empty = _rhs._capacity - _rhs._length;
			int64 right = std::min(_rhs._capacity - _rhs._front, _rhs._length);
			int64 left = _rhs._length - right;

			/*long long empty = _rhs._capacity - _rhs._length;
			long long right = _rhs._capacity - _rhs._front;
			long long left = _rhs._length - right + empty;*/

			for (int64 i = 0; i < _rhs._capacity; i++)
			{
				if (i == _rhs._front) _lhs << 'f';
				if ((_rhs._front + _rhs._length) >= _rhs._capacity)
					if (i <= _rhs._front - empty)
						_lhs << "[]";
					else _lhs << std::format("[{}]", _rhs._data[i]);
				else
					if (_rhs._front + _rhs._length <= i)
						_lhs << "[]";
					else _lhs << std::format("[{}]", _rhs._data[i]);

				/*if((_rhs._front + _rhs._length) % _rhs._capacity <= i) _lhs << "[]";
				else _lhs << std::format("[{}]", _rhs._data[i]);*/
			}

			/*for(int64 i = 0; i < left; i++)
			{
				_lhs << std::format("[{}]", _rhs._data[i]);
			}

			for(int64 i = 0; i < empty; i++)
			{
				_lhs << "[]";
			}

			_lhs << 'f';

			for(int64 i = 0; i < right; i++)
			{
				_lhs << std::format("[{}]", _rhs._data[_rhs._front + i]);
			}*/

			return (_lhs << std::endl);
		}

		inline constexpr bool Empty() const
		{
			std::scoped_lock lock(_data_mutex);
			return _length <= 0;
		}
		inline constexpr int Length() const { return _length; }
	};

	//template <typename T>
	//using tsqueue = tsqueue;
}