#include <cmath>
#include <map>
#include <string>
#include <vector>

#define REAPERAPI_IMPLEMENT
#include <reaper_plugin_functions.h>

double step = 0.0013123359531164;

std::map<int, int> actions;
std::map<GUID*, int> previousBand;
std::vector<int> pluginIndices;

int currentPlugin {-1};

std::vector<std::string> pluginNames {
    "Pro-Q 3",
    "Pro-C 2",
    "Pro-G",
    "Pro-L 2",
    "Pro-MB",
    "Pro-DS",
    "Saturn 2"};

void ProcessProMB(int command, int val, int index, MediaTrack* track)
{
    char bufOut[64] {0};
    GUID* fx_guid = TrackFX_GetFXGUID(track, index);
    int band = 1;
    double paramValue = 0.0;
    if (previousBand[fx_guid]) {
        band = previousBand[fx_guid];
    }
    // select previous band
    if (command == 17) {
        band = band - 1;
        if (band < 1) {
            band = 1;
        }
    }
    // select next band
    if (command == 18) {
        band = band + 1;
        if (band > 6) {
            band = 6;
        }
    }

    previousBand[fx_guid] = band;
    std::string targetString("Band ");
    targetString = targetString + std::to_string(band) + std::string(" ");

    if (command == 1) {
        targetString = targetString + std::string("Threshold");
    }
    if (command == 2) {
        targetString = targetString + std::string("Ratio");
    }
    if (command == 3) {
        targetString = targetString + std::string("Attack");
    }
    if (command == 4) {
        targetString = targetString + std::string("Release");
    }
    if (command == 5) {
        targetString = targetString + std::string("Knee");
    }
    if (command == 6) {
        targetString = targetString + std::string("Range");
    }
    if (command == 7) {
        targetString = targetString + std::string("High Crossover");
    }
    if (command == 8) {
        targetString = targetString + std::string("Level");
    }
    if (command == 19) {
        targetString = targetString + std::string("Dynamics Mode");
    }
    // if (command == 23 || command == 24) {
    //     targetString = std::string("Num Active Bands");
    //     val = 1;
    //     if (command == 23) {
    //         val = -1;
    //     }
    // }

    for (int i = 0; i < TrackFX_GetNumParams(track, index); i++) {
        TrackFX_GetParamName(track, index, i, bufOut, 64);
        std::string paramName(bufOut);
        if (paramName.compare(targetString) == 0) {
            double oldValue = TrackFX_GetParam(track, index, i, 0, 0);
            double stepOut, smallStepOut, largeStepOut;
            bool isToggleOut {false};
            bool isStepped = TrackFX_GetParameterStepSizes(
                track,
                index,
                i,
                &stepOut,
                &smallStepOut,
                &largeStepOut,
                &isToggleOut);
            if (isStepped) {
                paramValue = val * stepOut;
            }
            else {
                if (val < 64) {
                    paramValue = val * step;
                }
                if (val > 64) {
                    val = val - 128;
                    paramValue = val * step;
                }
                if (val < -1 || val > 1) {
                    paramValue = paramValue * 3;
                }
            }
            if (isToggleOut) {
                paramValue = 0.0;
                oldValue = abs(oldValue - 1.0);
            }
            TrackFX_SetParam(track, index, i, oldValue + paramValue);
        }
    }
    return;
}
void ProcessSaturn2(int command, int val, int index, MediaTrack* track)
{
    char bufOut[64] {0};
    GUID* fx_guid = TrackFX_GetFXGUID(track, index);
    int band = 1;
    double paramValue = 0.0;
    if (previousBand[fx_guid]) {
        band = previousBand[fx_guid];
    }
    // select previous band
    if (command == 17) {
        band = band - 1;
        if (band < 1) {
            band = 1;
        }
    }
    // select next band
    if (command == 18) {
        band = band + 1;
        if (band > 6) {
            band = 6;
        }
    }

    previousBand[fx_guid] = band;
    std::string targetString("Band ");
    targetString = targetString + std::to_string(band) + std::string(" ");
    // int targetParamIndex {-1};

    if (command == 1) {
        targetString = targetString + std::string("Drive");
    }
    if (command == 2) {
        targetString = targetString + std::string("Dynamics");
    }
    if (command == 3) {
        targetString = targetString + std::string("Crossover Frequency");
    }
    if (command == 7) {
        targetString = targetString + std::string("Level");
    }
    if (command == 8) {
        targetString = targetString + std::string("Mix");
    }
    // if (command == 17 || command == 18) {
    //     targetString = targetString + std::string("Enabled");
    //     val = 1;
    // }
    if (command == 19 || command == 20) {
        targetString = targetString + std::string("Style");
        val = 1;
        if (command == 19) {
            val = -1;
        }
    }
    if (command == 23 || command == 24) {
        targetString = std::string("Num Active Bands");
        val = 1;
        if (command == 23) {
            val = -1;
        }
    }

    for (int i = 0; i < TrackFX_GetNumParams(track, index); i++) {
        TrackFX_GetParamName(track, index, i, bufOut, 64);
        std::string paramName(bufOut);
        if (paramName.compare(targetString) == 0) {
            double oldValue = TrackFX_GetParam(track, index, i, 0, 0);
            double stepOut, smallStepOut, largeStepOut;
            bool isToggleOut {false};
            bool isStepped = TrackFX_GetParameterStepSizes(
                track,
                index,
                i,
                &stepOut,
                &smallStepOut,
                &largeStepOut,
                &isToggleOut);
            if (isStepped) {
                paramValue = val * stepOut;
            }
            else {
                if (val < 64) {
                    paramValue = val * step;
                }
                if (val > 64) {
                    val = val - 128;
                    paramValue = val * step;
                }
                if (val < -1 || val > 1) {
                    paramValue = paramValue * 3;
                }
            }
            if (isToggleOut) {
                paramValue = 0.0;
                oldValue = abs(oldValue - 1.0);
            }
            TrackFX_SetParam(track, index, i, oldValue + paramValue);
        }
    }
    return;
}

