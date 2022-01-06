#include "reafab.hpp"

namespace reafab {

ImGui_Context* ctx;
int imguiActionId {-1};

static void ImGuiFrame()
{
    std::scoped_lock<std::mutex> lock {m};
    Initialize();
    if (!isValid()) {
        return;
    }
    int& fx = fxIndices[currentFXindex];
    TrackFX_GetFXName(track, fx, bufOut, BUFSIZ);
    std::string fx_name(bufOut);

    std::string text = "track ";
    text +=
        std::to_string((int)GetMediaTrackInfo_Value(track, "IP_TRACKNUMBER"));
    text += std::string(": ");
    GetTrackName(track, bufOut, BUFSIZ);
    text += std::string(bufOut);
    ImGui_Text(ctx, text.c_str());

    text = std::string("fx: ");
    text += fx_name;
    ImGui_Text(ctx, text.c_str());

    std::map<int, ControlTarget> currentControlMap;
    for (auto& [key, value] : controlMap) {
        if (fx_name.find(key) != std::string::npos) {
            currentControlMap = value;
            break;
        }
    }
    for (auto& [key, value] : currentControlMap) {
        (void)key;
        if (value.bands > 0) {
            GUID* fx_guid = TrackFX_GetFXGUID(track, fx);
            int band = 1;
            if (previousBand[fx_guid]) {
                band = previousBand[fx_guid];
            }
            if (value.paramId.find("00") != std::string::npos) {
                value.paramId.replace(
                    value.paramId.find("00"),
                    2,
                    std::to_string(band));
            }
        }
    }
    if (ImGui_BeginTable(ctx, "table1", 9)) {
        for (int row = 0; row < 4; row++) {
            ImGui_TableNextRow(ctx);
            for (int column = 0; column < 9; column++) {
                ImGui_TableSetColumnIndex(ctx, column);
                std::string text = currentControlMap[row * 8 + column].paramId;
                if (currentControlMap[row * 8 + column].paramId.empty()) {
                    text = std::string("");
                }
                if (column == 0) {
                    if (row == 0) {
                        text = std::string("enc1-8: ");
                    }
                    if (row == 1) {
                        text = std::string("encp1-8: ");
                    }
                    if (row == 2) {
                        text = std::string("b1-8: ");
                    }
                }
                ImGui_Text(ctx, text.c_str());
            }
        }
        ImGui_TableSetColumnIndex(ctx, 0);
        text = "b9-16: ";
        ImGui_Text(ctx, text.c_str());
        for (int column = 1; column < 9; column++) {
            ImGui_TableSetColumnIndex(ctx, column);
            text = std::string("fx") + std::to_string(column);
            if ((int)fxIndices.size() > column - 1) {
                TrackFX_GetFXName(track, fxIndices[column - 1], bufOut, BUFSIZ);
                text = text + std::string(": ");
                text = text + std::string(bufOut);
            }
            ImGui_Text(ctx, text.c_str());
        }
        ImGui_EndTable(ctx);
    }
    return;
}

static void ImGuiLoop()
{
    if (!ctx)
        ctx = ImGui_CreateContext("ReaFab");

    int cond {ImGui_Cond_FirstUseEver};
    ImGui_SetNextWindowSize(ctx, 1024, 144, &cond);

    bool open {true};
    if (ImGui_Begin(ctx, "ReaFab Monitor", &open)) {
        ImGuiFrame();
        ImGui_End(ctx);
    }

    if (!open || !ImGui_ValidatePtr(ctx, "ImGui_Context*")) {
        plugin_register("-timer", reinterpret_cast<void*>(&ImGuiLoop));
        ImGui_DestroyContext(ctx);
        ctx = nullptr;
    }
}

static bool ImGuiCommandHook(
    KbdSectionInfo* sec,
    const int command,
    const int val,
    const int valhw,
    const int relmode,
    HWND hwnd)
{
    (void)sec;
    (void)val;
    (void)valhw;
    (void)relmode;
    (void)hwnd; // unused

    if (command == imguiActionId) {
        if (!ctx)
            plugin_register("timer", reinterpret_cast<void*>(&ImGuiLoop));
        return true;
    }

    return false;
}

void RegisterImGui(bool load)
{
    custom_action_register_t imGuiAction {
        0,
        "AK5K_REAFAB_MONITOR",
        "ReaFab Monitor",
        NULL};

    if (!load) {
        plugin_register(
            "-hookcommand2",
            reinterpret_cast<void*>(&ImGuiCommandHook));
        plugin_register("-custom_action", &imGuiAction);
    }
    else {
        imguiActionId = plugin_register("custom_action", &imGuiAction);
        plugin_register(
            "hookcommand2",
            reinterpret_cast<void*>(&ImGuiCommandHook));
    }
}

} // namespace reafab
