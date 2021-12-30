
#ifdef NDEBUG
#define REAPERAPI_MINIMAL
#define REAPERAPI_WANT_GetSelectedTrack2
#define REAPERAPI_WANT_ShowConsoleMsg
#define REAPERAPI_WANT_TrackFX_GetCount
#define REAPERAPI_WANT_TrackFX_GetFXGUID
#define REAPERAPI_WANT_TrackFX_GetFXName
#define REAPERAPI_WANT_TrackFX_GetFormattedParamValue
#define REAPERAPI_WANT_TrackFX_GetNumParams
#define REAPERAPI_WANT_TrackFX_GetOpen
#define REAPERAPI_WANT_TrackFX_GetParam
#define REAPERAPI_WANT_TrackFX_GetParamEx
#define REAPERAPI_WANT_TrackFX_GetParamName
#define REAPERAPI_WANT_TrackFX_GetParameterStepSizes
#define REAPERAPI_WANT_TrackFX_SetParam
#define REAPERAPI_WANT_TrackFX_Show
#define REAPERAPI_WANT_plugin_getapi
#define REAPERAPI_WANT_plugin_register
#endif

#define REAPERAPI_IMPLEMENT
#include <reaper_plugin_functions.h>
#define REAIMGUIAPI_IMPLEMENT
#include <reaper_imgui_functions.h>

#include "ReaFab.hpp"

extern "C" {
REAPER_PLUGIN_DLL_EXPORT int ReaperPluginEntry(
    REAPER_PLUGIN_HINSTANCE hInstance,
    reaper_plugin_info_t* rec)
{
    (void)hInstance;
    if (!rec) {
        if (ImGui_GetVersion) {
            reafab::RegisterImGui(true);
        }
        reafab::Register(false);
        return 0;
    }
    else if (
        rec->caller_version != REAPER_PLUGIN_VERSION ||
        REAPERAPI_LoadAPI(rec->GetFunc)) {
        return 0;
    }

    plugin_getapi = reinterpret_cast<decltype(plugin_getapi)>(
        rec->GetFunc("plugin_getapi")); // used by reaper_imgui_functions.h

    if (ImGui_GetVersion) {
        reafab::RegisterImGui(true);
    }
    reafab::Register(true);
    return 1;
}
}
