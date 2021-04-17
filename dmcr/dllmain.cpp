#include "pch.h"
//#include <stdio.h>
#include <iostream>
#include "utils.h"

#include <detours.h>
#pragma comment(lib, "detours.lib")

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

#pragma region consts
Unit** allUnits = (Unit**)0x00fa5ac0;
void* fnGatherResources = (void*)0x004d5ac0;
void* fnPeasantIdleAnimation = (void*)0x004accb0;
byte* playersIds = (byte*)0x005adb3c;
byte* currentPlayerId = (byte*)0x010c1598;
int* resPeasants = (int*)0x02717080;
int* goldPeasants = (int*)0x026d1544;
bool* GameInProgress = (bool*)0x005e89d0;
HWND* hWindow = (HWND*)0x007ffee4;
void* SmallWhiteFont = (void*)0x0081d7b4;
void* BlackFont = (void*)0x0081d9e0;
void* BigBlackFont = (void*)0x0081d964;
void* BigRedFont = (void*)0x008005ec;
int* population = (int*)0x00ff13b2;
int* maxPopulation = (int*)0x00ff13b6;
#pragma endregion

#pragma region funcs
// 00463770 void __cdecl ShowString(int x, int y, char* text, RLCFont* font)
typedef void(__cdecl* ShowString) (int , int , char*, void*);
ShowString pShowString = (ShowString)0x00463770;

typedef void(__fastcall* RefreshView) (void*, DWORD);
RefreshView RealRefreshView = (RefreshView)0x0045d770;

size_t CountPeasantsOnRes(int playerNumber, GATHER_RES_TYPE type);
size_t CountOfFreePeasants(int playerNumber);
size_t CountOfWarriors(int playerNumber);
size_t CountOfFreeMines(int playerNumber);
#pragma endregion

void __fastcall _RefreshView(void* _this, DWORD edx) {
    // printf("_RefreshView\n");
    if (*GameInProgress) {
        char buf[256];
        int y = 80;
        int vSpacing = 12;

        sprintf_s(buf, "��������� %d / %d", *population, *maxPopulation);
        pShowString(20, y += vSpacing, buf, SmallWhiteFont);
        
        sprintf_s(buf, "������: %d", CountOfFreePeasants((*currentPlayerId) ^ 0x85));
        pShowString(20, y += vSpacing, buf, SmallWhiteFont);
        
        sprintf_s(buf, "�������� �����: %d", CountOfFreeMines((*currentPlayerId) ^ 0x85));
        pShowString(20, y += vSpacing, buf, SmallWhiteFont);
        
        sprintf_s(buf, "�����: %d", CountOfWarriors((*currentPlayerId) ^ 0x85));
        pShowString(20, y += vSpacing, buf, SmallWhiteFont);

        //pShowString(200, 300, (char*)"Hello world", SmallWhiteFont);
        //pShowString(200, 330, (char*)"Hello ������", BigRedFont);

        CountPeasantsOnRes((*currentPlayerId) ^ 0x85, GATHER_FOOD);
        CountPeasantsOnRes((*currentPlayerId) ^ 0x85, GATHER_WOOD);
        CountPeasantsOnRes((*currentPlayerId) ^ 0x85, GATHER_STONE);
    }
    RealRefreshView(_this, edx);
}

size_t CountOfFreePeasants(int playerNumber) {
    size_t res = 0;
    for (int n = 0; n < 65535; n++) {
        Unit* u = allUnits[n];
        if (
            u != NULL
            && u->tickAfterKill == 0
            && u->unit2->type == UNIT_PEASANT
            && (
                u->unitAction == NULL || u->unitAction->fn == fnPeasantIdleAnimation
            ) && u->owner == playersIds[playerNumber]
        ) {
            res++;
        }
    }
    return res;
}

size_t CountOfWarriors(int playerNumber) {
    size_t res = 0;
    for (int n = 0; n < 65535; n++) {
        Unit* u = allUnits[n];
        if (
            u != NULL
            && u->tickAfterKill == 0
            && u->owner == playersIds[playerNumber]
            && ((u->mask2 & 0x10) == 0)
            && (u->unit2->field_0xa1f & 2) == 0
            && (u->field_0x52 == '\0')
        ) {
            res++;
        }
    }
    return res;
}