void ProcessProDS(int command, int val, int index, MediaTrack* track)
{
    char bufOut[64] {0};
    double paramValue = 0.0;
    std::string targetString {""};
    // int targetParamIndex {-1};

    if (command == 1) {
        targetString = std::string("Threshold");
    }
    if (command == 2) {
        targetString = std::string("Range");
    }
    if (command == 3) {
        targetString = std::string("High-Pass Frequency");
    }
    if (command == 4) {
        targetString = std::string("Low-Pass Frequency");
    }
    if (command == 5) {
        targetString = std::string("Stereo Link");
    }
    if (command == 6) {
        targetString = std::string("Lookahead");
    }
    if (command == 17) {
        targetString = std::string("Mode");
    }
    if (command == 18) {
        targetString = std::string("Band Processing");
    }
    if (command == 19) {
        targetString = std::string("Stereo Link Mode");
    }
    if (command == 20) {
        targetString = std::string("Lookahead Enabled");
    }
    for (int i = 0; i < TrackFX_GetNumParams(track, index); i++) {
        TrackFX_GetParamName(track, index, i, bufOut, 64);
        std::string paramName(bufOut);
        if (paramName.compare(targetString) == 0) {
            double oldValue = TrackFX_GetParam(track, index, i, 0, 0);
            double stepOut, smallStepOut, largeStepOut;
            bool isToggleOut {false};
            bool isStepped = TrackFX_GetParameterStepSizes(
                track,
                index,
                i,
                &stepOut,
                &smallStepOut,
                &largeStepOut,
                &isToggleOut);
            if (isStepped) {
                paramValue = val * stepOut;
            }
            else {
                if (val < 64) {
                    paramValue = val * step;
                }
                if (val > 64) {
                    val = val - 128;
                    paramValue = val * step;
                }
                if (val < -1 || val > 1) {
                    paramValue = paramValue * 3;
                }
            }
            if (isToggleOut) {
                paramValue = 0.0;
                oldValue = abs(oldValue - 1.0);
            }
            TrackFX_SetParam(track, index, i, oldValue + paramValue);
        }
    }
}
void ProcessProL2(int command, int val, int index, MediaTrack* track)
{
    char bufOut[64] {0};
    double paramValue = 0.0;
    std::string targetString {""};
    // int targetParamIndex {-1};

    if (command == 1) {
        targetString = std::string("Gain");
    }
    if (command == 2) {
        targetString = std::string("Output Level");
    }
    if (command == 3) {
        targetString = std::string("Attack");
    }
    if (command == 4) {
        targetString = std::string("Release");
    }
    if (command == 5) {
        targetString = std::string("Channel Link Transients");
    }
    if (command == 6) {
        targetString = std::string("Channel Link Release");
    }
    if (command == 7) {
        targetString = std::string("Lookahead");
    }

    if (command == 17 || command == 18) {
        targetString = std::string("Style");
        val = 1;
        if (command == 17) {
            val = -1;
        }
    }
    if (command == 19) {
        targetString = std::string("Unity Gain");
    }
    for (int i = 0; i < TrackFX_GetNumParams(track, index); i++) {
        TrackFX_GetParamName(track, index, i, bufOut, 64);
        std::string paramName(bufOut);
        if (paramName.compare(targetString) == 0) {
            double oldValue = TrackFX_GetParam(track, index, i, 0, 0);
            double stepOut, smallStepOut, largeStepOut;
            bool isToggleOut {false};
            bool isStepped = TrackFX_GetParameterStepSizes(
                track,
                index,
                i,
                &stepOut,
                &smallStepOut,
                &largeStepOut,
                &isToggleOut);
            if (isStepped) {
                paramValue = val * stepOut;
            }
            else {
                if (val < 64) {
                    paramValue = val * step;
                }
                if (val > 64) {
                    val = val - 128;
                    paramValue = val * step;
                }
                if (val < -1 || val > 1) {
                    paramValue = paramValue * 3;
                }
            }
            if (isToggleOut) {
                paramValue = 0.0;
                oldValue = abs(oldValue - 1.0);
            }
            TrackFX_SetParam(track, index, i, oldValue + paramValue);
        }
    }
}
void ProcessProC2(int command, int val, int index, MediaTrack* track)
{
    char bufOut[64] {0};
    double paramValue = 0.0;
    std::string targetString {""};
    // int targetParamIndex {-1};

    if (command == 1) {
        targetString = std::string("Threshold");
    }
    if (command == 2) {
        targetString = std::string("Ratio");
    }
    if (command == 3) {
        targetString = std::string("Attack");
    }
    if (command == 4) {
        targetString = std::string("Release");
    }
    if (command == 5) {
        targetString = std::string("Hold");
    }
    if (command == 6) {
        targetString = std::string("Knee");
    }
    if (command == 7) {
        targetString = std::string("Dry Gain");
    }
    if (command == 8) {
        targetString = std::string("Wet Gain");
    }

    if (command == 17 || command == 18) {
        targetString = std::string("Style");
        val = 1;
        if (command == 17) {
            val = -1;
        }
    }
    if (command == 19) {
        targetString = std::string("Auto Gain");
    }
    for (int i = 0; i < TrackFX_GetNumParams(track, index); i++) {
        TrackFX_GetParamName(track, index, i, bufOut, 64);
        std::string paramName(bufOut);
        if (paramName.compare(targetString) == 0) {
            double oldValue = TrackFX_GetParam(track, index, i, 0, 0);
            double stepOut, smallStepOut, largeStepOut;
            bool isToggleOut {false};
            bool isStepped = TrackFX_GetParameterStepSizes(
                track,
                index,
                i,
                &stepOut,
                &smallStepOut,
                &largeStepOut,
                &isToggleOut);
            if (isStepped) {
                paramValue = val * stepOut;
            }
            else {
                if (val < 64) {
                    paramValue = val * step;
                }
                if (val > 64) {
                    val = val - 128;
                    paramValue = val * step;
                }
                if (val < -1 || val > 1) {
                    paramValue = paramValue * 3;
                }
            }
            if (isToggleOut) {
                paramValue = 0.0;
                oldValue = abs(oldValue - 1.0);
            }
            TrackFX_SetParam(track, index, i, oldValue + paramValue);
        }
    }
}

