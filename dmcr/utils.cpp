#include "pch.h"
#include "utils.h"

#include <Windows.h>

void* ByPtr(DWORD base, DWORD offset, ...)
{
    DWORD a = (base + offset);
    DWORD tmp;
    va_list vl;
    va_start(vl, offset);
    while (true)
    {
        tmp = va_arg(vl, DWORD);
        if (tmp == -1) break;

        if (IsBadWritePtr((DWORD*)a, 4)) {
            a = 0;
            break;
        }

        a = *(DWORD*)a;
        if (a == 0) {
            break;
        }
        a += tmp;
    }
    va_end(vl);
    return (void*)a;
}