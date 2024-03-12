#pragma once
#include <vector>

namespace networking
{
	class message
	{
	public:
		class _header
		{
			friend class message;
		public:

			_header() = default;
			_header(const _header&) = delete;
			_header& operator = (const _header&) = delete;
			_header(_header&&) = default;
		private:

		public:

			uint8_t flag;
		} header;

	public:
		class _body
		{
			friend class message;
		public:

			_body() = default;
			_body(uint8_t* first, size_t bytes) : byte_buffer(first, first + bytes) { }
			_body(const _body&) = delete;
			_body& operator = (const _body&) = delete;
			_body(_body&& other) noexcept : byte_buffer{std::move(other.byte_buffer)} { }

		private:

			std::vector<uint8_t> byte_buffer;
		} body;

	public:
		virtual ~message() = default;
		message() = default;
		message(uint8_t* first, size_t bytes);
		message(const message&) = delete;
		message& operator = (const message&) = delete;

		bool packed = false;

		message(message&& other) noexcept : header{std::move(other.header)}, body{std::move(other.body)}, packed(other.packed) { }

		inline constexpr std::vector<uint8_t>& buffer()
		{
			return body.byte_buffer;
		}

		inline constexpr void* data() noexcept
		{
			return body.byte_buffer.data();
		}

		inline constexpr size_t size() const noexcept
		{
			return body.byte_buffer.size();
		}

		inline constexpr void resize(size_t size)
		{
			body.byte_buffer.resize(size);
		}

		inline constexpr void clear() noexcept
		{
			body.byte_buffer.clear();
		}

		inline void pack() noexcept;
		inline void unpack() noexcept;
	};

	template <typename T>
	message& operator << (message& lhs, const T& rhs)
	{
		if (lhs.packed)
			lhs.unpack();

		static_assert(
			/*std::is_fundamental_v<T> &&*/ !std::is_pointer_v<T> && !std::is_array_v<T>,
			"Type of the object must be primitive!"
			);

		const size_t size = sizeof(T);
		std::vector<uint8_t>& byte_buffer = lhs.buffer();
		const size_t buffer_size = byte_buffer.size();

		byte_buffer.resize(buffer_size + size);

		std::memcpy(byte_buffer.data() + buffer_size, &rhs, size);

		return lhs;
	}

	template <typename T>
	message& operator >> (message& lhs, T& rhs)
	{
		if (lhs.packed)
			lhs.unpack();

		static_assert(
			/*std::is_fundamental_v<T> &&*/ !std::is_pointer_v<T> && !std::is_array_v<T>,
			"Type of the object must be primitive!"
			);

		const size_t size = sizeof(T);
		std::vector<uint8_t>& byte_buffer = lhs.buffer();
		const size_t buffer_size = byte_buffer.size();
		if (buffer_size < size) throw std::exception("Size overrun while reading from message buffer!");

		std::memcpy(&rhs, byte_buffer.data() + buffer_size - size, size);
		//byte_buffer.pop()
		byte_buffer.resize(buffer_size - size);

		return lhs;
	}

	template<>
	inline message& operator << <std::string>(message& lhs, const std::string& rhs)
	{
		if (lhs.packed)
			lhs.unpack();

		constexpr static size_t char_size = sizeof(char);
		constexpr static size_t int_size = sizeof(int);
		const size_t length = rhs.length();
		std::vector<uint8_t>& byte_buffer = lhs.buffer();
		const size_t buffer_size = byte_buffer.size();

		byte_buffer.resize(buffer_size + int_size + (length * char_size));

		std::memcpy(byte_buffer.data() + buffer_size, rhs.c_str(), length * char_size);
		std::memcpy(byte_buffer.data() + buffer_size + (length * char_size), &length, int_size);

		return lhs;
	}

	template<>
	inline message& operator >> <std::string>(message& lhs, std::string& rhs)
	{
		if (lhs.packed)
			lhs.unpack();

		constexpr static size_t char_size = sizeof(char);
		constexpr static size_t int_size = sizeof(int);

		std::vector<uint8_t>& byte_buffer = lhs.buffer();
		const size_t buffer_size = byte_buffer.size();

		int length;
		char buf[256];
		std::memcpy(&length, byte_buffer.data() + buffer_size - int_size, int_size);
		std::memcpy(buf, byte_buffer.data() + buffer_size - int_size - (length * char_size), length * char_size);
		byte_buffer.resize(buffer_size - int_size - (length * char_size));

		rhs = std::string(buf, length);

		return lhs;
	}

	inline message::message(uint8_t* first, size_t bytes) : body(first, bytes) { *this >> header.flag; }

	void message::pack() noexcept
	{
		if (packed) return;

		*this << header.flag;
		packed = true;
	}

	void message::unpack() noexcept
	{
		if (!packed) return;

		packed = false;
		uint8_t flag;
		*this >> flag;
	}
}