void ProcessProG(int command, int val, int index, MediaTrack* track)
{
    char bufOut[64] {0};
    double paramValue = 0.0;
    std::string targetString {""};
    // int targetParamIndex {-1};

    if (command == 1) {
        targetString = std::string("Threshold");
    }
    if (command == 2) {
        targetString = std::string("Ratio");
    }
    TrackFX_GetFormattedParamValue(track, index, 5, bufOut, 64);
    std::string paramName(bufOut);
    if (paramName.compare("Upward") == 0 && (command == 1 || command == 2)) {
        targetString = targetString + std::string(" (Upward)");
    }
    if (command == 3) {
        targetString = std::string("Attack");
    }
    if (command == 4) {
        targetString = std::string("Release");
    }
    if (command == 5) {
        targetString = std::string("Hold");
    }
    if (command == 6) {
        targetString = std::string("Knee");
    }
    if (command == 7) {
        targetString = std::string("Range");
    }

    if (command == 17 || command == 18) {
        targetString = std::string("Style");
        val = 1;
        if (command == 17) {
            val = -1;
        }
    }
    for (int i = 0; i < TrackFX_GetNumParams(track, index); i++) {
        TrackFX_GetParamName(track, index, i, bufOut, 64);
        std::string paramName(bufOut);
        if (paramName.compare(targetString) == 0) {
            double oldValue = TrackFX_GetParam(track, index, i, 0, 0);
            double stepOut, smallStepOut, largeStepOut;
            bool isToggleOut {false};
            bool isStepped = TrackFX_GetParameterStepSizes(
                track,
                index,
                i,
                &stepOut,
                &smallStepOut,
                &largeStepOut,
                &isToggleOut);
            if (isStepped) {
                paramValue = val * stepOut;
            }
            else {
                if (val < 64) {
                    paramValue = val * step;
                }
                if (val > 64) {
                    val = val - 128;
                    paramValue = val * step;
                }
                if (val < -1 || val > 1) {
                    paramValue = paramValue * 3;
                }
            }
            TrackFX_SetParam(track, index, i, oldValue + paramValue);
        }
    }
}
void ProcessProQ3(int command, int val, int index, MediaTrack* track)
{
    char bufOut[64] {0};
    GUID* fx_guid = TrackFX_GetFXGUID(track, index);
    int band = 1;
    double paramValue = 0.0;
    if (previousBand[fx_guid]) {
        band = previousBand[fx_guid];
    }
    // select previous band
    if (command == 17) {
        band = band - 1;
        if (band < 1) {
            band = 1;
        }
    }
    // select next band
    if (command == 18) {
        band = band + 1;
        if (band > 24) {
            band = 24;
        }
    }

    previousBand[fx_guid] = band;
    std::string targetString("Band ");
    targetString = targetString + std::to_string(band) + std::string(" ");
    // int targetParamIndex {-1};

    if (command == 1) {
        targetString = targetString + std::string("Frequency");
    }
    if (command == 2) {
        targetString = targetString + std::string("Gain");
    }
    if (command == 3) {
        targetString = targetString + std::string("Q");
    }
    if (command == 4) {
        targetString = targetString + std::string("Dynamic Range");
    }
    if (command == 5) {
        targetString = targetString + std::string("Threshold");
    }
    if (command == 17 || command == 18) {
        targetString = targetString + std::string("Used");
        val = 1;
    }
    if (command == 19 || command == 20) {
        targetString = targetString + std::string("Shape");
        val = 1;
        if (command == 19) {
            val = -1;
        }
    }
    if (command == 21 || command == 22) {
        targetString = targetString + std::string("Slope");
        val = 1;
        if (command == 21) {
            val = -1;
        }
    }

    for (int i = 0; i < TrackFX_GetNumParams(track, index); i++) {
        TrackFX_GetParamName(track, index, i, bufOut, 64);
        std::string paramName(bufOut);
        if (paramName.compare(targetString) == 0) {
            double oldValue = TrackFX_GetParam(track, index, i, 0, 0);
            double stepOut, smallStepOut, largeStepOut;
            bool isToggleOut {false};
            bool isStepped = TrackFX_GetParameterStepSizes(
                track,
                index,
                i,
                &stepOut,
                &smallStepOut,
                &largeStepOut,
                &isToggleOut);
            if (isStepped) {
                paramValue = val * stepOut;
            }
            else {
                if (val < 64) {
                    paramValue = val * step;
                }
                if (val > 64) {
                    val = val - 128;
                    paramValue = val * step;
                }
                if (val < -1 || val > 1) {
                    paramValue = paramValue * 3;
                }
            }
            TrackFX_SetParam(track, index, i, oldValue + paramValue);
        }
    }
    return;
}

