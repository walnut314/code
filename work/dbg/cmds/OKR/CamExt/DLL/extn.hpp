#pragma once

// C/C++ standard headers
#include <cstdint>

class Extension
{
public:
    Extension(PDEBUG_CLIENT DebugClient)
        : m_Progress(0)
    {
        HRESULT Status = S_OK;
        if ((Status = QueryInterfaces(DebugClient)) != S_OK) {
        }
        m_DebugControl->Output(DEBUG_OUTPUT_NORMAL, "Extension:ctor\n");
    }
    ~Extension()
    {
        m_DebugControl->Output(DEBUG_OUTPUT_NORMAL, "Extension:dtor\n");
        ReleaseInterfaces();
    }

    HRESULT Exec(
            _In_ PCHAR cmd)
    {
        HRESULT Status = S_OK;
        Status = m_DebugControl->Execute(DEBUG_OUTCTL_ALL_CLIENTS, cmd, DEBUG_EXECUTE_ECHO);
        return Status;
    }

    HRESULT Eval(
            _In_ PCHAR expr,
            _Inout_ PDEBUG_VALUE val)
    {
        HRESULT Status = S_OK;

        __try {
            if ((Status = m_DebugControl->SetExpressionSyntax(DEBUG_EXPR_CPLUSPLUS))      != S_OK) { return Status; }
            if ((Status = m_DebugControl->Evaluate(expr, DEBUG_VALUE_INVALID, val, NULL)) != S_OK) { return Status; }
            if ((Status = m_DebugControl->SetExpressionSyntax(DEBUG_EXPR_MASM))           != S_OK) { return Status; }
            m_DebugControl->Output(DEBUG_OUTPUT_NORMAL, "Val: 0x%I64x\n", val->I64);
        }

        __finally {
            return Status;
        }
    }

    HRESULT
    Out(
        _In_ PCHAR str
       )
    {
        m_DebugControl->Output(DEBUG_OUTPUT_NORMAL, str);
    }

    HRESULT
    QueryInterfaces(
        _In_ PDEBUG_CLIENT DebugClient
        ) {
        HRESULT Status = S_OK;
        if ((Status = DebugClient->QueryInterface(__uuidof(IDebugControl),      (PVOID *)&m_DebugControl))    != S_OK) { return Status; }
        if ((Status = DebugClient->QueryInterface(__uuidof(IDebugSymbols),      (PVOID *)&m_DebugSymbols))    != S_OK) { return Status; }
        if ((Status = DebugClient->QueryInterface(__uuidof(IDebugDataSpaces),   (PVOID *)&m_DebugDataSpaces)) != S_OK) { return Status; }
        return Status;
    }

    VOID
    ReleaseInterfaces(
        VOID
        )
    {
        if (m_DebugDataSpaces)  { m_DebugDataSpaces->Release(); m_DebugDataSpaces   = NULL; }
        if (m_DebugSymbols)     { m_DebugSymbols->Release();    m_DebugSymbols      = NULL; }
        if (m_DebugControl)     { m_DebugControl->Release();    m_DebugControl      = NULL; }
    }

private:
    std::uint64_t m_Progress;
    PDEBUG_CONTROL4         m_DebugControl;
    PDEBUG_SYMBOLS3         m_DebugSymbols;
    PDEBUG_DATA_SPACES4     m_DebugDataSpaces;

};

