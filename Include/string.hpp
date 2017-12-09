#pragma once

#include <comutil.h>

#include <algorithm>
#include <chrono>
#include <codecvt>
#include <functional>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <vector>

namespace trim_utility
{
    template<typename _Elem> inline
        void trim_left(std::basic_string<_Elem> &str, const _Elem* delim)
    {
        str.erase(0, str.find_first_not_of(delim));
    }

    template<typename _Elem> inline
        void trim_right(std::basic_string<_Elem> &str, const _Elem* delim)
    {
        str.erase(str.find_last_not_of(delim) + 1);
    }

    template<typename _Elem> inline
        void trim_all(std::basic_string<_Elem> &str, const _Elem *delim)
    {
        str.erase(str.find_last_not_of(" ") + 1);
        str.erase(0, str.find_first_not_of(" "));
    }

    template<typename _Elem> inline
        std::basic_string<_Elem> trim(std::basic_string<_Elem>& str, const _Elem *delim)
    {
        using Str = std::basic_string<_Elem>;

        auto first = str.find_first_not_of(delim);
        if (first != std::string::npos)
        {
            auto second = str.find_last_not_of(delim);
            return str.substr(first, second - first);
        }
        return Str();
    }
}

namespace parse_utility
{
    template<typename _Elem> inline
        auto _tokenize(const std::basic_string<_Elem> &str, _Elem separator)->std::vector<std::basic_string<_Elem>>
    {
        using Str = std::basic_string<_Elem>;
        std::vector<Str> container;

        std::istringstream iss(str);
        Str word;

        while (getline(iss, word, ','))
        {
            container.push_back(Str(it, find));
            trim(word);
        }

        return 0;
    }
    template <typename _Elem, size_t size> inline
        auto _tokenize(const std::basic_string<_Elem> &str, const _Elem(&separator)[size])->std::vector<std::basic_string<_Elem>>
    {
        using Str = std::basic_string<_Elem>;
        std::vector<Str> container;

        for (auto it = str.begin(); it != str.end();)
        {
            auto find = std::find_first_of(it, str.end(), std::begin(separator), std::end(separator));
            container.push_back(Str(it, find));
            if (find != str.end())
            {
                find++;
            }
            it = find;
        }
        return container;
    }
    template<typename _Elem, size_t size> inline
        auto tokenize(const std::basic_string<_Elem> &str, const _Elem(&separator)[size])->std::vector<std::basic_string<_Elem>>
    {
        return _tokenize(str, separator);
    }
    template<typename _Elem, size_t size> inline
        auto tokenize(const _Elem *str, const _Elem(&separator)[size])->std::vector<std::basic_string<_Elem>>
    {
        return _tokenize(std::basic_string<_Elem>(str), separator);
    }

    inline void TokenTest()
    {
        char sep[] = ",.";
        auto tok_list = tokenize("one,two..,..three", sep);
        auto wtok_list = tokenize(L"one,two..,..three", L",.");
    }
}

namespace string_helper
{
    inline
        std::wstring to_wstring(const std::string& utf8string)
    {
        return (LPCWSTR)_bstr_t(utf8string.data());
    }

    inline
        std::string to_string(const std::wstring& widestring)
    {
        return (LPCSTR)_bstr_t(widestring.data());
    }

    // convert to upper and lower case
    template<typename _Elem> inline
        void to_upper(std::basic_string<_Elem> &str)
    {
        std::transform(str.begin(), str.end(), str.begin(), toupper);
    }

    template<typename _Elem> inline
        void to_lower(std::basic_string<_Elem> &str)
    {
        std::transform(str.begin(), str.end(), str.begin(), tolower);
    }