void ProcessPlugin(int command, int val, int index)
{
    auto track = GetSelectedTrack2(0, 0, true);
    char bufOut[64] {0};

    TrackFX_GetFXName(track, index, bufOut, 64);
    std::string fx_name(bufOut);
    if (fx_name.find(std::string("Pro-Q 3")) != std::string::npos) {
        ProcessProQ3(command, val, index, track);
    }
    if (fx_name.find(std::string("Pro-G")) != std::string::npos) {
        ProcessProG(command, val, index, track);
    }
    if (fx_name.find(std::string("Pro-C 2")) != std::string::npos) {
        ProcessProC2(command, val, index, track);
    }
    if (fx_name.find(std::string("Pro-L 2")) != std::string::npos) {
        ProcessProL2(command, val, index, track);
    }
    if (fx_name.find(std::string("Pro-DS")) != std::string::npos) {
        ProcessProDS(command, val, index, track);
    }
    if (fx_name.find(std::string("Saturn 2")) != std::string::npos) {
        ProcessSaturn2(command, val, index, track);
    }
    if (fx_name.find(std::string("Pro-MB")) != std::string::npos) {
        ProcessProMB(command, val, index, track);
    }
    return;
}

void ProcessInput()
{
    pluginIndices.clear();

    auto track = GetSelectedTrack2(0, 0, true);
    auto fx_count = TrackFX_GetCount(track);

    char bufOut[64] {0};

    // find fx indices for supported plugins based on names
    for (int i = 0; i < fx_count; i++) {
        TrackFX_GetFXName(track, i, bufOut, 64);
        std::string fx_name(bufOut);
        if (fx_name.find(std::string("FabFilter")) != std::string::npos) {
            for (auto item : pluginNames) {
                if (fx_name.find(item) != std::string::npos) {
                    // ShowConsoleMsg("FabFitler!!");
                    pluginIndices.push_back(i);
                }
            }
        }
    }

    return;
}

