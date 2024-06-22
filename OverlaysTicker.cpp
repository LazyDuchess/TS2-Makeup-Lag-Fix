#include "pch.h"
#include "Hooking.h"
#include "OverlaysTicker.h"

OverlaysTicker* pInstance = nullptr;

// Address of nTSUICAS::cTSUICASComponentOverlays::OnTick
constexpr auto ONTICK_HOOK_ADDR = 0x0057a68e;
constexpr auto ONTICK_HOOK_END_ADDR = 0x0057A694;

bool __stdcall OnTick_Call() {
    if (pInstance == nullptr)
        return true;
    return pInstance->DoTick;
}

void __declspec(naked) OnTick_Hook()
{
    __asm {
        push eax
        push edx
        push ecx
        push edi
        push ebx
        push esi
        call OnTick_Call
        test al, al
        je ReturnEarly
        pop esi
        pop ebx
        pop edi
        pop ecx
        pop edx
        pop eax
        // Original
        push esi
        push edi
        push [esp+0xC]
        jmp ONTICK_HOOK_END_ADDR
        ReturnEarly:
        pop esi
        pop ebx
        pop edi
        pop ecx
        pop edx
        pop eax
        mov al, 0x1
        ret 0x4
    }
}

OverlaysTicker::OverlaysTicker() {
    pInstance = this;
    DoTick = false;
    Hooking::MakeJMP((BYTE*)ONTICK_HOOK_ADDR, (DWORD)OnTick_Hook, 6);
}