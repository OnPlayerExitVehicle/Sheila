#pragma once
#include <exception>
#include <string>

namespace networking
{
    class netexception : public std::exception
    {
    public:
        virtual ~netexception() = default;
        netexception(const netexception&) = delete;
        netexception(netexception&&) = delete;
        netexception& operator=(const netexception&) = delete;
        netexception& operator=(netexception&&) = delete;

        netexception(const char* str) noexcept : _str(str) { }
        netexception(const char* str, const size_t size) noexcept : _str(str, size) { }
        netexception(const std::string& str) noexcept : _str(str) { }
        netexception(std::string&& str) noexcept : _str(str) { }

        inline const char* what() const noexcept override
        {
            return _str.c_str();
        }

    private:
        const std::string _str;
    };
}
