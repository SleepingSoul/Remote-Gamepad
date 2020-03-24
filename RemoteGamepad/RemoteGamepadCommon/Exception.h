#pragma once

#include <exception>
#include <string_view>
#include <sstream>

#include <spdlog/fmt/ostr.h>bin_to_hex.h


namespace RemoteGamepad
{
    class Exception : public std::exception
    {
    public:
        Exception(std::string_view text) { m_info << text << ", arguments: "; }

        template <class TArgument>
        Exception& withArgument(std::string_view name, const TArgument& argument) &
        {
            m_info << '{' << name << ':' << argument << '}';
            return *this;
        }

        template <class TArgument>
        Exception&& withArgument(std::string_view name, const TArgument& argument) &&
        {
            m_info << '{' << name << ':' << argument << '}';
            return std::move(*this);
        }

        template<typename TOStream>
        friend TOStream& operator <<(TOStream& stream, const Exception& exception)
        {
            stream << exception.m_info.str();
            return stream;
        }

    private:
        std::stringstream m_info;
    };
}