    inline
        std::wstring utf8_to_utf16(const std::string& utf8)
    {
        std::vector<unsigned long> unicode;
        std::size_t i = 0;
        while (i < utf8.size())
        {
            unsigned long uni;
            std::size_t todo;
            bool error = false;

            unsigned char ch = utf8[i++];

            if (ch <= 0x7F)
            {
                uni = ch;
                todo = 0;
            }
            else if (ch <= 0xBF)
            {
                throw std::logic_error("not a UTF-8 string");
            }
            else if (ch <= 0xDF)
            {
                uni = ch & 0x1F;
                todo = 1;
            }
            else if (ch <= 0xEF)
            {
                uni = ch & 0x0F;
                todo = 2;
            }
            else if (ch <= 0xF7)
            {
                uni = ch & 0x07;
                todo = 3;
            }
            else
            {
                throw std::logic_error("not a UTF-8 string");
            }
            for (std::size_t j = 0; j < todo; ++j)
            {
                if (i == utf8.size())
                    throw std::logic_error("not a UTF-8 string");

                unsigned char ch = utf8[i++];

                if (ch < 0x80 || ch > 0xBF)
                    throw std::logic_error("not a UTF-8 string");

                uni <<= 6;
                uni += ch & 0x3F;
            }
            if (uni >= 0xD800 && uni <= 0xDFFF)
                throw std::logic_error("not a UTF-8 string");

            if (uni > 0x10FFFF)
                throw std::logic_error("not a UTF-8 string");

            unicode.push_back(uni);
        }

        std::wstring utf16;

        for (unsigned long uni : unicode)
        {
            if (uni <= 0xFFFF)
            {
                utf16 += (wchar_t)uni;
            }
            else
            {
                uni -= 0x10000;
                utf16 += (wchar_t)((uni >> 10) + 0xD800);
                utf16 += (wchar_t)((uni & 0x3FF) + 0xDC00);
            }
        }

        return utf16;
    }

    inline
        std::vector<std::wstring> read_utf8_file(const std::wstring &filename)
    {
        std::vector<std::wstring> contents;

        std::wstring line;
        // Read file in UTF-8
        std::wifstream wif(filename);
        if (!wif.is_open())
            return contents;

        wif.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::consume_header>));
        while (getline(wif, line)) {
            contents.push_back(line);
        }
        return contents;
    }

    inline
        void write_utf8_file(const std::wstring &filename, std::vector<std::wstring> &contents)
    {
        std::wofstream wof;
        wof.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::generate_header>));
        wof.open(filename);

        for (auto &line : contents) {
            wof << line << std::endl;
        }
        wof.close();
    }

    inline
        void load_text_file(const std::string &filename, std::function<void(const std::string)> process)
    {
        std::ifstream ifile(filename);
        if (!ifile.is_open())
            return;

        std::string line;
        // first skip
        while (getline(ifile, line)) {
            process(line);
        }
    }

    struct string_format
    {
        static std::string &_format(std::string &buffer, const char *fmt, va_list vl)
        {
            int size = _vscprintf(fmt, vl);
            if (size > 0)
            {
                buffer.resize(size + 1);
                vsnprintf_s(&buffer[0], size + 1, _TRUNCATE, fmt, vl);
            }
            return buffer;
        }

        static std::string format(const char* fmt, ...)
        {
            va_list vl;
            va_start(vl, fmt);
            std::string buffer;
            _format(buffer, fmt, vl);
            va_end(vl);
            return buffer;
        }
    };
    inline void tprintf(const char* format) // base function
    {
        std::cout << format;
    }

    template<typename T, typename... Targs> inline
    void tprintf(const char* format, T value, Targs... Fargs) // recursive variadic function
    {
        for (; *format != '\0'; format++) {
            if (*format == '%') {
                std::cout << value;
                tprintf(format + 1, Fargs...); // recursive call
                return;
            }
            std::cout << *format;
        }
    }

    //int main()
    //{
    //    tprintf("% world% %\n", "Hello", '!', 123);
    //    return 0;
    //}
    // Hello world! 123
}

/*

The formatting codes for strftime are listed below:
%a - Abbreviated weekday name
%A - Full weekday name
%b - Abbreviated month name
%B - Full month name
%c - Date and time representation appropriate for locale
%d - Day of month as decimal number (01 – 31)
%H - Hour in 24-hour format (00 – 23)
%I - Hour in 12-hour format (01 – 12)
%j - Day of year as decimal number (001 – 366)
%m - Month as decimal number (01 – 12)
%M - Minute as decimal number (00 – 59)
%p - Current locale's A.M./P.M. indicator for 12-hour clock
%S - Second as decimal number (00 – 59)
%U - Week of year as decimal number, with Sunday as first day of week (00 – 53)
%w - Weekday as decimal number (0 – 6; Sunday is 0)
%W - Week of year as decimal number, with Monday as first day of week (00 – 53)
%x - Date representation for current locale
%X - Time representation for current locale
%y - Year without century, as decimal number (00 – 99)
%Y - Year with century, as decimal number
%z, %Z - Either the time-zone name or time zone abbreviation, depending on registry settings; no characters if time zone is unknown
%% - Percent sign

_CRTIMP size_t __cdecl strftime(_Out_writes_z_(_SizeInBytes) char * _Buf, _In_ size_t _SizeInBytes, _In_z_ _Printf_format_string_ const char * _Format, _In_ const struct tm * _Tm);
_CRTIMP size_t __cdecl _strftime_l(_Pre_notnull_ _Post_z_ char *_Buf, _In_ size_t _Max_size, _In_z_ _Printf_format_string_ const char * _Format, _In_ const struct tm *_Tm, _In_opt_ _locale_t _Locale);

*/

