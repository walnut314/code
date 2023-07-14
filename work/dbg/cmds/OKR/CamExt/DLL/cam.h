#pragma once

// C/C++ standard headers
#include <cstdint>

class Kam
{
public:
    Kam();
    ~Kam();
    Kam& operator++();

    HRESULT Exec(
            _In_ PCHAR cmd);

    HRESULT Eval(
            _In_ PCHAR cmd);

private:
    std::uint64_t m_Progress;
};