void ProcessCommand(int command, int val)
{
    if (command > 24) {
        (void)val;
        currentPlugin = command - 24 - 1;
        ProcessInput();
        if ((int)pluginIndices.size() > currentPlugin) {
            ProcessPlugin(command, val, pluginIndices[currentPlugin]);
            if (TrackFX_GetOpen(
                    GetSelectedTrack2(0, 0, true),
                    pluginIndices[currentPlugin])) {
                TrackFX_Show(
                    GetSelectedTrack2(0, 0, true),
                    pluginIndices[currentPlugin],
                    0);
                TrackFX_Show(
                    GetSelectedTrack2(0, 0, true),
                    pluginIndices[currentPlugin],
                    2);
            }
            else {
                TrackFX_Show(
                    GetSelectedTrack2(0, 0, true),
                    pluginIndices[currentPlugin],
                    1);
            }
        }
        return;
    }
    if (command < 25 && currentPlugin != -1) {
        ProcessInput();
        if ((int)pluginIndices.size() > currentPlugin) {
            ProcessPlugin(command, val, pluginIndices[currentPlugin]);
        }
    }
    return;
}

bool OnAction(
    KbdSectionInfo* sec,
    int command,
    int val,
    int valhw,
    int relmode,
    HWND hwnd)
{
    // treat unused variables 'pedantically'
    (void)sec;
    // (void)val;
    // (void)valhw;
    // (void)relmode;
    (void)hwnd;

    if (!actions[command]) {
        return false;
    }
    // std::string str = std::to_string(val) + " " + std::to_string(valhw) +
    // " "
    // +
    //                   std::to_string(relmode) + " ";

    // ShowConsoleMsg(str.c_str());
    ProcessCommand(actions[command], val);
    return true;
}