size_t CountOfFreeMines(int playerNumber) {
    size_t res = 0;
    for (int n = 0; n < 65535; n++) {
        Unit* u = allUnits[n];
        //(((u->mask3 & 1) != 0 &&
        //    ((uint)u->peasantsIn <
        //        (uint)(ushort)u->building->bs1->baseCapacity + (uint)u->upgradeCapacity))))
        if (
            u != NULL
            && u->tickAfterKill == 0
            && u->owner == playersIds[playerNumber]
            && u->unit2->type == UNIT_MINE
            && (u->mask3 & 1) != 0
            && u->peasantsIn < u->building->bs1->baseCapacity + u->upgradeCapacity
        ) {
            res++;
        }
    }
    return res;
}

size_t CountPeasantsOnRes(int playerNumber, GATHER_RES_TYPE type) {
    size_t res = 0;
    for (int n = 0; n < 65535; n++) {
        Unit* u = allUnits[n];
        if (
            u != NULL 
            && u->tickAfterKill == 0 
            && u->unit2->type == UNIT_PEASANT 
            && u->unitAction != NULL
            && u->unitAction->fn == fnGatherResources
            && u->unitAction->resType == type
            && u->owner == playersIds[playerNumber]
        ) {
            res++;
        }
    }
    switch (type)
    {
    case GATHER_FOOD:
        resPeasants[3] = res; break;
    case GATHER_WOOD:
        resPeasants[0] = res; break;
    case GATHER_STONE:
        resPeasants[2] = res; break;
    default:
        break;
    }
    resPeasants[1] = *goldPeasants; // gold
    resPeasants[5] = *(goldPeasants + 8); // +8 coal
    resPeasants[4] = *(goldPeasants + 16); // +16 iron

    return res;
}

DWORD WINAPI MainThread(HMODULE hModule) {
    OutputDebugString(L"MainThread");

    AllocConsole();
    FILE* fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);

    std::cout << "Console window activated, press HOME to activate detour" << std::endl;

    while (!GetAsyncKeyState(VK_HOME)) {
        Sleep(50);
    }

    DetourRestoreAfterWith();

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    auto csemu = LoadLibrary(L"csemu.dll"); // 0318c 3177
    DetourAttach(&(PVOID&)RealRefreshView, _RefreshView);

    DetourTransactionCommit();

    while (!GetAsyncKeyState(VK_END)) {
        if (*GameInProgress) {
            //std::cout << "==================================" << std::endl;
            //std::cout << "Peasants on food " << CountPeasantsOnRes((*currentPlayerId) ^ 0x85, GATHER_FOOD) << std::endl;
            //std::cout << "Peasants on wood " << CountPeasantsOnRes((*currentPlayerId) ^ 0x85, GATHER_WOOD) << std::endl;
            //std::cout << "Peasants on stone " << CountPeasantsOnRes((*currentPlayerId) ^ 0x85, GATHER_STONE) << std::endl;
        }
        Sleep(250);
    }

    //for (int n = 0; n < 65535; n++) {
    //    Unit* u = allUnits[n];
    //    if (u != NULL && /*u->tickAfterKill == 0 &&*/ u->unit2->type == UNIT_PEASANT && u->unitAction == NULL) {
    //        std::cout << "Unit is peasant u->tickAfterKill " << u->tickAfterKill << " and his id: " << u->id << std::endl;
    //    }
    //    else if (u != NULL /* && u->tickAfterKill == 0*/ && u->unit2->type != UNIT_PEASANT) {
    //        std::cout << "Unit type is different "<< (int)u->unit2->type <<" and his id: " << u->id << std::endl;
    //    }
    //}

    OutputDebugString(L"Dettach and shutdown everything\r\n");
    std::cout << "Dettach and shutdown everything" << std::endl;

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    // DetourDetach(&(PVOID&)RealFlip, _RefreshView);
    DetourDetach(&(PVOID&)RealRefreshView, _RefreshView);

    DetourTransactionCommit();

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

