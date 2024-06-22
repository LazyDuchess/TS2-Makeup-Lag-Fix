#include "pch.h"
#include "Hooking.h"
#include "UIEventWatcher.h"
#include "cGZMessage.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>

// Uncomment to debug cRZMessage input.
//#define DEBUG_MESSAGES

UIEventWatcher* pInstance = nullptr;

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

#ifdef DEBUG_MESSAGES
    int lastMessage = 0;
    int lastUnknown = 0;
#endif

// Only tick on mouse press events.
bool ShouldTickOnMessage(cGZMessage* message) {
    if (message->MessageID == 0x3) {
        if (message->Unknown == 0x287259f6 || message->Unknown == 0x28759f7 || message->Unknown == 0x28759f8)
            return true;
    }
    return false;
}

void __stdcall QueueTickMessage(cGZMessage* message) {
#ifdef DEBUG_MESSAGES
    if (message->MessageID != lastMessage || message->Unknown != lastUnknown) {
        wprintf(L"Received new message: 0x");
    }
    else
        wprintf(L"Repeated message: 0x");
    lastMessage = message->MessageID;
    lastUnknown = message->Unknown;

    std::wostringstream hexs;  // note the 'w'
    hexs << std::hex << lastMessage;

    std::wstring hex = hexs.str();

    std::wostringstream hexs2;  // note the 'w'
    hexs2 << std::hex << lastUnknown;

    std::wstring hex2 = hexs2.str();

    wprintf(hex.c_str());
    wprintf(L", 0x");
    wprintf(hex2.c_str());
    wprintf(L"\n");
#endif
    if (pInstance == nullptr) 
        return;

    if (ShouldTickOnMessage(message)) {
#ifdef DEBUG_MESSAGES
        wprintf(L"Ticking!\n");
#endif
        pInstance->QueueTick();
    }
}

void __declspec(naked) DoMessage_Hook()
{
    __asm {
        mov eax, [esp+0x4]
        //push eax
        push edx
        push ecx
        push edi
        push ebp
        push esi
        push eax
        call QueueTickMessage
        pop esi
        pop ebp
        pop edi
        pop ecx
        pop edx
        //pop eax
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

// Tick only in 1 frame, to avoid lag.
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
    Hooking::MakeJMP((BYTE*)ONCEPERFRAMEUPDATE_HOOK_ADDR, (DWORD)OncePerFrameUpdate_Hook, 7);
#ifdef DEBUG_MESSAGES
    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
#endif
}

void UIEventWatcher::QueueTick() {
    _ticker->DoTick = true;
}

void UIEventWatcher::ClearQueue() {
    _ticker->DoTick = false;
}