void RegisterReaFab()
{
    auto count = 8;
    for (int i = 0; i < count; i++) {
        std::string commandStr {"AK5K_REAFAB"};
        std::string actionStr {"ReaFab Encoder "};
        commandStr = commandStr + std::to_string(i);
        actionStr = actionStr + std::to_string(i + 1);
        custom_action_register_t action =
            {0, commandStr.c_str(), actionStr.c_str(), NULL};
        auto commandId = plugin_register("custom_action", &action);
        actions[commandId] = i + 1;
    }

    for (int i = 0; i < count; i++) {
        std::string commandStr {"AK5K_REAFAB"};
        std::string actionStr {"ReaFab Encoder Push "};
        commandStr = commandStr + std::to_string(i + count);
        actionStr = actionStr + std::to_string(i + 1);
        custom_action_register_t action =
            {0, commandStr.c_str(), actionStr.c_str(), NULL};
        auto commandId = plugin_register("custom_action", &action);
        actions[commandId] = i + count + 1;
    }

    count = 16;
    for (int i = 0; i < count; i++) {
        std::string commandStr {"AK5K_REAFAB"};
        std::string actionStr {"ReaFab Button "};
        commandStr = commandStr + std::to_string(i + count);
        actionStr = actionStr + std::to_string(i + 1);
        custom_action_register_t action =
            {0, commandStr.c_str(), actionStr.c_str(), NULL};
        auto commandId = plugin_register("custom_action", &action);
        actions[commandId] = i + count + 1;
    }

    plugin_register("hookcommand2", (void*)OnAction);

    return;
}

// from cfillion reapack
#define REQUIRED_API(name)                                                     \
    {                                                                          \
        reinterpret_cast<void**>(&name), #name, true                           \
    }

static bool loadAPI(void* (*getFunc)(const char*))
{
    struct ApiFunc {
        void** ptr;
        const char* name;
        bool required;
    };

    const ApiFunc funcs[] {REQUIRED_API(plugin_register)};

    for (const ApiFunc& func : funcs) {
        *func.ptr = getFunc(func.name);
        if (func.required && *func.ptr == nullptr) {
            return false;
        }
    }

    return true;
}
// from cfillion reapack

extern "C" {
REAPER_PLUGIN_DLL_EXPORT int ReaperPluginEntry(
    REAPER_PLUGIN_HINSTANCE hInstance,
    reaper_plugin_info_t* rec)
{
    (void)hInstance;
    if (!rec) {
        return 0;
    }
    else if (
        rec->caller_version != REAPER_PLUGIN_VERSION ||
        // !loadAPI(rec->GetFunc) ||
        REAPERAPI_LoadAPI(rec->GetFunc)) {
        return 0;
    }
    RegisterReaFab();
    return 1;
}
}
