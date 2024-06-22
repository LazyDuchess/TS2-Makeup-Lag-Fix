#include "pch.h"
#include "Hooking.h"
#include "UIEventWatcher.h"

#define QUEUE_TICKS
#define TICKS_TO_QUEUE 10000
#define CLEAR_QUEUE_EVERY_FRAME

UIEventWatcher* pInstance = nullptr;

// POTENTIAL ADDRESSES:
// 0x00578b2c - TOO LAGGY
// 0x0064655d - Triggers when entering jewelry and full face makeup, multiple times at once. Promising? - TOO LAGGY
// 0x005779e0 - Triggers like above but not as often. dunno.
// 0x0057521e - Triggers when hovering over certain UI elements.

// 0x00577aaa - nTSUICAS::cTSUICASComponentOverlays::DoMessage
// 0x00cf3930 - Triggers a lot in jewelry.

// Address of nTSUICAS::cTSUICASComponentOverlays::Activate
constexpr auto ACTIVATE_HOOK_ADDR = 0x0057a6c1;
constexpr auto ACTIVATE_HOOK_END_ADDR = 0x0057A6C6;

// Address of nTSUICAS::cTSUICASComponentOverlays::UNKNOWN.
constexpr auto UNKNOWN_HOOK_ADDR = 0x0064655d;
constexpr auto UNKNOWN_HOOK_END_ADDR = 0x0064AB45;

// Address of nTSUICAS::cTSUICASComponentOverlays::DoMessage
constexpr auto DOMESSAGE_HOOK_ADDR = 0x00577aaa;
constexpr auto DOMESSAGE_HOOK_END_ADDR = 0x00577AB0;

// Address of nTSSG::cTSSGSystem::OncePerFrameUpdate
constexpr auto ONCEPERFRAMEUPDATE_HOOK_ADDR = 0x00B271D6;
constexpr auto ONCEPERFRAMEUPDATE_HOOK_END_ADDR = 0x00B271DD;

void __stdcall QueueTick() {
	if (pInstance == nullptr) return;
	pInstance->QueueTick();
}

void __stdcall ClearQueue() {
    if (pInstance == nullptr) return;
    pInstance->ClearQueue();
}

void __declspec(naked) DoMessage_Hook()
{
    __asm {
        push eax
        push edx
        push ecx
        push edi
        push ebp
        push esi
        call QueueTick
        pop esi
        pop ebp
        pop edi
        pop ecx
        pop edx
        pop eax
        // Original
        push ebp
        mov ebp, esp
        sub esp, 0x10
        jmp DOMESSAGE_HOOK_END_ADDR
    }
}

void __declspec(naked) Unknown_Hook()
{
    __asm {
        push eax
        push edx
        push ecx
        push edi
        push ebx
        push esi
        call QueueTick
        pop esi
        pop ebx
        pop edi
        pop ecx
        pop edx
        pop eax
        // Original
        jmp UNKNOWN_HOOK_END_ADDR
    }
}

void __declspec(naked) Activate_Hook()
{
    __asm {
        push eax
        push edx
        push ecx
        push edi
        push ebx
        push esi
        call QueueTick
        pop esi
        pop ebx
        pop edi
        pop ecx
        pop edx
        pop eax
        // Original
        mov eax, 0x01144C97
        jmp ACTIVATE_HOOK_END_ADDR
    }
}

void __declspec(naked) OncePerFrameUpdate_Hook()
{
    __asm {
        push esp
        push eax
        push edx
        push ecx
        push edi
        push ebx
        push esi
        call ClearQueue
        pop esi
        pop ebx
        pop edi
        pop ecx
        pop edx
        pop eax
        pop esp
        // Original
        push -01
        push 0x0118B3D0
        jmp ONCEPERFRAMEUPDATE_HOOK_END_ADDR
    }
}

UIEventWatcher::UIEventWatcher(OverlaysTicker* ticker) {
	pInstance = this;
	_ticker = ticker;
    Hooking::MakeJMP((BYTE*)ACTIVATE_HOOK_ADDR, (DWORD)Activate_Hook, 5);
    Hooking::MakeJMP((BYTE*)DOMESSAGE_HOOK_ADDR, (DWORD)DoMessage_Hook, 6);
    Hooking::MakeJMP((BYTE*)UNKNOWN_HOOK_ADDR, (DWORD)Unknown_Hook, 5);
#ifdef CLEAR_QUEUE_EVERY_FRAME
    Hooking::MakeJMP((BYTE*)ONCEPERFRAMEUPDATE_HOOK_ADDR, (DWORD)OncePerFrameUpdate_Hook, 7);
#endif
}

void UIEventWatcher::QueueTick() {
#ifdef  QUEUE_TICKS
    _ticker->QueuedTicks += TICKS_TO_QUEUE;
#else
    _ticker->QueuedTicks = TICKS_TO_QUEUE;
#endif //  QUEUE_TICKS
}

void UIEventWatcher::ClearQueue() {
    _ticker->QueuedTicks = 0;
}