namespace time_helper
{
    using Clock = std::chrono::system_clock;// high_resolution_clock;
    using Ms = std::chrono::milliseconds;
    using Sec = std::chrono::seconds;

    template<class Duration>
    using TimePoint = std::chrono::time_point<Clock, Duration>;

    //#ifdef GetCurrentTime
    //#undef GetCurrentTime
    //#endif
    inline
        std::string GetCurrentDateTime()
    {
        //TimePoint<Ms> now = std::chrono::time_point_cast<Ms>(Clock::now());
        Clock::time_point now = Clock::now();
        std::time_t now_c = Clock::to_time_t(now);

        tm now_tm;
        int err = localtime_s(&now_tm, &now_c);
        std::stringstream ss;
        ss << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S"); // yyyy-mm-dd HH:MM:SS

        //TimePoint<Ms> ms(now);
        Ms ms = std::chrono::duration_cast<Ms>(now.time_since_epoch());
        auto msec_unit = std::chrono::duration_cast<Ms>(Sec(1)).count();
        auto msec = ms % msec_unit;
        //Ms msec = std::chrono::duration_cast<Ms>(ms % Sec(1).count());
        ss << "." << msec.count();
        //ss << (std::chrono::duration<Ms>(now).count() % Sec(1));

        return ss.str();

        //SYSTEMTIME st;
        //::GetLocalTime(&st);
        //return string_format::format("%04d-%02d-%02d %02d:%02d:%02d.%03d",
        //    st.wYear, st.wMonth, st.wDay,
        //    st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    }

    inline
        std::string GetCurrentTimeEx()
    {
        Clock::time_point now = Clock::now();
        std::time_t now_c = Clock::to_time_t(now);

        tm now_tm;
        int err = localtime_s(&now_tm, &now_c);
        std::stringstream ss;
        ss << std::put_time(&now_tm, "%H:%M:%S"); // HH:MM:SS

        Ms ms = std::chrono::duration_cast<Ms>(now.time_since_epoch());
        ms %= Sec(1).count();
        ss << "." << ms.count();
        //ss << (std::chrono::duration<Ms>(now).count() % Sec(1));

        return ss.str();

        //SYSTEMTIME st;
        //::GetLocalTime(&st);
        //return string_format::format("%02d:%02d:%02d.%03d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    }

    inline
        std::string GetCurrentLogHour()
    {
        Clock::time_point now = Clock::now();
        std::time_t now_c = Clock::to_time_t(now);

        tm now_tm;
        int err = localtime_s(&now_tm, &now_c);
        std::stringstream ss;
        ss << std::put_time(&now_tm, "%Y-%m-%d"); // yyyy-mm-dd HH:MM:SS

        return ss.str();

        //SYSTEMTIME st;
        //::GetLocalTime(&st);
        //return string_format::format("%04d%02d%02d%02d",
        //    st.wYear, st.wMonth, st.wDay,
        //    st.wHour);
    }

    struct ElapsedTimer
    {
        Ms elapsed() const {
            auto end = Clock::now();
            return std::chrono::duration_cast<Ms>(end - start);// .count();
        }
        bool expired(Ms timeout) const
        {
            return timeout <= elapsed();
        }
        Ms rest(Ms timeout) const
        {
            if (timeout.count() != (Ms::max)().count())
                return timeout - std::min<Ms>(elapsed(), timeout);
            return timeout;
        }
        void reset() { start = Clock::now(); }
        Clock::time_point start = Clock::now();
    };
}


