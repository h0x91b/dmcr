// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
//#include <stdio.h>
#include <iostream>

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */
#include "dmcr.exe.h"
#pragma pack(pop)   /* restore original alignment from stack */

#pragma region asserts
static_assert(sizeof(Unit2) == 0x1be9, "Wrong size of Unit2 struct");
static_assert(offsetof(struct Unit, field_0x28) == 0x28, "Wrong offset of field_0x28 in Unit struct");
static_assert(offsetof(struct Unit, field_0x46) == 0x46, "Wrong offset of field_0x46 in Unit struct");
static_assert(offsetof(struct Unit, field_0x4f) == 0x4f, "Wrong offset of field_0x4f in Unit struct");
static_assert(offsetof(struct Unit, field_0x6a) == 0x6a, "Wrong offset of field_0x6a in Unit struct");
static_assert(sizeof(Unit) == 0x104, "Wrong size of Unit struct");
#pragma endregion

DWORD WINAPI MainThread(HMODULE hModule) {
    OutputDebugString(L"MainThread");

    AllocConsole();
    FILE* fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);

    std::cout << "This works" << std::endl;

    while (!GetAsyncKeyState(VK_END)) {
        Sleep(50);
    }

    Unit** unit = (Unit**)(0x00fa5ac0);
    for (int n = 0; n < 65535; n++) {
        Unit* u = unit[n];
        if (u != NULL && /*u->tickAfterKill == 0 &&*/ u->unit2->type == UNIT_PEASANT && u->action == NULL) {
            std::cout << "Unit is peasant u->tickAfterKill " << u->tickAfterKill << " and his id: " << u->id << std::endl;
        }
        else if (u != NULL /* && u->tickAfterKill == 0*/ && u->unit2->type != UNIT_PEASANT) {
            std::cout << "Unit type is different "<< (int)u->unit2->type <<" and his id: " << u->id << std::endl;
        }
    }

    OutputDebugString(L"Dettach and shutdown everything\r\n");
    std::cout << "Dettach and shutdown everything" << std::endl;
    FreeLibraryAndExitThread(hModule, 0);
}



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, 0));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

