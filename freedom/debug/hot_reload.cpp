#include <windows.h>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"

#define EXPORT __declspec(dllexport)

extern "C" EXPORT void hr_update_ui()
{
    ImGui::Text("Sample Text Updated!");
}

extern "C" EXPORT void hr_set_imgui_context(void *ctx)
{
    ImGui::SetCurrentContext((ImGuiContext *)ctx);
}

extern "C" BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}
