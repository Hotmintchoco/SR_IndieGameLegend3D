#pragma once

#include "Engine_Define.h"

namespace Utils
{
    inline wstring Utf8ToWide(const string& s)
    {
        if (s.empty()) return {};
        int len = ::MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), nullptr, 0);
        wstring w(len, L'\0');
        ::MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), w.data(), len);
        return w;
    }

    inline string WideToUtf8(const wstring& w)
    {
        if (w.empty()) return {};
        int len = ::WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), nullptr, 0, nullptr, nullptr);
        string s(len, '\0');
        ::WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), s.data(), len, nullptr, nullptr);
        return s;
    }
}